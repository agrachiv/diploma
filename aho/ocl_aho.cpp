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
    platform = cl::Platform::setDefault( selected_platform);
    assert( platform == selected_platform && "Error setting default platform");
    std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
}

void ocl_aho_corasick::set_device()
{
    std::vector<cl::Device> devices;
    platform.getDevices( CL_DEVICE_TYPE_GPU, &devices);
    assert( devices.size() != 0 && "No devices found");
    device = devices[0];
    std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
}

void ocl_aho_corasick::set_context() { context = cl::Context( device); } 

void ocl_aho_corasick::set_command_queue() { command_queue = cl::CommandQueue( context, device); } //3 param?

void ocl_aho_corasick::build_kernel()
{
    std::ifstream kernel_file( "kernel.cl");
    assert( kernel_file.is_open() && "Can't open file kernel.cl");
    std::string kernel_code( std::istreambuf_iterator<char>( kernel_file), (std::istreambuf_iterator<char>())); //remove bracket?
    program = cl::Program( context, kernel_code);
    try
    {
        std::ostringstream options;
        //options << " -D OPTION_NAME=" << value;
        program.build( options.str().data());
    }
    catch ( cl::Error error)
    {
        std::cout << "Kernel build failed: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>( device) << std::endl;
        exit(1);
    }
}

void ocl_aho_corasick::init()
{
    vocabluary.number_of_entries.resize( vocabluary.words.size());
    vocabluary.entries.resize( vocabluary.words.size());
    set_platform();
    set_device();
    set_context();
    build_kernel();
    init_transition_table();
    set_command_queue();

    
    for ( auto& i : transition_table)
    {
        for ( int j = 0; j < i.size(); j++)
        {
            int a = i[j].first;
            int b = i[j].second;
            if ( a != 0 || b != 0)
                std::cout << (char)j << "[" << a << "," << b << "] ";
        }
        std::cout << std::endl;
    }
    std::string stupid_string( "0123456789012");
    std::cout << stupid_string.size() << std::endl;
    cl::Buffer hello_world_buffer = create_buffer<std::string>( stupid_string);

    cl::Kernel kernel(program, "HelloWorld");
    kernel.setArg(0, hello_world_buffer);

    cl::Event event;
    int err = command_queue.enqueueNDRangeKernel( kernel, 0, 1, 1, nullptr, &event);
    assert( err == CL_SUCCESS && "Can't create buffer");
    char returned_string[16];
    //assert( sizeof(returned_string) == stupid_string.size() * sizeof( stupid_string.front()));
    std::cout << "1 " << sizeof(returned_string) << std::endl << "2 " << stupid_string.size() * sizeof( stupid_string.front()) << std::endl;
    command_queue.enqueueReadBuffer( hello_world_buffer, CL_TRUE, 0, stupid_string.size() * sizeof( stupid_string.front()), returned_string);
    //std::cin.get();
    std::cout << returned_string;
}

void ocl_aho_corasick::run_event( const cl::Kernel& kernel, const cl::NDRange& loc_sz, const cl::NDRange& glob_sz)
{
    cl::Event event;
    command_queue.enqueueNDRangeKernel( kernel , 0 , glob_sz , loc_sz , nullptr , &event);
    event.wait();
}

//template cl::Buffer ocl_aho_corasick::create_buffer<std::string>( std::string); //Ask Kostya

void ocl_aho_corasick::init_transition_table()
{
    int number_of_state = 0;
    transition_table = std::vector<std::vector<std::pair< int, int>>>( size(), std::vector<std::pair< int, int>>( std::numeric_limits<char>::max(), std::make_pair( 0, 0))); //split
    write_states( &root, &number_of_state);
}

void ocl_aho_corasick::write_states( trie_node* state, int* number_of_state)
{
    for ( auto& [symbol, child_state] : state->child_links)
    {
        std::cout << "|" << (char)symbol << "|" << std::endl;
        if ( child_state->is_terminal == false)
        {
            transition_table.at( state->depth).at( symbol) = std::make_pair( ++(*number_of_state), 0);
            write_states( child_state, number_of_state);
        } 
        else
        {
            int matched_pattern_id = child_state->vocabluary_index + 1;
            transition_table.at( state->depth).at( symbol) = std::make_pair( ++(*number_of_state), matched_pattern_id);
        }
    }
}