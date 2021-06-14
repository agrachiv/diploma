#include "raw_aho.h"
#include "CL/cl2.hpp"

class ocl_aho_corasick : public aho_corasick
{
    public:
        //ocl_aho_corasick() : aho_corasick() {} //why do I need it?
        void init() override;
    private:
        cl::Platform default_platform;
        cl::Device default_device;
        void set_platform();
        void set_device();
};

int main()
{
   ocl_aho_corasick test;
   test.init();  
}