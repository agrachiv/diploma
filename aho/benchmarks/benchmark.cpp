#include "benchmark/benchmark.h"
#include "ocl_aho.h"
#include "cm_aho.h"

enum Test_result 
{ 
    TEST_PASSED,
    TEST_FAILED
};

enum Platforms
{
    CPU,
    OCL,
    CM,
    ALL_PLATFORMS
};

template<typename aho_type>
void setup_test( aho_type test_class, std::string data_file_name)
{
    std::string data_file_path = "../test_data/" + data_file_name;
    std::ifstream file_stream( data_file_path);
    assert( file_stream.is_open() && "Can't open file");

    std::string text_string;
    std::getline( file_stream, text_string);
    test_class.set_text( text_string);

    std::string dictionary_size_str;
    std::getline( file_stream, dictionary_size_str);
    int dictionary_size = std::stoi( dictionary_size_str);

    for ( int i = 0; i < dictionary_size; i++)
    {
        std::string word;
        std::getline( file_stream, word);
        test_class.add_word( word);
    }
    test_class.init();
}

template<typename aho_type>
int execute_test( aho_type test_class, std::string data_file_name)
{
    std::string data_file_path = "../test_data/" + data_file_name + "_ans";
    std::ifstream file_stream( data_file_path);
    assert( file_stream.is_open() && "Can't open file");

    std::string dictionary_size_str;
    std::getline( file_stream, dictionary_size_str);
    int dictionary_size = std::stoi( dictionary_size_str);
    
    std::cout << std::endl << "----------------------------------------------------------" << std::endl;
    std::cout << "Running test " << data_file_name << std::endl << std::endl;
    std::cout << "Searching in text: " << std::endl << "Index:  ";
    for ( int i = 0; i < test_class.text.size(); i++)
        std::cout << i%10; 
    std::cout << std::endl << "Text:   " << test_class.text << std::endl << std::endl;
    test_class.dictionary.print();
    
    assert( dictionary_size == test_class.dictionary.words.size() &&
            "Number of answers provided in answers file doesn't match to data file");
    
    for ( int pattern_number = 0; pattern_number < dictionary_size; pattern_number++)
    {
        std::string answer;
        std::getline( file_stream, answer);

        std::istringstream is( answer);
        std::vector<int> expected_entries( ( std::istream_iterator<int>( is)), ( std::istream_iterator<int>()));
    
        auto current_entry = test_class.dictionary.entries[pattern_number];
        
        if ( expected_entries.size() != current_entry.size())
        {
            std::cout << "Pattern \"" << test_class.dictionary.words.at( pattern_number) << "\" found " << current_entry.size();
            std::cout << " times, instead of " << expected_entries.size() << std::endl;
            std::cout << "Test " << data_file_name << " failed!" << std::endl;
            std::cout << "----------------------------------------------------------" << std::endl << std::endl;
            return TEST_FAILED;
        }
        for ( int i = 0; i < expected_entries.size(); i++)
        {
            if ( current_entry[i] != expected_entries[i])
            {
                std::cout << "Pattern " << pattern_number << " found at index " << current_entry[i] << ", ";
                std::cout << "instead of " << expected_entries[i] << std::endl;
                std::cout << "Test " << data_file_name << " failed!" << std::endl;
                std::cout << "----------------------------------------------------------" << std::endl << std::endl;
                return TEST_FAILED;
            }
        }
    }
    std::cout << "Test " << data_file_name << " passed!" << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl << std::endl;
    
    return TEST_PASSED;
}

int run_unit_test( int platform, std::string data_file_prefix, bool perfomance_only = false)
{
    std::cout << data_file_prefix << std::endl;
    int test_results = TEST_PASSED;
    if ( platform == CPU || platform == ALL_PLATFORMS)
    {
        Aho_corasick cpu_test;
        std::cout << "Chosen platform CPU" << std::endl;
        setup_test<Aho_corasick&>( cpu_test, data_file_prefix);
        if ( perfomance_only == false)
            test_results += execute_test<Aho_corasick&>( cpu_test, data_file_prefix);
    }
    if ( platform == OCL || platform == ALL_PLATFORMS)
    {
        std::cout << "Chosen platform OpenCl" << std::endl;
        Ocl_aho_corasick ocl_test;
        setup_test<Ocl_aho_corasick&>( ocl_test, data_file_prefix);
        if ( perfomance_only == false)
            test_results += execute_test<Ocl_aho_corasick&>( ocl_test, data_file_prefix);
    }
    if ( platform == CM || platform == ALL_PLATFORMS)
    {
        std::cout << "Chosen platform CM" << std::endl;
        Cm_aho_corasick cm_test;
        setup_test<Cm_aho_corasick&>( cm_test, data_file_prefix);
         if ( perfomance_only == false)
            test_results += execute_test<Cm_aho_corasick&>( cm_test, data_file_prefix);
    }
    return test_results;
}

int main()
{
    int test_results = TEST_PASSED;
    test_results += run_unit_test( ALL_PLATFORMS, "simple_test_1");
    test_results += run_unit_test( ALL_PLATFORMS, "test_long_string");
    test_results += run_unit_test( OCL, "simple_test_2");
    test_results += run_unit_test( CM, "simple_test_2");

    /*
    run_unit_test( ALL_PLATFORMS, "genome_1000", true);
    run_unit_test( ALL_PLATFORMS, "genome_500", true);
    run_unit_test( ALL_PLATFORMS, "genome_100", true);
    run_unit_test( ALL_PLATFORMS, "genome_50", true);
    */

    std::cout << "==========================================================" << std::endl;
    if ( test_results == TEST_PASSED)
        std::cout << "ALL TESTS PASSED" << std::endl;
    else
        std::cout << test_results << " TESTS FAILED" << std::endl;
    std::cout << "==========================================================" << std::endl;
    
    //benchmark::RegisterBenchmark( "Basic OpenCL unit test", basic_benchmark_gpu);
    //benchmark::RunSpecifiedBenchmarks();
}