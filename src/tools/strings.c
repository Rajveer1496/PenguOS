#include "maths.h"
#include "debug.h"

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

    int string_len = strlength(char_buffer);
    int number=0;

    if(char_buffer[0] == '-'){ //negative int

        char * temp_buffer = &char_buffer[1];
        string_len--;
        for(int i=0;i<string_len;i++){
            number += ((int)temp_buffer[i] - 48)*pow(10,(string_len-i-1));
        }
        return number*(-1);
    }else{
        for(int i=0;i<string_len;i++){
            number += ((int)char_buffer[i] - 48)*pow(10,(string_len-i-1));
        }
        return number;
    }
    return -1;
}