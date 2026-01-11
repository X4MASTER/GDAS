cmake_minimum_required(VERSION 3.16)
project(AutoSave)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# GEODE_SDK_ROOT debe pasarse desde CI o configurarse localmente
if(NOT DEFINED GEODE_SDK_ROOT)
  message(FATAL_ERROR "GEODE_SDK_ROOT is not defined. Pass -DGEODE_SDK_ROOT=/path/to/geode-sdk")
endif()

# Incluir el helper oficial del SDK (fallará si GEODE_SDK_ROOT es incorrecto)
include("${GEODE_SDK_ROOT}/cmake/Geode.cmake")

# Fuentes: main.cpp en la raíz del mod, resto en src/
geode_add_mod(AutoSave
    SOURCES
        ${CMAKE_SOURCE_DIR}/main.cpp
        src/AutoSave.cpp
        src/SettingsLayer.cpp
    HEADERS
        src/AutoSave.hpp
        src/SettingsLayer.hpp
)

# Forzar nombre de salida conocido (opcional)
set_target_properties(AutoSave PROPERTIES OUTPUT_NAME "AutoSave")
