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

    if(buffer[j] == '0') j--; //clear preceding 0
 
    for(;j>=0;j--){
        serial_write_char(buffer[j]);
    }
}

int auto_debug_counter = 1;

void serial_auto_debug(){
    serial_print("[DEBUG]: At ");
    serial_print_number(auto_debug_counter);
    auto_debug_counter++;
    return;
}

/*
| 1 bit  |  8 bits  |      23 bits        |
|  Sign  | Exponent |      Mantissa       |
*/

void serial_print_float(float number,int precision){

    uint32_t all_bits = *(uint32_t *)&number; //get the bits from that memory address where float is

    int Exponent_127 = (all_bits >> 23) & 0xFF; //exponent with base 127
    uint8_t sign = (all_bits >> 31) & 0x1;
    uint32_t Mentissa = all_bits & 0x007FFFFF;

    //SPECIAL CASES
    switch(Exponent_127){
        case 0:
            if(Mentissa == 0)
                serial_print_number(0);
            else{
                //TODO
                serial_print("[Print Float]: Denormalized number, very samll number close to zero\n");
            }
            return;

        case 255:
            if(Mentissa == 0)
                serial_print((sign == 0)?"+infinity\n":"-infinity\n");
            else
                serial_print("NaN\n");
            return;

        default:
            break;
    }

    serial_print((sign == 0)?" ":"-"); //print the sign

    if(sign!=0) number *= -1;

    int int_num = (int)number;
    serial_print_number(int_num);
    serial_print(".");
    float float_num = (float)int_num;
    number -= float_num;

    for(int i=0;i<precision;i++){
        number *= 10.0;
        int_num = (int)number;
        serial_print_number(int_num);
        float_num = (float)int_num;
        number -= float_num;
    }
}