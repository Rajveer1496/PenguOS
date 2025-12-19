#include <stdint.h>
#include <stddef.h>
#include "graphics.h"
#include "debug.h"

#define WIDTH 320
#define HEIGHT 200

#define MOUSE_COMMAND_STATUS_PORT 0x64
#define DATA_PORT 0x60

//Hardware helpers
extern uint8_t inb(uint16_t port); //to read data from port (in io.asm)
extern void outb(uint16_t port, uint8_t value); // to write data to a port
extern void pic_send_eoi(uint8_t irq);
extern void pic_unmask(uint32_t irqNo);

struct image roxo; //TEMP

void wait_untill_controller_ready();
void wait_untill_dataPort_ready();

void mouse_init(){

    //First Enable mouse in PS/2 controller
    // Read configuration byte
    outb(0x64, 0x20);  // Command: Read configuration
    wait_untill_dataPort_ready();
    uint8_t config = inb(0x60);

    // Enable mouse interrupt (set bit 1)
    config |= 0x02;

    // Write configuration back
    outb(0x64, 0x60);  // Command: Write configuration
    wait_untill_controller_ready();
    outb(0x60, config);

    pic_unmask(1);   // Unmask Keyboard
    pic_unmask(2);  // Unmask the cascade line

    //Then unmask the IRQ 12 (Mouse)
    pic_unmask(12); //But IRQ 12 is unmasked by default by BIOS/GRUB

    //Now Enable mouse for CPU
    outb(MOUSE_COMMAND_STATUS_PORT,0xA8); //Enable Mouse
    wait_untill_controller_ready();

    outb(MOUSE_COMMAND_STATUS_PORT,0xD4); //next byte is for mouse
    wait_untill_controller_ready();

    set_default:
    outb(DATA_PORT,0xF6); //set defaults
    wait_untill_dataPort_ready();
    if(inb(DATA_PORT) != 0xFA) goto set_default;

    wait_untill_controller_ready();
    outb(MOUSE_COMMAND_STATUS_PORT,0xD4); //Tell chip that next byte is for mouse
    wait_untill_controller_ready();
    
    start_packet:
    outb(DATA_PORT,0xF4); //tell mouse to start sending packets
    wait_untill_dataPort_ready();
    if(inb(DATA_PORT) != 0xFA) goto start_packet;

    serial_print("Mouse is ON!\n");

    //TEMPP
    roxo.img = image_canvas_new(50,50);
    for(int i=4;i<2500;i++){
        roxo.img[i] = 1;
    }

    roxo.screen_x = 0;
    roxo.screen_y = 0;
}

//KEY STATUS VARIABLES
int left_hold = 0;
int right_hold = 0;
int middle_hold = 0;

//Mouse cursor variable
int mouse_x=100;
int mouse_y=0;

int packet_counter = 0;
uint8_t packet[3];
void mouse_handler(){
    // Check if data is actually available
    uint8_t status = inb(MOUSE_COMMAND_STATUS_PORT);
    if (!(status & 0x1)) {  // No data available
        pic_send_eoi(12);
        return;
    }

    if (!(status & 0x20)) {  // Bit 5 = 0, data is from keyboard, not mouse!
        pic_send_eoi(12);
        return;
    }

    wait_untill_dataPort_ready();
    packet[packet_counter] = inb(0x60);
    pic_send_eoi(12); //We are done with interrupt (CPU dont accepts new interrupts untill one interrupt is not handled (untill iret is not called))
    if(packet_counter==2) goto process_packets;
    packet_counter++;
    return;
    
//---PROCESS---
    process_packets:
    packet_counter=0;

    //---KEY STATUS---
    //Left key
    if(left_hold == 0){
    if(packet[0] & 0x1){ //pressed
        left_hold = 1;
        serial_print("Left key pressed!\n");
    }}
    
    if(left_hold == 1){
    if(!(packet[0] & 0x1)){ //released
        left_hold = 0;
        serial_print("Left key Released!\n");
    }}

    //Right key
    if(right_hold == 0){
    if(packet[0] & (0x1<<1)){ //pressed
        right_hold = 1;
        serial_print("Right key pressed!\n");
    }}
    
    if(right_hold == 1){
    if(!(packet[0] & (0x1<<1))){ //released
        right_hold = 0;
        serial_print("Right key Released!\n");
    }}

    //middle key
    if(middle_hold == 0){
    if(packet[0] & (0x1<<2)){ //pressed
        middle_hold = 1;
        serial_print("Middle key pressed!\n");
    }}
    
    if(middle_hold == 1){
    if(!(packet[0] & (0x1<<2))){ //released
        middle_hold = 0;
        serial_print("Middle key Released!\n");
    }}

    //---MOUSE MOVEMENT---

    //Moved left
    if(packet[0] & (0x1<<4)){
        if( mouse_x>0 &&(packet[1] !=0)){ //
            // serial_print("Moved Left!\n");
            mouse_x += (packet[1] - 256);
        }
    }

    //Moved Right
    if(!(packet[0] & (0x1<<4))){
        if(mouse_x<WIDTH && (packet[1] !=0)){  //
            // serial_print("Moved Right!\n");
            mouse_x += (packet[1]);
        }
    }

    int inv_sensitivity = 100; //y is moving too much, so to slow it down
    //Moved Down
    if(packet[0] & (0x1<<5)){
        if(mouse_y<HEIGHT && (packet[2] !=0)){  //
            // serial_print("Moved Down!\n");
            mouse_y += (packet[2])/inv_sensitivity;
        }
    }

    //Moved UP
    if(!(packet[0] & (0x1<<5))){
        if(mouse_y >0 && (packet[2] !=0)){ //
            // serial_print("Moved UP!\n");
            mouse_y += (packet[2] - 256)/inv_sensitivity;
        }
    }

    //---KEY ACTIONS---

    vga_clear_backBuffer();

    image_update(&roxo);
    
    //Update cursor
    update_mouse();

    //rest of code to process packets
}



//---HELPER FUNCTIONS---
void wait_untill_controller_ready(){
    while(1){ //wait while data buffer becomes empty (controller gets ready)
        if(!(inb(MOUSE_COMMAND_STATUS_PORT) & 0x2)) break; //1st bit = 0 tells data port is empty and ready to take the data
    }
}

void wait_untill_dataPort_ready(){
    while(1){ 
        if(inb(MOUSE_COMMAND_STATUS_PORT) & 0x1) break; //0th bit = 1 tells that data is ready to be read in Data port
    }
}


/*
CHECK LIST


*/