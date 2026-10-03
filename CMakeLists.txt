cmake_minimum_required(VERSION 3.20)

project(
    embedded_telemetry_cpp
    VERSION 0.1.0
    LANGUAGES CXX)

add_executable(
    telemetry_demo
    src/main.cpp
)

target_compile_features(
    telemetry_demo
    PRIVATE cxx_std_20
)

target_compile_options(
    telemetry_demo
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

set_target_properties(
    telemetry_demo
    PROPERTIES
        CXX_EXTENSIONS OFF
)