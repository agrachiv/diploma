__kernel void Find_matches(
    __constant char* transition_table,
    __global char* matched_patterns,
    __constant char* text,
    int dictionary_size,
    int text_size)
{
    
    int id = get_global_id(0);
    int state_index = transition_table[0];

    for ( int i = id; i < text_size; i++)
    {
        char symbol = text[i];
        int next_state = transition_table[state_index + 2 * symbol];
        int matched_pattern = transition_table[state_index + 2 * symbol + 1];
        if ( next_state != 0) //partial match
        {
            if ( matched_pattern != 0) //full pattern match
                matched_patterns[ id * dictionary_size + matched_pattern - 1] = '1'; //mark as matched
            if ( next_state == -1)
                break;
            state_index = next_state * 128 * 2;
        }
        else
        {
            break;
        }
    }
}
