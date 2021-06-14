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

int main()
{
    aho_corasick test;

    test.add_word("abccba");
    test.add_word("aabba");
    test.add_word("bab");
    test.add_word("ab");
    test.add_word("acdab");
    test.add_word("cdab");
    test.add_word("bc");

    test.init();
    
    assert( test.root.suffix_link == nullptr && "broken suffix link");

    assert( test.find_word("a")     ->suffix_link == &(test.root)           && "broken suffix link");
    assert( test.find_word("ab")    ->suffix_link == test.find_word("b")    && "broken suffix link");
    assert( test.find_word("abc")   ->suffix_link == test.find_word("bc")   && "broken suffix link");
    assert( test.find_word("abcc")  ->suffix_link == test.find_word("c")    && "broken suffix link");
    assert( test.find_word("abccb") ->suffix_link == test.find_word("b")    && "broken suffix link");
    assert( test.find_word("abccba")->suffix_link == test.find_word("ba")   && "broken suffix link");
    assert( test.find_word("bab")   ->suffix_link == test.find_word("ab")   && "broken suffix link");

    assert( test.find_word("a")     ->terminal_link == nullptr                && "broken terminal link");
    assert( test.find_word("ab")    ->terminal_link == nullptr                && "broken terminal link");
    assert( test.find_word("abc")   ->terminal_link == test.find_word("bc")   && "broken terminal link");
    assert( test.find_word("abcc")  ->terminal_link == nullptr                && "broken terminal link");
    assert( test.find_word("abccb") ->terminal_link == nullptr                && "broken terminal link");
    assert( test.find_word("abccba")->terminal_link == nullptr                && "broken terminal link");
    assert( test.find_word("bab")   ->terminal_link == test.find_word("ab")   && "broken terminal link");

    assert( test.find_word("a")     ->is_terminal == false  && "broken terminal flag");
    assert( test.find_word("ab")    ->is_terminal == true   && "broken terminal flag");
    assert( test.find_word("abc")   ->is_terminal == false  && "broken terminal flag");
    assert( test.find_word("abcc")  ->is_terminal == false  && "broken terminal flag");
    assert( test.find_word("abccb") ->is_terminal == false  && "broken terminal flag");
    assert( test.find_word("abccba")->is_terminal == true   && "broken terminal flag");

    std::string numbers       = "01234567890123";
    //std::string test_string_1 = "abababababababc";
    std::string test_string_2 = "abccbaaabbacda";

    /*
    test.find_all_entries(test_string_1);
    std::cout << numbers << std::endl;
    std::cout << test_string_1 << std::endl;
    test.vocabluary.print();
    */
    
    test.find_all_entries(test_string_2);
    std::cout << numbers << std::endl;
    std::cout << test_string_2 << std::endl;
    test.vocabluary.print();

    return 0;
}