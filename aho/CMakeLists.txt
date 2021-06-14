cmake_minimum_required(VERSION 3.10)

option(RAW "cpu" ON)
option(OCL "ocl" OFF)

if (RAW)
    message("Building cpu version")
    add_executable(raw raw_aho.cpp)
endif(RAW)

if (OCL)
    add_library( OCL_diag STATIC OCL_diag.cpp)
    target_include_directories(OCL_diag PUBLIC ../core)
    target_include_directories(OCL_diag PUBLIC ../OpenCL)
    target_link_libraries(OCL_diag OpenCL::OpenCL)
endif(OCL)