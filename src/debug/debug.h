#ifndef DEBUG_H
#define DEBUG_H

//serial.c
void serial_print(const char* str);
extern void serial_print_number(uint32_t number);

//kernel.c
void vga_print_hex(int x, int y, uint32_t value);
void vga_putchar(int x, int y, char c, unsigned char color);
void vga_print_hex(int x, int y, uint32_t value); 
void vga_print(int x, int y, const char *str, unsigned char color);

//shell.c
void printToscreen(const char *str,unsigned char color);


#endif