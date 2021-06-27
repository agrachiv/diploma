#ifndef OCL_AHO_H
#define OCL_AHO_H

#include "gpu_aho.h"

class Ocl_aho_corasick : public Gpu_aho_corasick
{
    public:

    private:
        void build_kernel() override;
        void run_kernel() override;
        void run_event( const cl::Kernel& kernel, const cl::NDRange& glob_sz, const cl::NDRange& loc_sz);
};

#endif