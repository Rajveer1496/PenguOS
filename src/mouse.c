#include <stdint.h>

#define MOUSE_COMMAND_STATUS_PORT 0x64
#define DATA_PORT 0x60

//Hardware helpers
extern uint8_t inb(uint16_t port); //to read data from port (in io.asm)
extern void outb(uint16_t port, uint8_t value); // to write data to a port
extern void pic_send_eoi(uint8_t irq);
extern void pic_unmask(uint32_t irqNo);

//serial debug functions
extern void serial_print(const char* str);

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
}


int packet_counter = 0;
uint8_t packet[3];
void mouse_handler(){
    serial_print("1\n");
    serial_print("start of mouse_handler()\n");

    // Check if data is actually available
    // wait_untill_controller_ready();
   uint8_t status = inb(MOUSE_COMMAND_STATUS_PORT);
   serial_print("2\n");
if (!(status & 0x1)) {  // No data available
    serial_print("3-no data\n");
    pic_send_eoi(12);
    return;
}
serial_print("4\n");
if (!(status & 0x20)) {  // Bit 5 = 0, data is from keyboard, not mouse!
    serial_print("5-keyboard data\n");
    pic_send_eoi(12);
    return;
}
serial_print("6\n");

    

    wait_untill_dataPort_ready();
    serial_print("7\n");
    packet[packet_counter] = inb(0x60);
    serial_print("8\n");
    pic_send_eoi(12); //We are done with interrupt (CPU dont accepts new interrupts untill one interrupt is not handled (untill iret is not called))
    if(packet_counter==2) goto process_packets;
    serial_print("9-Read Next packet\n");
    packet_counter++;
    return;
    
    process_packets:
    serial_print("10\n");
    packet_counter=0;
    serial_print("11\n");
    serial_print("Mouseeeeeeee!\n");
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