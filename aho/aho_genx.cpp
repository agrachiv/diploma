#include <cm/cm.h>

_GENX_MAIN_ void Find_matches(
    const SurfaceIndex transition_table [[type("buffer_t char")]],
    const SurfaceIndex text_buffer [[type("buffer_t char")]],
    SurfaceIndex matched_patterns_buffer [[type("buffer_t char")]],
    const int dictionary_size,
    const int text_size
    )
{   
    unsigned tid = cm_group_id(0) * cm_local_size(0) + cm_local_id(0);
    
    constexpr int MAX_TEXT_SIZE = 1024;
    constexpr int VECTOR_SIZE = 128;
    constexpr int EXT_VECTOR_SIZE = 128 + 16; //hack to get small offset
    int allinged_tid = tid / VECTOR_SIZE;
    int remainder = tid % VECTOR_SIZE; 

    //Read text buffer to vector
    vector<char, EXT_VECTOR_SIZE> ext_text;
    read( text_buffer, allinged_tid * 128, ext_text.select<128,1>(0));
    read( text_buffer, allinged_tid * 128 + 128, ext_text.select<16,1>(128));

    vector<char, 128> text = ext_text.select<128,1>(remainder);

    //Prepare store buffer vector
    //vector<char, MAX_TEXT_SIZE> = matched_patterns_info;
    int next_state = 0;
    
    //#pragma unroll
    for ( int i = 0; i < text_size; i++)
    {
        char symbol = text[i];
        
        //Get raw (can't read more than 128 at once)
        vector<char, 256> full_raw;
        read( transition_table, next_state * 256, full_raw.select<128, 1>(0));  
        read( transition_table, next_state * 256 + 128, full_raw.select<128, 1>(128));

        //Get states vector
        vector<char, 128> next_states = full_raw.select<128, 2>(0);

        //Get pattern numbers vector
        vector<char, 128> matched_patterns = full_raw.select<128, 2>(1);

        //Get next state (raw) number
        next_state = next_states[symbol];
        //Get matched pattern number (might be 0)
        int matched_pattern = matched_patterns[symbol];

        //Partial match
        if ( next_state != 0)
        {
            //Full pattern match
            if ( matched_pattern != 0)
                //Mark as matched
                write( matched_patterns_buffer, tid * dictionary_size, matched_pattern - 1, '1');
            //Longest pattern
            if ( next_state == -1)
                break;
        }
        else
        {
            break;
        }
    }
}