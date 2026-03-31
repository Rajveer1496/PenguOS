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

float stringToFloat(char * char_buffer){
    char * after_dot;
    char before_dot[20];
    for(int i=0; char_buffer[i] != '\0' && i<100; i++){ //seperating float into two parts
        if(char_buffer[i] == '.'){
            after_dot = &char_buffer[i+1];
            for(int j=0;j<i;j++){
                before_dot[j] = char_buffer[j];
            }
            before_dot[i] = '\0';
        }
    }
    int len_after_dot = strlength(after_dot);

    float number_before_dot = (float)stringToInt(before_dot);

    float number_after_dot = ((float)stringToInt(after_dot))*pow_f(0.1,len_after_dot);

    if(char_buffer[0] == '-') number_after_dot*=(-1);
    
    return number_before_dot + number_after_dot;
}