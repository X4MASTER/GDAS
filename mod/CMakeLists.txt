cmake_minimum_required(VERSION 3.16...3.31)
project(AutoSave)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# GEODE_SDK_ROOT debe pasarse desde CI o configurarse localmente
if(NOT DEFINED GEODE_SDK_ROOT)
  message(FATAL_ERROR "GEODE_SDK_ROOT is not defined. Pass -DGEODE_SDK_ROOT=/path/to/geode-sdk")
endif()

# Incluir el helper oficial del SDK
include("${GEODE_SDK_ROOT}/cmake/Geode.cmake")

# Fuentes y cabeceras
geode_add_mod(AutoSave
    SOURCES
        ${CMAKE_SOURCE_DIR}/main.cpp
        src/AutoSave.cpp
        src/SettingsLayer.cpp
    HEADERS
        src/AutoSave.hpp
        src/SettingsLayer.hpp
    RESOURCES
        mod.json
)

# Nombre de salida opcional
set_target_properties(AutoSave PROPERTIES OUTPUT_NAME "AutoSave")
