#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 210

#include "raw_aho.h"
#include "CL/cl2.hpp"  

const int TERMINAL = -1;

class Ocl_aho_corasick : public Aho_corasick
{
    public:
        Ocl_aho_corasick( std::string text) :  Aho_corasick( text) {}
        void init() override;
        void run_kernel();

    private:
        cl::Platform platform;
        cl::Device device;
        cl::Context context;
        cl::CommandQueue command_queue;
        cl::Program program;

        void set_platform();
        void set_device();
        void set_context();
        void set_command_queue();
        void build_kernel();

        template <typename T>
        cl::Buffer create_buffer ( T& container);
        void run_event( const cl::Kernel& kernel, const cl::NDRange& loc_sz, const cl::NDRange& glob_sz);
};

class Transition_table
{
    public:
        Transition_table( Ocl_aho_corasick* ocl_aho_corasick) 
            : ocl_aho_corasick( ocl_aho_corasick) {};
        void init();
        void print();
        char* data() { return contiguous_data.data();  }
        int   size() { return contiguous_data.size();  }
        auto front() { return contiguous_data.front(); }

    private:
        Ocl_aho_corasick* ocl_aho_corasick;
        std::vector<std::vector<std::pair< int, int>>> vector_data;
        std::string contiguous_data;

        void write_states( Trie_node* state, int* number_of_state);
};

template <typename T>
cl::Buffer Ocl_aho_corasick::create_buffer ( T& container)
{
    std::cout << sizeof( container.front()) << std::endl;
    cl::Buffer created_buffer( context, CL_MEM_READ_WRITE , container.size() * sizeof( container.front()));
    command_queue.enqueueWriteBuffer( created_buffer, CL_TRUE , 0 , container.size() * sizeof( container.front()), container.data());
    return created_buffer;
}

int main()
{
   Ocl_aho_corasick test( std::string( "XXAAATCGAAAA"));
   test.add_word( "ACGCC");
   test.add_word( "AAA");
   test.add_word( "AAATCG");
   test.add_word( "AAATTG");

   test.init();
   test.run_kernel();
}