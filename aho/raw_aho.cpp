#include "raw_aho.h"

///////////////////////////////////////////////////////////////////////
///////////////////////////////debug///////////////////////////////////

void trie_node::print_word_backwards()
{
    if ( parrent != nullptr)
    {
        for ( const auto& [character, node] : parrent->child_links)
            if ( node == this)
                std::cout << character;
        parrent->print_word_backwards();
    }
}

char trie_node::get_character()
{
    if (this->parrent == nullptr)
    {
        return 'E';
    }
    else
    {
        for ( auto it : parrent->child_links)
            if ( it.second == this)
                return it.first;
    }
    assert( false && "something wetn wrong");
}

///////////////////////////////////////////////////////////////////////
/////////////////////////vocabluary_info///////////////////////////////

void vocabluary_info::print()
{
    std::cout << "Search words are:" << std::endl;
    for ( int i = 0; i < words.size(); i++)
    {
        std::cout << "Word '" << words[i] << "' found " << number_of_entries[i] << " times";
        if ( number_of_entries[i] != 0)
        {
            std::cout << " at positions: ";
            for ( auto& j : entries[i])
                std::cout << j - words[i].length() << " ";
        }
        std::cout << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////
//////////////////////////aho_corasick/////////////////////////////////


void aho_corasick::init() 
{
    vocabluary.number_of_entries.resize( vocabluary.words.size());
    vocabluary.entries.resize( vocabluary.words.size());
    calculate_suffix_links();
    calculate_terminal_links();
}

void aho_corasick::find_all_entries( std::string text) 
{
    int index = 0;
    for ( auto c = text.cbegin(); c != text.cend(); ++c, index++)
    {
        step( *c, index);
    }
}

void aho_corasick::step( char character, int index)
{
    while ( current_state != nullptr)
    {
        if ( current_state->is_terminal == true)
        {
            vocabluary.number_of_entries.at( current_state->vocabluary_index)++;
            vocabluary.entries.at( current_state->vocabluary_index).push_back( index);
        }
        for (
              trie_node* terminal_search_state = current_state->terminal_link;
              terminal_search_state != nullptr;
              terminal_search_state = terminal_search_state->terminal_link
            )
        {
            vocabluary.number_of_entries.at( terminal_search_state->vocabluary_index)++;
            vocabluary.entries.at( terminal_search_state->vocabluary_index).push_back( index);
        }
        auto candidate = current_state->child_links.find( character);
        std::cout << "Character " << character << " bor_state " << current_state->get_character() << std::endl;
        if ( candidate != current_state->child_links.end())
        {
            current_state = candidate->second;
            return;
        }
        else
            current_state = current_state->suffix_link;
    }
    current_state = &root;
}

void aho_corasick::add_word( std::string word)
{
    trie_node* current_node = &root;
    for ( auto character_it = word.cbegin(); character_it != word.cend(); ++character_it)
    {
        auto search = current_node->child_links.find( *character_it);
        bool is_terminal = ( std::next( character_it) == word.cend() ? true : false);
        if ( search == current_node->child_links.end())
        {
            trie_node* child_node = new trie_node( current_node, is_terminal);
            current_node->child_links.insert( {*character_it, child_node});
            current_node = child_node;
            if ( is_terminal == true)
            {
                current_node->vocabluary_index = vocabluary.words.size();
                vocabluary.words.push_back( word);
            }
        } 
        else 
        {
            current_node = search->second;
            if ( is_terminal == true && current_node->is_terminal == false)
            {
                current_node->is_terminal = true;
                current_node->vocabluary_index = vocabluary.words.size();
                vocabluary.words.push_back( word);
            }
        }
    }
}

void aho_corasick::calculate_suffix_links()
{
    //breadth-first search
    std::queue<trie_node*> search_queue;
    root.suffix_link = nullptr;
    for ( const auto& [character, child_node] : root.child_links)
    {
        child_node->suffix_link = &root;
        search_queue.push( child_node);
    }
    while ( search_queue.empty() == false)
    {
        trie_node* current_node = search_queue.front();
        search_queue.pop();
        for ( const auto& [character, child_node] : current_node->child_links)
        {
            trie_node* potentional_suffix_parrent = current_node->suffix_link;
            auto potentional_suffix_it = potentional_suffix_parrent->child_links.find( character);
            while ( potentional_suffix_it == potentional_suffix_parrent->child_links.end() && potentional_suffix_parrent != &root)
            {
                potentional_suffix_parrent = potentional_suffix_parrent->suffix_link;
                potentional_suffix_it = potentional_suffix_parrent->child_links.find( character);
            }
            if ( potentional_suffix_it != potentional_suffix_parrent->child_links.end())
                child_node->suffix_link = potentional_suffix_it->second;
            else
                child_node->suffix_link = &root;
            search_queue.push( child_node);
        }
    }
}

void aho_corasick::calculate_terminal_links() //do for breath-first search?
{
    //breadth-first search
    std::queue<trie_node*> search_queue;
    root.terminal_link = nullptr;

    for ( const auto& [character, child_node] : root.child_links)
        search_queue.push( child_node);
    while ( search_queue.empty() == false)
    {
        trie_node* current_node = search_queue.front();
        search_queue.pop();

        if ( current_node->suffix_link->is_terminal == true)
            current_node->terminal_link = current_node->suffix_link;
        else if ( current_node->suffix_link != &root)
            current_node->terminal_link = current_node->suffix_link->terminal_link;

        for ( const auto& [character, child_node] : current_node->child_links)
            search_queue.push( child_node);
    }
}

trie_node* aho_corasick::find_word( std::string word)
{
    trie_node* current_node = &root;
    for ( auto it = word.cbegin(); it != word.cend(); ++it)
    {
        auto search = current_node->child_links.find(*it);
        if ( search == current_node->child_links.end())
            return nullptr;
        else 
            current_node = search->second;
        if ( std::next( it) == word.cend())
            return current_node;
    }
}

///////////////////////////////////////////////////////////////////

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