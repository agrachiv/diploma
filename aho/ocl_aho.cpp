#include "ocl_aho.h"

void ocl_aho_corasick::set_platform()
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get( &platforms);
    cl::Platform selected_platform;
    for ( auto &p : platforms)
    {
        std::string platver = p.getInfo<CL_PLATFORM_VERSION>();
        std::cout << platver << std::endl;
        //if (platver.find("OpenCL 2.") != std::string::npos)
            selected_platform = p;
    }
    assert( selected_platform() != 0 && "No OpenCL 2.0 platform found.\n");
    default_platform = cl::Platform::setDefault( selected_platform);
    assert( default_platform == selected_platform && "Error setting default platform");
    std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
}

void ocl_aho_corasick::set_device()
{
    std::vector<cl::Device> devices;
    default_platform.getDevices( CL_DEVICE_TYPE_GPU, &devices);
    assert( devices.size() != 0 && "No devices found");
    default_device = devices[0];
    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << std::endl;
}

void ocl_aho_corasick::init()
{
    vocabluary.number_of_entries.resize( vocabluary.words.size());
    vocabluary.entries.resize( vocabluary.words.size());
    set_platform();
    set_device();
}
