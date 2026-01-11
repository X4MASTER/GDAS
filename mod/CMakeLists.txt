cmake_minimum_required(VERSION 3.16)
project(AutoSave)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT DEFINED GEODE_SDK_ROOT)
  set(GEODE_SDK_ROOT "${CMAKE_SOURCE_DIR}/geode-sdk" CACHE PATH "Path to Geode SDK")
endif()

include("${GEODE_SDK_ROOT}/cmake/Geode.cmake")

geode_add_mod(AutoSave
    SOURCES
        ${CMAKE_SOURCE_DIR}/main.cpp
        src/AutoSave.cpp
        src/SettingsLayer.cpp
    HEADERS
        src/AutoSave.hpp
        src/SettingsLayer.hpp
)
