#!/usr/bin/env bash
# =============================================================================
# AFSS Linux 一键构建脚本 (Release)
#   用法:
#     ./linux-release-build.sh             # 构建 gRPC(如未安装) + 配置 + 编译 AFSS
#     ./linux-release-build.sh --skip-grpc # 跳过 gRPC 构建(仅配置 + 编译 AFSS)
#     ./linux-release-build.sh --clean     # 清理 build 目录后全新配置(默认增量复用)
#     ./linux-release-build.sh -j 16       # 指定并行度
#   说明:
#     - 工具链使用 ~/cpp20 下的 conda gcc(须同时用于 C 与 C++,避免混用系统 gcc)
#     - PRJ_LIB_ROOT 指向第三方库安装根(与 config/*.cmake 布局一致)
#     - 构建类型默认 Release(与 release 库一致),可传 BUILD_TYPE 覆盖
#     - OpenSSL: 使用 ${PRJ_LIB_ROOT}/openssl3 的真实 OpenSSL 3(hc_writer 的 EVP_MAC 需要)
# =============================================================================
set -euo pipefail

# ----------------------------------------------------------------------------
# 可调配置
# ----------------------------------------------------------------------------
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# ~/.zshrc 全局导出 PRJ_LIB_ROOT=/home/JC1136/code/third_party_debug,不能用于 Release 构建。
# 这里仅忽略指向 debug 库根的继承值,其余显式设置仍生效:
if [[ "${PRJ_LIB_ROOT:-}" == *"/third_party_debug"* ]]; then
    echo "[warn] 忽略环境中的 PRJ_LIB_ROOT=${PRJ_LIB_ROOT}(debug 库根),Release 使用 /home/JC1136/code/third_party" >&2
    PRJ_LIB_ROOT="/home/JC1136/code/third_party"
fi
PRJ_LIB_ROOT="${PRJ_LIB_ROOT:-/home/JC1136/code/third_party}"
GRPC_SRC="${GRPC_SRC:-/home/JC1136/code/third_party_source/grpc}"
GRPC_PREFIX="${PRJ_LIB_ROOT}/grpc"
GRPC_BUILD="${GRPC_SRC}/build-release"

# 真实 OpenSSL 3(hc_writer.cc 使用 EVP_MAC/OSSL_PARAM,BoringSSL 不提供)。
# 装在 ${PRJ_LIB_ROOT}/openssl3,与 gRPC 依赖的 BoringSSL 并存互不覆盖。
OPENSSL3_ROOT="${PRJ_LIB_ROOT}/openssl3"
OPENSSL3_INC="${OPENSSL3_ROOT}/include"
# gRPC(BoringSSL)与 OpenSSL3 的 libcrypto.a 存在同名符号,链接时让 BoringSSL 优先
# (其出现在链接行更前),OpenSSL3 仅提供 BoringSSL 缺少的 EVP_MAC_* 符号。
LINK_RELAX_FLAGS="-Wl,--allow-multiple-definition"

TOOLCHAIN_DIR="${TOOLCHAIN_DIR:-/home/JC1136/cpp20}"
CC_BIN="${TOOLCHAIN_DIR}/bin/x86_64-conda-linux-gnu-gcc"
CXX_BIN="${TOOLCHAIN_DIR}/bin/x86_64-conda-linux-gnu-g++"
CMAKE_BIN="${CMAKE_BIN:-/home/JC1136/.opt/python/bin/cmake}"
NINJA_BIN="${NINJA_BIN:-/home/JC1136/.opt/python/bin/ninja}"

BUILD_TYPE="${BUILD_TYPE:-Release}"
JOBS="$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 8)"
BUILD_GRPC=1
CLEAN=0
BUILD_DIR="${ROOT_DIR}/out/build/linux-${BUILD_TYPE,,}"

# ----------------------------------------------------------------------------
# 参数解析
# ----------------------------------------------------------------------------
while [[ $# -gt 0 ]]; do
    case "$1" in
        --skip-grpc)      BUILD_GRPC=0; shift ;;
        --clean)          CLEAN=1; shift ;;
        -j|--jobs)        JOBS="$2"; shift 2 ;;
        -h|--help)
            sed -n '2,14p' "$0" | sed 's/^# \{0,1\}//'
            exit 0 ;;
        *) echo "未知参数: $1" >&2; exit 1 ;;
    esac
done

export PRJ_LIB_ROOT
export LD_LIBRARY_PATH="${TOOLCHAIN_DIR}/lib:${LD_LIBRARY_PATH:-}"

log() { printf '\n\033[1;32m[build] %s\033[0m\n' "$*"; }

# ----------------------------------------------------------------------------
# 1. gRPC 构建 + 安装
# ----------------------------------------------------------------------------
grpc_installed() { [[ -f "${GRPC_PREFIX}/lib/cmake/grpc/gRPCConfig.cmake" ]]; }

if [[ ${BUILD_GRPC} -eq 1 ]]; then
    if grpc_installed; then
        log "gRPC 已安装于 ${GRPC_PREFIX},跳过构建(--skip-grpc 可强制跳过)"
    else
        log "[1/3] 配置 gRPC (${GRPC_SRC}, Release)"
        "${CMAKE_BIN}" -S "${GRPC_SRC}" -B "${GRPC_BUILD}" \
            -DCMAKE_C_COMPILER="${CC_BIN}" \
            -DCMAKE_CXX_COMPILER="${CXX_BIN}" \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX="${GRPC_PREFIX}"

        log "[1/3] 编译并安装 gRPC (make install -j${JOBS})"
        make -C "${GRPC_BUILD}" install -j"${JOBS}"

        log "[1/3] 强制安装到 ${GRPC_PREFIX} (gRPC 会内部覆盖 CMAKE_INSTALL_PREFIX)"
        "${CMAKE_BIN}" --install "${GRPC_BUILD}" --prefix "${GRPC_PREFIX}"
    fi
else
    [[ -f "${GRPC_PREFIX}/lib/cmake/grpc/gRPCConfig.cmake" ]] || {
        log "错误: ${GRPC_PREFIX} 未安装(使用了 --skip-grpc)"; exit 1; }
fi

# ----------------------------------------------------------------------------
# 2. 配置 AFSS
# ----------------------------------------------------------------------------
log "[2/3] 配置 AFSS (${BUILD_TYPE}, build dir: ${BUILD_DIR})"
[[ -f "${OPENSSL3_ROOT}/lib/libcrypto.a" ]] || {
    log "错误: 缺少真实 OpenSSL 3 (${OPENSSL3_ROOT})。hc_writer.cc 需要 EVP_MAC API。"
    log "     请将 OpenSSL 3 静态库+头文件安装到 ${OPENSSL3_ROOT}(参考 debug 树 openssl3 目录)"
    exit 1; }
if [[ ${CLEAN} -eq 1 && -d "${BUILD_DIR}" ]]; then
    log "    --clean: 清理 ${BUILD_DIR}"
    rm -rf "${BUILD_DIR}"
fi

"${CMAKE_BIN}" -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_MAKE_PROGRAM="${NINJA_BIN}" \
    -DCMAKE_C_COMPILER="${CC_BIN}" \
    -DCMAKE_CXX_COMPILER="${CXX_BIN}" \
    -DCMAKE_C_FLAGS="-I${OPENSSL3_INC}" \
    -DCMAKE_CXX_FLAGS="-I${OPENSSL3_INC}" \
    -DCMAKE_EXE_LINKER_FLAGS="${LINK_RELAX_FLAGS}" \
    -DOPENSSL_ROOT_DIR="${OPENSSL3_ROOT}" \
    -DOPENSSL_INCLUDE_DIR="${OPENSSL3_INC}" \
    -DOPENSSL_CRYPTO_LIBRARY="${OPENSSL3_ROOT}/lib/libcrypto.a" \
    -DOPENSSL_SSL_LIBRARY="${OPENSSL3_ROOT}/lib/libssl.a" \
    -DOPENSSL_USE_STATIC_LIBS=TRUE

# ----------------------------------------------------------------------------
# 3. 编译 AFSS
# ----------------------------------------------------------------------------
log "[3/3] 编译 AFSS (ninja -j${JOBS})"
"${NINJA_BIN}" -C "${BUILD_DIR}" -j"${JOBS}"

log "完成: ${BUILD_DIR}/AFSS"
