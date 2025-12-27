#include<stdint.h>
#include "debug.h"

#define DATA_PORT 0x3F8
#define CONTROL 0x3F9
#define LINE_CONTROL 0x3FB
#define FIFO_CONTROL 0x3FA
#define MODEM_CONTROL 0x3FC
#define LINE_STATUS 0x3FD

extern void outb(uint16_t port, uint8_t value); // to write data to a port
extern uint8_t inb(uint16_t port); //to read data from port 

void serial_init(){
    //Disable Interrupts
    outb(CONTROL, 0x0);

    //Enable DLAB
    outb(LINE_CONTROL,0x80);

    //set low byte of divisor
    outb(DATA_PORT,0x03);

    //set high byte of divisor
    outb(CONTROL, 0x0);

    //Disable DLAB amd set protocol (0x03 = 8 bits, no parity, 1 stop bit)
    outb(LINE_CONTROL,0x03);

    //Enable FIFO
    outb(FIFO_CONTROL,0xC7);

    //Enable Modem
    outb(MODEM_CONTROL,0x0B);

}

void serial_write_char(char c){
    while(1){
        if(inb(LINE_STATUS) & 0x20){ //check if transmit buffer is empty or not (bit 5)
            outb(DATA_PORT,c);
            return;
        }
    }
}

void serial_print(const char* str){
    int i = 0;
    while(str[i] != '\0'){
        if(str[i] == '\n'){ // serial expects \r\n for new line not just \n
            serial_write_char('\r');
            serial_write_char('\n');
        }else serial_write_char(str[i]);
        i++;
    }
}

void serial_print_number(uint32_t number){ //supports at max 9 digits due to int limits (the int i)
    char buffer[100];
    int j=0;
    int c;
    uint32_t i=10;
    int b = (number % i);
    buffer[j] = (char)(b+48);
    do{
        j++;
        i = i*10;
        c = ((number % i) - b)/(i/10);
        b = (number % i);
        buffer[j] = (char)(c+48);
    }while((number % i)!= number);
 
    for(;j>=0;j--){
        serial_write_char(buffer[j]);
    }
    serial_write_char('\r');
    serial_write_char('\n');
}