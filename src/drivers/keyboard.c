// keyboard.c - Simple keyboard driver

#include <stdint.h>
#include "debug.h"

#define cmd_buffer_size 80

// External functions
extern uint8_t inb(uint16_t port); //to read data from port (in io.asm)
extern void outb(uint16_t port, uint8_t value); // to write data to a port
extern void pic_send_eoi(uint8_t irq);
extern void shellExecute(const char *str);
void print_header(); //to print header
void addCtoBuffer(char c, char *str); //to add a char to a string buffer
void RemoveCbuffer(char *str); // to remove a char from string
void ClearStrBuffer(char *str); // to clear entire string buffer

//Animation cursor
extern int anim_cursor_x;
extern int anim_cursor_y;

#define KEYBOARD_DATA_PORT 0x60

// Simple US keyboard scancode to ASCII table (only printable keys)
// This is a simplified version - real keyboards are more complex!
static const char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,  ' '
};

static const char scancode_to_ascii_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '`',
    0,    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*',  0,  ' '
};

int cursor_x = 0;
int cursor_y = 0;  // Start a few lines down

static int shift_status = 0;
static int caps_status = 0;
static int ctrl_status = 0;

//command buffer
char cmdBuffer[cmd_buffer_size] = {0}; // max size 80 for now, initialized to all zeros

// Keyboard interrupt handler
void keyboard_handler(void) {
    // Read scancode from keyboard
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // vga_print_hex(10,22, scancode);//DEBUG
    
    //SHIFT Handling
        if((scancode == 0x2A) || (scancode == 0x36)){ //press (Left and right both shifts)
            shift_status = 1;
        }else if((scancode == 0xAA) || (scancode == 0xB6)){ //release
            shift_status = 0;
        }

    //CAPS Handling
        if(scancode == 0xBA){ //only toogle upon caps release
            if(caps_status == 0){
               caps_status = 1;
           }else caps_status = 0;
         }

    //Ctrl Handling
        if(scancode == 0x1D){ //press (Left and right both shifts)
                ctrl_status = 1;
        }else if(scancode == 0x9D){ //release
            ctrl_status = 0;
        }

    
    if (!(scancode & 0x80)) { // Only handle key press events (bit 7 = 0 means key pressed)
        
        //Arrow Keys handling
        if(scancode == 0x48) {//up arrow
            // cursor_y--;
            anim_cursor_y--;
        } 
        if(scancode == 0x50){ //down arrow
            // cursor_y++;
            anim_cursor_y++;
        } 
        if(scancode == 0x4B){//left arrow
            if(cursor_x > 10)cursor_x--;
            anim_cursor_x--;
        } 
        if(scancode == 0x4D) { //right arrow
            cursor_x++;
            anim_cursor_x++;
        }

        // Convert scancode to ASCII
        if (scancode < sizeof(scancode_to_ascii)) {
            char c;

            //SHIFT and CAPS handling
            if(
               ((scancode<=13) |  //for  1 to +
                ((scancode == 26) | // {
                 (scancode==27) | // }
                 (scancode==43) | // "\"
                 ((scancode>=39) & (scancode<=41)) |  
                 ((scancode>=51) & (scancode<=53))))){ // < to /
                if(shift_status == 1) c = scancode_to_ascii_shift[scancode];
                else c = scancode_to_ascii[scancode];
            }
            else if((shift_status == 1) & (caps_status==1)){
                c = scancode_to_ascii[scancode];
            }
            else if((shift_status == 1) | (caps_status==1)){
                c = scancode_to_ascii_shift[scancode];
            }
            else{
                c = scancode_to_ascii[scancode];
            }
            
            if((ctrl_status == 1) & (scancode == 38) ){ // Ctrl + L , to clear screen
                cursor_x = 0;
                cursor_y = 5; 
                for(int y =0 ; y<=25; y++){
                    vga_print(0, y, "                                                                                ", 0x0F);
                }
            }

            else if (c != 0) {
                // Handle special characters
                if (c == '\n') { //Enter key
                    if(cursor_y < 24){
                    // cursor_x = 0;
                    shellExecute(cmdBuffer); //execute the command
                    cursor_y++;
                    print_header();
                    }
                } else if (c == '\b') { //Backspace
                    if(cursor_x > 10) {
                        cursor_x--;
                        vga_putchar(cursor_x, cursor_y, ' ', 0x0F);
                        RemoveCbuffer(cmdBuffer);
                    }
                } else {
                    // Print character
                    vga_putchar(cursor_x, cursor_y, c, 0x0F);
                    addCtoBuffer(c,cmdBuffer);
                    cursor_x++;
                    // Wrap to next line
                    if (cursor_x >= 80) {
                        cursor_y++;
                        print_header();
                    }
                }
                
            }
        }
    }

    // vga_print(5, 15, cmdBuffer, 0x0F); //DEBUG print the buffer

    //Update Cursor
        uint16_t position = (cursor_y * 80) + cursor_x;
        uint8_t low_byte = (position & 0xFF);
        uint8_t high_byte = ((position >> 8) & 0xFF);

        outb(0x3D4, 14); // 14 = command to set high_byte
        outb(0x3D5, high_byte); //pass the high byte

        outb(0x3D4, 15); // 15 = command to set low_byte
        outb(0x3D5, low_byte); //pass the low byte

    
    
    // Send EOI to PIC
    // vga_print(0, 2, "Type something on the keyboard:", color);
    pic_send_eoi(1);  // IRQ 1 is keyboard
}

///---HELPER FUNCTIONS---

    //SHELL HEADER WRITE
    void print_header(){
        vga_print(0, cursor_y, "PenguOS>> ", 0x0F);
        cursor_x = 10; //length of string

        ClearStrBuffer(cmdBuffer); //clear command buffer, time to write new command
    }

    //Add char to buffer
    void addCtoBuffer(char c, char *str){
        int i=0;
        while(str[i] != '\0'){
            i++;
        }
        if(i >= 79) return; // safety check BUFFER IS FULL
        str[i] = c;
        str[i+1] = '\0'; //add null terminator at end
    }

    //Remove charachter from buffer
    void RemoveCbuffer(char *str){
        int i=0;
        while(str[i] != '\0') i++;
        str[i-1] = '\0';
    }

    //Clear whole command Buffer
    void ClearStrBuffer(char *str){
        str[0] = '\0';
    }




/*
left SHIFT Press = 2A
left SHIFT Release = AA

right SHIFT press = 36
right SHIFT Relaes = B6

CAPSLOCK press = 3A
CAPSLOCK Release = BA

*/ 


/* THE CHECKLIST

*/