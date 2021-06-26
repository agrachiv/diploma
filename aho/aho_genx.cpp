extern "C" _GENX_MAIN_ void
find_matches(
    __constant char* transition_table,
    __global char* matched_patterns,
    __constant char* text,
    int dictionary_size,
    int text_size)
{

    /*
    unsigned tid = cm_group_id(0) * cm_local_size(0) + cm_local_id(0);

    vector<unsigned, 32> in0;
    vector<unsigned, 32> in1;
    read(ibuf0, tid * 32 * sizeof(unsigned), in0);
    read(ibuf1, tid * 32 * sizeof(unsigned), in1);
    vector<unsigned, 32> in2 = in0 + in1;
    write(obuf,  tid * 32 * sizeof(unsigned), in2);
    */
}

/*
    int state_index = transition_table[0];

    for ( int i = id; i < text_size; i++)
    {
        char symbol = text[i];
        int next_state = transition_table[state_index + 2 * symbol]; //next_state индекс начала новой строки
        int matched_pattern = transition_table[state_index + 2 * symbol + 1]; //в решеточку 
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
*/