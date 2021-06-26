cmake_minimum_required(VERSION 3.10)
project(Aho_Corasick)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(OpenCL REQUIRED)

option(CPU "Build raw cpu version" ON)
option(OCL "Build OpenCL version" OFF)
option(BENCH "Build benchmarks version" ON)

if(CPU)
    message("Building OCL version")
    add_library(rawModule raw_aho.cpp)
    add_executable(cpu cpu_unit_test.cpp)
    target_link_libraries(cpu rawModule)
endif(CPU)

if(OCL)
    message("Building OCL version")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Werror=return-type") #why doesn't it work without this shit
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

    add_library(LibsModule raw_aho.cpp)
    add_executable(ocl ocl_aho.cpp)
    target_include_directories(ocl PUBLIC ../OpenCL)
    target_link_libraries(ocl OpenCL::OpenCL)
    target_link_libraries(ocl LibsModule)
endif(OCL)

if(BENCH)
    message("Building benchmarks")

    link_directories(${CMAKE_SOURCE_DIR}/../google/benchmark/install/lib/)
    set(benchmark_DIR ${CMAKE_SOURCE_DIR}/../google/benchmark/install/lib/cmake/benchmark/)
    find_package(benchmark REQUIRED)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Werror=return-type -isystem")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

    add_library(LibsModule2 raw_aho.cpp ocl_aho.cpp gpu_aho.cpp)
    add_executable(bench ../benchmarks/ocl_benchmark.cpp)
    link_directories(${CMAKE_SOURCE_DIR}/../google/benchmark/install/lib/)
    target_include_directories(bench PUBLIC ../OpenCL ../google/benchmark/include ../aho)  
    target_link_libraries(bench OpenCL::OpenCL benchmark::benchmark LibsModule2)

endif(BENCH)