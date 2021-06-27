#include "raw_aho.h"

///////////////////////////////////////////////////////////////////////
///////////////////////////////debug///////////////////////////////////

Trie_node::Trie_node( Trie_node* parrent, bool is_terminal) : parrent( parrent), is_terminal( is_terminal)
{
    if ( parrent != nullptr)
        depth = parrent->depth + 1;
}


void Trie_node::print_word_backwards()
{
    if ( parrent != nullptr)
    {
        for ( const auto& [character, node] : parrent->child_links)
            if ( node == this)
                std::cout << character;
        parrent->print_word_backwards();
    }
}

char Trie_node::get_character()
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
    assert( false && "something went wrong");
}

///////////////////////////////////////////////////////////////////////
/////////////////////////Dictionary_info///////////////////////////////

void Dictionary_info::print()
{
    std::cout << "Search words are:" << std::endl;
    for ( int i = 0; i < words.size(); i++)
    {
        std::cout << i << ") Word '" << words[i] << "' found " << number_of_entries[i] << " times";
        if ( number_of_entries[i] != 0)
        {
            std::cout << " at positions: ";
            for ( auto& j : entries[i])
                std::cout << j << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

///////////////////////////////////////////////////////////////////////
//////////////////////////Aho_corasick/////////////////////////////////

void Aho_corasick::init() 
{
    assert( dictionary.words.size() != 0 && text.size() != 0 &&
            "Run set_text() and add_word() methods before calling init()");
    dictionary.number_of_entries.resize( dictionary.words.size());
    dictionary.entries.resize( dictionary.words.size());
    calculate_suffix_links();
    calculate_terminal_links();
    find_all_entries();
    fix_indexes();
}

void Aho_corasick::find_all_entries() 
{
    int index = 0;
    for ( auto c = text.cbegin(); c != text.cend(); ++c, index++)
    {
        step( *c, index);
    }
}

void Aho_corasick::step( char character, int index)
{
    while ( current_state != nullptr)
    {
        if ( current_state->is_terminal == true)
        {
            dictionary.number_of_entries.at( current_state->dictionary_index)++;
            dictionary.entries.at( current_state->dictionary_index).push_back( index);
        }
        for (
              Trie_node* terminal_search_state = current_state->terminal_link;
              terminal_search_state != nullptr;
              terminal_search_state = terminal_search_state->terminal_link
            )
        {
            dictionary.number_of_entries.at( terminal_search_state->dictionary_index)++;
            dictionary.entries.at( terminal_search_state->dictionary_index).push_back( index);
        }
        auto candidate = current_state->child_links.find( character);
        if ( candidate != current_state->child_links.end())
        {
            current_state = candidate->second;
            return;
        }
        else
            current_state = current_state->suffix_link;
    }
    current_state = get_root_ptr();
}

void Aho_corasick::add_word( std::string word)
{
    assert( word.size() <= 256 && "Patterns longer than 256 are currently not supported");
    Trie_node* current_node = get_root_ptr();
    for ( auto character_it = word.cbegin(); character_it != word.cend(); ++character_it)
    {
        auto search = current_node->child_links.find( *character_it);
        bool is_terminal = ( std::next( character_it) == word.cend() ? true : false);
        if ( search == current_node->child_links.end())
        {
            Trie_node* child_node = new Trie_node( current_node, is_terminal);
            number_of_nodes++;
            current_node->child_links.insert( {*character_it, child_node});
            current_node = child_node;
            if ( is_terminal == true)
            {
                current_node->dictionary_index = dictionary.words.size();
                dictionary.words.push_back( word);
            }
        } 
        else 
        {
            current_node = search->second;
            if ( is_terminal == true && current_node->is_terminal == false)
            {
                current_node->is_terminal = true;
                current_node->dictionary_index = dictionary.words.size();
                dictionary.words.push_back( word);
            }
        }
    }
}

void Aho_corasick::calculate_suffix_links()
{
    //breadth-first search
    std::queue<Trie_node*> search_queue;
    root.suffix_link = nullptr;
    for ( const auto& [character, child_node] : root.child_links)
    {
        child_node->suffix_link = get_root_ptr();
        search_queue.push( child_node);
    }
    while ( search_queue.empty() == false)
    {
        Trie_node* current_node = search_queue.front();
        search_queue.pop();
        for ( const auto& [character, child_node] : current_node->child_links)
        {
            Trie_node* potentional_suffix_parrent = current_node->suffix_link;
            auto potentional_suffix_it = potentional_suffix_parrent->child_links.find( character);
            while ( potentional_suffix_it == potentional_suffix_parrent->child_links.end() && potentional_suffix_parrent != get_root_ptr())
            {
                potentional_suffix_parrent = potentional_suffix_parrent->suffix_link;
                potentional_suffix_it = potentional_suffix_parrent->child_links.find( character);
            }
            if ( potentional_suffix_it != potentional_suffix_parrent->child_links.end())
                child_node->suffix_link = potentional_suffix_it->second;
            else
                child_node->suffix_link = get_root_ptr();
            search_queue.push( child_node);
        }
    }
}

void Aho_corasick::calculate_terminal_links() //do for breath-first search?
{
    //breadth-first search
    std::queue<Trie_node*> search_queue;
    root.terminal_link = nullptr;

    for ( const auto& [character, child_node] : root.child_links)
        search_queue.push( child_node);
    while ( search_queue.empty() == false)
    {
        Trie_node* current_node = search_queue.front();
        search_queue.pop();

        if ( current_node->suffix_link->is_terminal == true)
            current_node->terminal_link = current_node->suffix_link;
        else if ( current_node->suffix_link != get_root_ptr())
            current_node->terminal_link = current_node->suffix_link->terminal_link;

        for ( const auto& [character, child_node] : current_node->child_links)
            search_queue.push( child_node);
    }
}

void Aho_corasick::fix_indexes()
{
    for ( int i = 0; i < dictionary.words.size(); i++)
        if ( dictionary.number_of_entries[i] != 0)
            for ( auto& j : dictionary.entries[i])
                j -= dictionary.words[i].length();
}

Trie_node* Aho_corasick::find_word( std::string word)
{
    Trie_node* current_node = get_root_ptr();
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
    return nullptr; //leak???
}