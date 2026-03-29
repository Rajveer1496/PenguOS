
int strlength(char * char_buffer){
    int safe_counter = 1000; //string limit to 1000
    int length=0;
    for(int i=0;(i<safe_counter && char_buffer[i] != '\0');i++){
        length++;
    }
    return length;
}

float stringToFloat(char * char_buffer){
    //TODO
    int string_len = strlength(char_buffer);
    
    return 0.00;
}

int stringToInt(char * char_buffer){
    //TODO
    int string_len = strlength(char_buffer);

    return 0;
}