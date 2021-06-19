#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 210

#include "raw_aho.h"
#include "CL/cl2.hpp"

class ocl_aho_corasick : public aho_corasick
{
    public:
        //ocl_aho_corasick() : aho_corasick() {} //why do I need it?
        void init() override;

    private:
        cl::Platform platform;
        cl::Device device;
        cl::Context context;
        cl::CommandQueue command_queue;
        cl::Program program;

        std::vector<std::vector<std::pair< int, int>>> transition_table;

        void set_platform();
        void set_device();
        void set_context();
        void set_command_queue();
        void build_kernel();

        template <typename T>
        cl::Buffer create_buffer ( T& container);
        void run_event( const cl::Kernel& kernel, const cl::NDRange& loc_sz, const cl::NDRange& glob_sz);

        void write_states( trie_node* state, int* number_of_state);
        void init_transition_table();
};

template <typename T>
cl::Buffer ocl_aho_corasick::create_buffer ( T& container)
{
    std::cout << sizeof( container.front()) << std::endl;
    cl::Buffer created_buffer( context, CL_MEM_READ_WRITE , container.size() * sizeof( container.front()));
    command_queue.enqueueWriteBuffer( created_buffer, CL_TRUE , 0 , container.size() * sizeof( container.front()), container.data());
    std::cout << "huyhuyhuyhuyhuy" << std::endl;
    return created_buffer;
}

int main()
{
   ocl_aho_corasick test;
   test.add_word("TACGCC");
   test.add_word("AAATCG");
   test.add_word("AAATTG");
   

   test.init();
}