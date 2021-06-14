#include <unordered_map>
#include <string>
#include <iostream>
#include <queue>
#include <cassert>
#include <utility>

struct trie_node
{
    trie_node( trie_node* parrent = nullptr, bool is_terminal = false) :
        parrent( parrent),
        is_terminal( is_terminal)
    {}
    std::unordered_map<char, trie_node*> child_links;
    //std::unordered_map<const char, trie_node*> child_links; why that doesn't work?
    trie_node* parrent;
    trie_node* suffix_link = nullptr;
    trie_node* terminal_link = nullptr;
    bool is_terminal;
    int vocabluary_index = -1;

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
        void init();
        void find_all_entries( std::string text);
    //private:
        trie_node root;
        trie_node* current_state = &root;
        vocabluary_info vocabluary; 

        void calculate_suffix_links();
        void calculate_terminal_links();

        void step( char character, int index);
};

///////////////////////////////////////////////////////////////////////////////////