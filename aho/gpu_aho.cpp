#include "gpu_aho.h"

void Gpu_aho_corasick::set_platform()
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

void Gpu_aho_corasick::set_device()
{
    std::vector<cl::Device> devices;
    platform.getDevices( CL_DEVICE_TYPE_GPU, &devices);
    assert( devices.size() != 0 && "No devices found");
    device = devices[0];
    std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    std::cout << "Local memory size: " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
}

void Gpu_aho_corasick::set_context() { context = cl::Context( device); } 

void Gpu_aho_corasick::set_command_queue() { command_queue = cl::CommandQueue( context, device); } //3 param?

void Gpu_aho_corasick::init()
{
    assert( dictionary.words.size() != 0 && text.size() != 0 &&
            "Run set_text() and add_word() methods before calling init()");
    dictionary.number_of_entries.resize( dictionary.words.size());
    dictionary.entries.resize( dictionary.words.size());
    set_platform();
    set_device();
    set_context();
    build_kernel();
    run_kernel();
}

void Gpu_aho_corasick::process_resulted_buffer( std::string matched_patterns)
{
        for ( int i = 0; i < matched_patterns.size(); i+=dictionary.words.size())
    {
        assert( i % dictionary.words.size() == 0 && "Corrupted mathed_patterns buffer");
        int text_index = i / dictionary.words.size();
        for ( int pattern_number = 0; pattern_number < dictionary.words.size(); pattern_number++)
        {
            if ( matched_patterns[i + pattern_number] == '1')
            {
                dictionary.entries.at( pattern_number).push_back( text_index);
                dictionary.number_of_entries.at( pattern_number)++;
            }
        }
    }
}

void Gpu_aho_corasick::print_address_device_bits()
{
    size_t size;
    cl_uint address_bits;
    clGetDeviceInfo( device(), CL_DEVICE_ADDRESS_BITS, 0, NULL, &size);
    clGetDeviceInfo( device(), CL_DEVICE_ADDRESS_BITS, size, &address_bits, NULL);
    std::cout <<  "Device address space size: " << size << " bits: " << address_bits << std::endl;
}

//template cl::Buffer Gpu_aho_corasick::create_buffer<std::string>( std::string); //Ask Kostya

void Transition_table::init()
{
    int number_of_state = 0;
    vector_data = std::vector<std::vector<std::pair< int, int>>>( gpu_aho_corasick->size(), std::vector<std::pair< int, int>>( std::numeric_limits<char>::max() + 1, std::make_pair( 0, 0))); //split
    write_states( gpu_aho_corasick->get_root_ptr(), &number_of_state);
    vector_data.erase( vector_data.end() - gpu_aho_corasick->dictionary.words.size() + 1, vector_data.end());

    for ( auto& raw : vector_data)
    {
        for ( auto& [next_state, pattern_number] : raw)
        {
            contiguous_data.push_back( next_state);
            contiguous_data.push_back( pattern_number);
        }
    }
    assert( contiguous_data.size() == vector_data.size() * (std::numeric_limits<char>::max() + 1) * 2 && "Corrupted buffer");
}

void Transition_table::write_states( Trie_node* state, int* number_of_state)
{
    int current_state = *number_of_state;
    for ( auto& [symbol, child_state] : state->child_links)
    {
        if ( child_state->is_terminal == false)
        {
            vector_data.at( current_state).at( symbol) = std::make_pair( ++(*number_of_state), 0);
            write_states( child_state, number_of_state);
        } 
        else
        {
            int matched_pattern_id = child_state->dictionary_index + 1;
            if ( child_state->is_leaf() == false)
            {
                vector_data.at( current_state).at( symbol) = std::make_pair( ++(*number_of_state), matched_pattern_id);
                write_states( child_state, number_of_state);
            }
            else
            {
                vector_data.at( current_state).at( symbol) = std::make_pair( TERMINAL, matched_pattern_id);
            }
        }
    }
}

void Transition_table::print()
{
    int index = 0;    
    for ( auto& i : vector_data)
    {
        std::cout << index << ") ";
        for ( int j = 0; j < i.size(); j++)
        {
            int a = i[j].first;
            int b = i[j].second;
            if ( a != 0 || b != 0)
                std::cout << (char)j << "[" << a << "," << b << "] ";
        }
        std::cout << std::endl;
        index++;
    }
}