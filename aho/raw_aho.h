#include <unordered_map>
#include <string>
#include <iostream>
#include <queue>
#include <cassert>
#include <utility>
#include <fstream>
#include <sstream>
#include <iterator>

struct trie_node
{
    trie_node( trie_node* parrent, bool is_terminal);
    std::unordered_map<char, trie_node*> child_links;
    //std::unordered_map<const char, trie_node*> child_links; why that doesn't work?
    trie_node* parrent;
    trie_node* suffix_link = nullptr;
    trie_node* terminal_link = nullptr;
    bool is_terminal;
    int vocabluary_index = -1;
    int depth = 0;

    ////////////////////////////////////debug////////////////////////////////////////////
    void print_word_backwards();
    char get_character();
    /////////////////////////////////////////////////////////////////////////////////////
};

struct vocabluary_info
{
    std::vector< std::string> words;
    std::vector< int> number_of_entries;
    std::vector< std::vector< int>> entries;

    void print();
};

class aho_corasick
{
    public:
        void add_word( std::string word);
        trie_node* find_word( std::string word); //temporary
        void find_all_entries( std::string text);
        int size() { return number_of_nodes; }
        virtual void init();
    //private:
        trie_node root;
        trie_node* current_state = &root;
        vocabluary_info vocabluary; 
        int number_of_nodes = 1;

        void calculate_suffix_links();
        void calculate_terminal_links();

        void step( char character, int index);
};