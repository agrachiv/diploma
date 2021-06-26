#include "raw_aho.h"

int main()
{
    Aho_corasick test;

    test.add_word("abccba");
    test.add_word("aabba");
    test.add_word("bab");
    test.add_word("ab");
    test.add_word("acdab");
    test.add_word("cdab");
    test.add_word("bc");
    test.set_text("abccbaaabbacda");
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
    std::string test_string_1 = "abccbaaabbacda";

    std::cout << numbers << std::endl;
    std::cout << test_string_1 << std::endl;
    test.dictionary.print();
    /*

    std::string test_string_3 = "XXAAATCGAAAA";
    
    Aho_corasick test2;
    test2.add_word( "XAAA");
    test2.add_word( "XAAATCG");
    test2.init();
    test2.find_all_entries(test_string_3);
    std::cout << numbers << std::endl;
    std::cout << test_string_3 << std::endl;
    test2.dictionary.print();
 

    Aho_corasick test2;
    test2.add_word( "cab");
    //test2.add_word( "cabcab");
    test2.set_text( "cabcabcabcabcabcabcab");
    test2.init();
    test2.find_all_entries();
    test2.dictionary.print();
   */

    return 0;
}