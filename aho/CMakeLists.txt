cmake_minimum_required(VERSION 3.10)
project(Aho_Corasick)

find_package(OpenCL REQUIRED)

option(CPU "Build raw cpu version" OFF)
option(OCL "Build OpenCL version" OFF)

if(CPU)
    message("Building OCL version")
    add_library(rawModule raw_aho.cpp)
    add_executable(cpu main.cpp)
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