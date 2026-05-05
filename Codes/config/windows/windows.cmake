message(STATUS "Windows build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  include(debug.cmake)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  include(release.cmake)
endif()