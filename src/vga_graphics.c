#include <stdint.h>

#define WIDTH 320
#define HEIGHT 200
#define VGA_GRAPHICS_MEMORY 0xA0000

extern void outb(uint16_t port, uint8_t value); // to write data to a port
extern uint8_t inb(uint16_t port); //to read data from port 
extern void serial_print(const char* str);

void write_pixel(int x, int y, uint8_t color);

uint8_t *buffer = (uint8_t*)VGA_GRAPHICS_MEMORY;

// Mode 13h register values
unsigned char mode13h_regs[61] = { //theese are the values that should go into each register
    // MISC Output Register
    0x63,
    
    // Sequencer Registers (5)
    0x03, 0x01, 0x0F, 0x00, 0x0E,
    
    // CRTC Registers (25)
    0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
    0xFF,
    
    // Graphics Controller Registers (9)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
    0xFF,
    
    // Attribute Controller Registers (21)
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x41, 0x00, 0x0F, 0x00, 0x00
};

void set_mode_13h(){
    //MISC register
    outb(0x3C2, mode13h_regs[0]);

    // Sequencer Rigisters
    for(int i=0; i<5;i++){
        outb(0x3C4,i); //selecting the register
        outb(0x3C5,mode13h_regs[i+1]); //write value
    }

    //CRTC registers
    for(int i=0; i<25;i++){
        outb(0x3D4,i);
        outb(0x3D5, mode13h_regs[i+6]);
    }

    //Graphics registers
    for(int i=0; i<9;i++){
        outb(0x3CE,i);
        outb(0x3CF, mode13h_regs[i+31]);
    }

    //Attribute registers
    // Reset attribute controller flip-flop
    inb(0x3DA);  // Just reading resets it

    // write attribute registers
    for(int i = 0; i < 21; i++) {
        outb(0x3C0, i);                    // Write index
        outb(0x3C0, mode13h_regs[i + 40]); // Write data (same port!)
    }

    //Initializing the buffer - Black screen
    for(int i = 0; i < WIDTH; i++) {
        for(int j=0 ; j<HEIGHT; j++){
            write_pixel(i,j,0x0);
        }
    }

    // Enable video display
    outb(0x3C0, 0x20);
}

void write_pixel(int x, int y, uint8_t color){
    if(x<=WIDTH && y <=HEIGHT){
    int index = (WIDTH * y) + x;
    buffer[index] = color;
    }else serial_print("Error: Trying to print out of screen!\n");
}