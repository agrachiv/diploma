cmake_minimum_required(VERSION 3.10)

find_package(OpenCL REQUIRED)

option(RAW "cpu" OFF)
option(OCL "ocl" OFF)

if (RAW)
    message("Building OCL version")
    add_executable(raw raw_aho.cpp)
endif(RAW)

if (OCL)
    message("Building OCL version")
    add_executable(ocl ocl_aho.cpp)
    target_include_directories(ocl PUBLIC ../OpenCL)
    target_link_libraries(ocl OpenCL::OpenCL)
endif(OCL)