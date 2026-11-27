#!/usr/bin/env bash
# =============================================================================
# LearnCPP Linux 一键构建脚本 (Debug)
#   用法:
#     ./linux-debug-build.sh             # 配置 + 编译 Debug
#     ./linux-debug-build.sh --clean     # 清理 build 目录后全新配置(默认增量复用)
#     ./linux-debug-build.sh -j 16       # 指定并行度
#   说明:
#     - 工具链自动探测:C++23 优先 $HOME/.opt/gcc-16.2,可用 TOOLCHAIN_DIR 覆盖
#       (须同时用于 C 与 C++,避免混用系统 gcc)
#     - PRJ_LIB_ROOT 指向第三方库安装根,默认 third_party_debug
#     - 强制 C++23:CMakeLists.txt 中的 CMAKE_CXX_STANDARD 目前是注释状态
#     - gRPC 与本项目无关,默认不构建(--with-grpc 可开启)
# =============================================================================
set -euo pipefail

# ----------------------------------------------------------------------------
# 可调配置
# ----------------------------------------------------------------------------
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PRJ_LIB_ROOT="${PRJ_LIB_ROOT:-/home/JC1136/code/third_party_debug}"
GRPC_SRC="${GRPC_SRC:-/home/JC1136/code/third_party_source/grpc}"
GRPC_PREFIX="${PRJ_LIB_ROOT}/grpc"
GRPC_BUILD="${GRPC_SRC}/build"

BUILD_TYPE="${BUILD_TYPE:-Debug}"
JOBS="$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 8)"
BUILD_GRPC=0
CLEAN=0
BUILD_DIR="${ROOT_DIR}/out/build/linux-${BUILD_TYPE,,}"

log() { printf '\n\033[1;32m[build] %s\033[0m\n' "$*"; }

# ----------------------------------------------------------------------------
# 工具链探测(C / C++ 必须来自同一套编译器的两个前端)
#     优先显式 TOOLCHAIN_DIR,其次 $HOME/.opt/gcc-16.2、$HOME/cpp20,最后 PATH
#     并用 -std=c++23 实际探测,避免拿到不支持 C++23 的旧 gcc
# ----------------------------------------------------------------------------
resolve_toolchain() {
    local dir cxx gcc
    local -a cands=()
    [[ -n "${TOOLCHAIN_DIR:-}" ]] && cands+=("${TOOLCHAIN_DIR}")
    cands+=("${HOME}/.opt/gcc-16.2" "${HOME}/cpp20")

    for dir in "${cands[@]}"; do
        if [[ -x "${dir}/bin/x86_64-conda-linux-gnu-g++" ]]; then
            cxx="${dir}/bin/x86_64-conda-linux-gnu-g++"
            gcc="${dir}/bin/x86_64-conda-linux-gnu-gcc"
        elif [[ -x "${dir}/bin/g++" ]]; then
            cxx="${dir}/bin/g++"
            gcc="${dir}/bin/gcc"
        else
            continue
        fi
        if echo | "${cxx}" -std=c++23 -x c++ -E - >/dev/null 2>&1; then
            TOOLCHAIN_DIR="${dir}"; CXX_BIN="${cxx}"; CC_BIN="${gcc}"
            return 0
        fi
        echo "[warn] ${cxx} 不支持 -std=c++23,跳过" >&2
    done

    # 回退到 PATH 中的 gcc/g++,但必须支持 C++23
    if cxx="$(command -v g++)" && echo | "${cxx}" -std=c++23 -x c++ -E - >/dev/null 2>&1; then
        TOOLCHAIN_DIR=""; CXX_BIN="${cxx}"; CC_BIN="$(command -v gcc)"
        echo "[warn] 未找到 C++23 工具链目录,回退到 PATH: ${CXX_BIN}" >&2
        return 0
    fi

    echo "错误: 找不到支持 C++23 的 g++(可用 TOOLCHAIN_DIR 指定)" >&2
    exit 1
}

resolve_toolchain
# libstdc++ 所在目录:用于运行时库路径,免去手动 LD_LIBRARY_PATH
TOOLCHAIN_LIB_DIR="$(dirname "$("${CXX_BIN}" -print-file-name=libstdc++.so.6)")"
CMAKE_BIN="${CMAKE_BIN:-$(command -v cmake || echo "${HOME}/.opt/python/bin/cmake")}"
NINJA_BIN="${NINJA_BIN:-$(command -v ninja || echo "")}"

# ----------------------------------------------------------------------------
# 参数解析
# ----------------------------------------------------------------------------
while [[ $# -gt 0 ]]; do
    case "$1" in
        --with-grpc)      BUILD_GRPC=1; shift ;;
        --skip-grpc)      BUILD_GRPC=0; shift ;;   # 兼容旧用法
        --clean)          CLEAN=1; shift ;;
        -j|--jobs)        JOBS="$2"; shift 2 ;;
        -h|--help)
            sed -n '2,15p' "$0" | sed 's/^# \{0,1\}//'
            exit 0 ;;
        *) echo "未知参数: $1" >&2; exit 1 ;;
    esac
done

export PRJ_LIB_ROOT
export LD_LIBRARY_PATH="${TOOLCHAIN_LIB_DIR}:${LD_LIBRARY_PATH:-}"

log "工具链: ${CXX_BIN}"
log "第三方库根: ${PRJ_LIB_ROOT}"

# ----------------------------------------------------------------------------
# 1. [可选] gRPC 构建 + 安装(本项目默认不需要)
# ----------------------------------------------------------------------------
grpc_installed() { [[ -f "${GRPC_PREFIX}/lib/cmake/grpc/gRPCConfig.cmake" ]]; }

if [[ ${BUILD_GRPC} -eq 1 ]]; then
    CMAKE_GEN_ARGS=()
    [[ -n "${NINJA_BIN}" ]] && CMAKE_GEN_ARGS=(-G Ninja -DCMAKE_MAKE_PROGRAM="${NINJA_BIN}")
    if grpc_installed; then
        log "gRPC 已安装于 ${GRPC_PREFIX},跳过构建"
    else
        log "[1/3] 配置 gRPC (${GRPC_SRC})"
        "${CMAKE_BIN}" -S "${GRPC_SRC}" -B "${GRPC_BUILD}" \
            "${CMAKE_GEN_ARGS[@]}" \
            -DCMAKE_C_COMPILER="${CC_BIN}" \
            -DCMAKE_CXX_COMPILER="${CXX_BIN}" \
            -DCMAKE_BUILD_TYPE=Debug \
            -DCMAKE_INSTALL_PREFIX="${GRPC_PREFIX}"

        log "[1/3] 编译并安装 gRPC (make install -j${JOBS})"
        make -C "${GRPC_BUILD}" install -j"${JOBS}"

        log "[1/3] 强制安装到 ${GRPC_PREFIX} (gRPC 会内部覆盖 CMAKE_INSTALL_PREFIX)"
        "${CMAKE_BIN}" --install "${GRPC_BUILD}" --prefix "${GRPC_PREFIX}"
    fi
fi

# ----------------------------------------------------------------------------
# 2. 配置 LearnCPP
# ----------------------------------------------------------------------------
log "[2/3] 配置 LearnCPP (${BUILD_TYPE}, build dir: ${BUILD_DIR})"
if [[ ${CLEAN} -eq 1 && -d "${BUILD_DIR}" ]]; then
    log "    --clean: 清理 ${BUILD_DIR}"
    rm -rf "${BUILD_DIR}"
fi

GEN_ARGS=()
if [[ -n "${NINJA_BIN}" ]]; then
    GEN_ARGS=(-G Ninja -DCMAKE_MAKE_PROGRAM="${NINJA_BIN}")
fi

"${CMAKE_BIN}" -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
    "${GEN_ARGS[@]}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_CXX_STANDARD=23 \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_C_COMPILER="${CC_BIN}" \
    -DCMAKE_CXX_COMPILER="${CXX_BIN}" \
    -DCMAKE_BUILD_RPATH="${TOOLCHAIN_LIB_DIR};${PRJ_LIB_ROOT}/opencv/lib"

# ----------------------------------------------------------------------------
# 3. 编译 LearnCPP
# ----------------------------------------------------------------------------
log "[3/3] 编译 LearnCPP (ninja -j${JOBS})"
if [[ -n "${NINJA_BIN}" ]]; then
    "${NINJA_BIN}" -C "${BUILD_DIR}" -j"${JOBS}"
else
    "${CMAKE_BIN}" --build "${BUILD_DIR}" -j"${JOBS}"
fi

log "完成: ${BUILD_DIR}/LearnCPP"
log "测试: ${BUILD_DIR}/bin/LearnCPP_Test"
