#include <stdint.h>

extern void vga_print(int x, int y, const char *str, unsigned char color); //for debug
extern void vga_clear();

int strCompare(const char *str1,const char *str2);
void printToscreen(const char *str,unsigned char color);

extern int cursor_x;
extern int cursor_y;

void shellExecute(const char *str){
    if(strCompare(str,"help")){
        printToscreen("Commands: help, reboot, clear, about",0x0E);
    }else if(strCompare(str,"clear")){
        vga_clear();
        cursor_x = -1; //so print_header() can print from 0,0
        cursor_y = -1;
    }else if(strCompare(str,"about")){
        printToscreen("PenguOS - Made by Rj",0x0E);
    }
}

///--COMMAND FUNCTIONS---


///---HELPER FUNCTIONS---
    int strCompare(const char *str1,const char *str2){
        int i=0;
        while((str1[i] != '\0') && (str2[i] !='\0')){ //stop when either of string ends
            if(str1[i] != str2[i]){
                return 0;
            }
            i++;
            //Safety check!! (to avoid infinite loop)
            if(i>4096) return 3;
        }
        if((str1[i] == '\0') && str2[i] == '\0') return 1;
        else return 0;
    }

    void printToscreen(const char *str,unsigned char color){
        cursor_x = 0;
        cursor_y++;
        vga_print(cursor_x, cursor_y, str, color);
    }