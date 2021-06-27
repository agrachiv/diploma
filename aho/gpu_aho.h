#ifndef GPU_AHO_H
#define GPU_AHO_H

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 210

#include "raw_aho.h"
#include "CL/cl2.hpp"  

const int TERMINAL = -1;

class Gpu_aho_corasick : public Aho_corasick
{
    public:
        Gpu_aho_corasick() {}
        //Gpu_aho_corasick( std::string text) :  Aho_corasick( text) {}
        void init() override;

    protected:
        cl::Platform platform;
        cl::Device device;
        cl::Context context;
        cl::CommandQueue command_queue;
        cl::Program program;
        cl::Kernel kernel;

        cl::Buffer matched_patterns_buffer;
        cl::Buffer debug_buffer;

        std::string matched_patterns;
        std::vector<int> debug;

        void set_platform();
        void set_device();
        void set_context();
        void set_command_queue();
        virtual void build_kernel() = 0;
        virtual void run_kernel() = 0;
        virtual void run_event( const cl::Kernel& kernel, const cl::NDRange& loc_sz, const cl::NDRange& glob_sz) = 0;

        template <typename T>
        cl::Buffer create_buffer ( T& container, cl_mem_flags flag);
        void process_resulted_buffer( std::string matched_patterns);
        void print_address_device_bits();
};

class Transition_table
{
    public:
        Transition_table( Gpu_aho_corasick* gpu_aho_corasick) 
            : gpu_aho_corasick( gpu_aho_corasick) {};
        void init();
        void print();
        const char* data() { return contiguous_data.data();  }
        const int   size() { return contiguous_data.size();  }
        const auto front() { return contiguous_data.front(); }

    private:
        Gpu_aho_corasick* gpu_aho_corasick;
        std::vector<std::vector<std::pair< int, int>>> vector_data;
        std::string contiguous_data;

        void write_states( Trie_node* state, int* number_of_state);
};

template <typename T>
cl::Buffer Gpu_aho_corasick::create_buffer ( T& container, cl_mem_flags flag)
{
    cl::Buffer created_buffer( context, flag, container.size() * sizeof( container.front()));
    command_queue.enqueueWriteBuffer( created_buffer, CL_TRUE , 0 , container.size() * sizeof( container.front()), container.data());
    return created_buffer;
}

#endif