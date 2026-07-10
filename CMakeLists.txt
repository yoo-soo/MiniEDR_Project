cmake_minimum_required(VERSION 3.16)
project(MiniEDR CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBBPF REQUIRED libbpf)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
    ${CMAKE_SOURCE_DIR}/third_party/spdlog/include
    ${CMAKE_SOURCE_DIR}/bpf
    ${LIBBPF_INCLUDE_DIRS}
)

set(BPF_SRC ${CMAKE_SOURCE_DIR}/bpf/processExec.bpf.c)
set(BPF_OBJ ${CMAKE_SOURCE_DIR}/bpf/processExec.bpf.o)
set(BPF_SKEL ${CMAKE_SOURCE_DIR}/bpf/processExec.skel.h)

add_custom_command(
    OUTPUT ${BPF_OBJ}
    COMMAND clang -g -O2 -target bpf -D__TARGET_ARCH_x86
            -I${CMAKE_SOURCE_DIR}/bpf
            -c ${BPF_SRC} -o ${BPF_OBJ}
    DEPENDS ${BPF_SRC} ${CMAKE_SOURCE_DIR}/bpf/processExec.h
    COMMENT "Compiling BPF program ${BPF_SRC} to ${BPF_OBJ}"
)

add_custom_command(
    OUTPUT ${BPF_SKEL}
    COMMAND bpftool gen skeleton ${BPF_OBJ} > ${BPF_SKEL}
    DEPENDS ${BPF_OBJ}
    COMMENT "Generating processExec.skel.h"
)

# 위 두 custom_command는 "누가 이 파일을 필요로 할 때만" 실행된다.
# 그래서 이 타겟을 만들어 실제 빌드 타겟이 의존하게 만들어야 한다.
add_custom_target(bpf_skeletons DEPENDS ${BPF_SKEL})

# --- C++ 소스 ---
file(GLOB_RECURSE CORE_SOURCES src/core/*.cpp)
file(GLOB_RECURSE COLLECTOR_SOURCES src/collectors/*.cpp)

add_executable(miniedr_core_test
    src/main.cpp
    ${CORE_SOURCES}
    ${COLLECTOR_SOURCES}
)

add_dependencies(miniedr_core_test bpf_skeletons)

target_link_libraries(miniedr_core_test pthread ${LIBBPF_LIBRARIES} elf z)

add_custom_command(TARGET miniedr_core_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/config ${CMAKE_BINARY_DIR}/config
)

target_link_libraries(miniedr_core_test pthread)
