cmake_minimum_required(VERSION 3.10)
project(AutoSave)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT DEFINED GEODE_SDK_ROOT)
  set(GEODE_SDK_ROOT "${CMAKE_SOURCE_DIR}/geode-sdk" CACHE PATH "Path to Geode SDK")
endif()

if(EXISTS "${GEODE_SDK_ROOT}/cmake/Geode.cmake")
  include("${GEODE_SDK_ROOT}/cmake/Geode.cmake")
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB SRC_FILES "src/*.cpp" "src/*.c" "src/*.cc")

add_library(AutoSave SHARED ${SRC_FILES})

if(TARGET geode)
  target_link_libraries(AutoSave geode)
endif()

set_target_properties(AutoSave PROPERTIES OUTPUT_NAME "libAutoSave")

