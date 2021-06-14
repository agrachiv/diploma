#include "ocl_aho.h"

void set_platform()
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get( &platforms);
    cl::Platform selected_platform;
    for ( auto &p : platforms)
    {
        std::string platver = p.getInfo<CL_PLATFORM_VERSION>();
        std::cout << platver << std::endl;
        if (platver.find("OpenCL 2.") != std::string::npos)
            selected_platform = p;
    }
    assert( selected_platform() != 0 && "No OpenCL 2.0 platform found.\n");

    cl::Platform default_platform = cl::Platform::setDefault( selected_platform);

    assert( default_platform == selected_platform && "Error setting default platform");
}
