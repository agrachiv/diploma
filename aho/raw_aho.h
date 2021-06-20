#include <unordered_map>
#include <string>
#include <iostream>
#include <queue>
#include <cassert>
#include <utility>
#include <fstream>
#include <sstream>
#include <iterator>

struct Trie_node
{
    Trie_node( Trie_node* parrent = nullptr, bool is_terminal = false);
    std::unordered_map<char, Trie_node*> child_links;
    //std::unordered_map<const char, Trie_node*> child_links; why that doesn't work?
    Trie_node* parrent;
    Trie_node* suffix_link = nullptr;
    Trie_node* terminal_link = nullptr;
    bool is_terminal;
    int dictionary_index = -1;
    int depth = 0;

    bool is_leaf() { return child_links.size() == 0 ? true : false; }
    ////////////////////////////////////debug////////////////////////////////////////////
    void print_word_backwards();
    char get_character();
    /////////////////////////////////////////////////////////////////////////////////////
};

struct Dictionary_info
{
    std::vector< std::string> words;
    std::vector< int> number_of_entries;
    std::vector< std::vector< int>> entries;

    void print();
};

class Aho_corasick
{
    public:
        Aho_corasick() {}
        Aho_corasick( std::string text) : text( text) {}
        void add_word( std::string word);
        Trie_node* find_word( std::string word); //temporary
        void find_all_entries( std::string text);
        int size() { return number_of_nodes; }
        Trie_node* get_root_ptr() { return &root; };
        virtual void init();
    //private:
        Trie_node root;
        Trie_node* current_state = &root;
        Dictionary_info dictionary; 
        int number_of_nodes = 1;
        std::string text;

        void calculate_suffix_links();
        void calculate_terminal_links();

        void step( char character, int index);
};