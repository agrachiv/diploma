#include "cm_aho.h"

void Cm_aho_corasick::build_kernel()
{
    std::ifstream kernel_file( "aho_genx.cpp");
    assert( kernel_file.is_open() && "Can't open file kernel.cl");
    std::string kernel_code( std::istreambuf_iterator<char>( kernel_file), (std::istreambuf_iterator<char>())); //remove bracket?
    program = cl::Program( context, kernel_code);
    
    try
    {
        std::ostringstream options;
        options << " -cmc -Werror";
        program.build( options.str().data());
    }
    catch ( cl::Error error)
    {
        std::cout << "Kernel build failed: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>( device) << std::endl;
        exit(1);
    }
}

void Cm_aho_corasick::run_kernel()
{
    set_command_queue();

    Transition_table transition_table( this);
    transition_table.init();
    //transition_table.print();

    matched_patterns = std::string( text.size() * dictionary.words.size(), '0');
    //debug = std::vector<int>( text.size(), -99);

    try
    {
        cl::Buffer transition_table_buffer = create_buffer<Transition_table>( transition_table, CL_MEM_READ_ONLY);
        cl::Buffer text_buffer = create_buffer<std::string>( text, CL_MEM_READ_ONLY);
        cl::Buffer matched_patterns_buffer = create_buffer<std::string>( matched_patterns, CL_MEM_READ_WRITE);

        kernel = cl::Kernel( program, "Find_matches");

        kernel.setArg( 0, transition_table_buffer);
        kernel.setArg( 1, text_buffer);
        kernel.setArg( 2, matched_patterns_buffer);
        kernel.setArg( 3, dictionary.words.size());
        kernel.setArg( 4, text.size());

        run_event( kernel, text.size(), 1);

        cl::copy( command_queue, matched_patterns_buffer, matched_patterns.begin(), matched_patterns.end());

        command_queue.finish();
        process_resulted_buffer( matched_patterns);
    }
    catch ( cl::Error err)
    {
        std::cout << std::endl << err.what() << " : " << err.err() << std::endl;
    }
}

void Cm_aho_corasick::run_event( const cl::Kernel& kernel, const cl::NDRange& glob_sz, const cl::NDRange& loc_sz)
{

    cl::Event event;
    //print_address_device_bits();
    command_queue.enqueueNDRangeKernel( kernel , 0, glob_sz , loc_sz, nullptr, &event);
    event.wait();

}