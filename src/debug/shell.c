#include <stdint.h>
#include "debug.h"

extern void vga_print(int x, int y, const char *str, unsigned char color); //for debug
extern void vga_clear();
extern void outb(uint16_t port, uint8_t value); // to write data to a port

int strCompare(const char *str1,const char *str2);
void printToscreen(const char *str,unsigned char color);
void JustPrint(); //for debug

extern void set_mode_13h();

extern int cursor_x;
extern int cursor_y;

//GUI terminal
extern int gui_running;
extern void terminal_print(const char * str);
extern void terminal_clear();
extern void rtc_get_time(int *hour,int *minute,int *second);

void shellExecute(const char *str){
    if(strCompare(str,"help")){
        printToscreen("Commands: help, reboot, clear, about, time",0x0E);
    }else if(strCompare(str,"clear")){
        if(gui_running){
            terminal_clear();
            return;
        }
        vga_clear();
        cursor_x = -1; //so print_header() can print from 0,0
        cursor_y = -1;
    }else if(strCompare(str,"about")){
        printToscreen("PenguOS - Made by Rj",0x0E);
    }else if(strCompare(str,"reboot")){
        printToscreen("Rebooting!",0x0E);
        outb(0x64,0xFE);
    }else if(strCompare(str,"")){
        //do nothing if just pressed enter
    }else if(strCompare(str,"devdi")){
        printToscreen(";)",0x0E);
    }
    else if(strCompare(str,"time")){
        int h,m,sec;
        rtc_get_time(&h,&m,&sec);
        char time_str[9] = "00:00:00";
        time_str[0] = '0' + h/10;   time_str[1] = '0' + h%10;
        time_str[3] = '0' + m/10;   time_str[4] = '0' + m%10;
        time_str[6] = '0' + sec/10; time_str[7] = '0' + sec%10;
        printToscreen(time_str,0x0E);
    }
    else if(strCompare(str,"gui")){
        if(gui_running){
            printToscreen("Already in GUI ;)",0x0E);
            return;
        }
        printToscreen("Starting 13h mode!",0x0E);
        set_mode_13h();
    }
    else printToscreen("Buddy what are you trying to say? use 'help' for available commands",0x0E);
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
        if(gui_running){ //print in GUI terminal
            terminal_print(str);
            return;
        }
        if(cursor_y>=24){
            cursor_x = 0;
            cursor_y = 0;
            vga_print(cursor_x, cursor_y, str, color);
            return;
        }
        cursor_x = 0;
        cursor_y++;
        vga_print(cursor_x, cursor_y, str, color);
    }

    void JustPrint(){
        cursor_x = 0;
        cursor_y++;
        vga_print(cursor_x, cursor_y, "JUST PRINT", 0x0B);
    }