cmake_minimum_required(VERSION 3.16)
project(MiniEDR CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- third_party include ---
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
    ${CMAKE_SOURCE_DIR}/third_party/spdlog/include
    ${CMAKE_SOURCE_DIR}/bpf
)

# eBPF 관련 CMake는 Day 3~5에서 추가 예정 (지금은 core 인프라만 빌드)
file(GLOB_RECURSE CORE_SOURCES
    src/core/*.cpp
)

add_executable(miniedr_core_test
    src/main.cpp
    ${CORE_SOURCES}
)

add_custom_command(TARGET miniedr_core_test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	        ${CMAKE_SOURCE_DIR}/config ${CMAKE_BINARY_DIR}/config
)

target_link_libraries(miniedr_core_test pthread)
