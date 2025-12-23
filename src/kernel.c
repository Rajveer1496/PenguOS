// kernel.c - Our first C kernel!

#include <stdint.h> 
#include <stddef.h>
#include "debug.h"
#include "files.h"
#include "drivers.h"

// VGA text mode buffer address
#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_GRAPHICS_MEMORY 0xA0000

// VGA color codes
#define COLOR_BLACK 0
#define COLOR_WHITE 15

// External functions from other files
extern void idt_init(void);
extern void pic_remap(void);
extern void print_header();
extern void printToscreen(const char *str,unsigned char color);
extern int cursor_x;
extern int cursor_y;

//Paging functions
extern void page_init();
extern void* alloc_page();
extern void free_page(void* addr);
extern void paging_init();

//Serial Debuging Functions
extern void serial_init();
extern void serial_print(const char* str);

//VGA Grahics Functions
#define WIDTH 320
#define HEIGHT 200
extern void set_mode_13h();
extern void write_pixel(int x, int y, uint8_t color);
//Draw
extern void vga_draw_init();
extern void vga_flipBuffer();
extern void write_pixel_BackBuffer(int x, int y, uint8_t color);
//Image Helper
extern uint8_t * image_canvas_new(uint32_t width, uint32_t height);
extern void image_paste_backBuffer(int place_x,int place_y,uint16_t *ImgPtr);
extern void image_load_const(void* sourcePtr, uint16_t *ImgPtr);

//Mouse
extern void mouse_init();

//Hardware
extern void pic_unmask(uint32_t irqNo);
extern uint8_t inb(uint16_t port); //to read data from port (in io.asm)
extern void outb(uint16_t port, uint8_t value); // to write data to a port


// Helper function to write a character to VGA at position (x, y)
void vga_putchar(int x, int y, char c, unsigned char color) {
    unsigned short *vga = (unsigned short *)VGA_MEMORY;
    unsigned short entry = (color << 8) | c;  // High byte = color, low byte = character
    vga[y * VGA_WIDTH + x] = entry;
}

// Helper function to print a string at position (x, y)
void vga_print(int x, int y, const char *str, unsigned char color) {
    int i = 0;
    while (str[i] != '\0') {
        vga_putchar(x + i, y, str[i], color);
        i++;
    }
}

// Helper function to clear the screen
void vga_clear() {
    unsigned short *vga = (unsigned short *)VGA_MEMORY;
    unsigned short blank = ((COLOR_BLACK << 4) | COLOR_WHITE) << 8 | ' ';
    
    for (int i = 0; i < VGA_WIDTH * 25; i++) {
        vga[i] = blank;
    }
}

// Enable interrupts
static inline void enable_interrupts() {
    __asm__ volatile("sti");
}

// Add this helper function 
void vga_print_hex(int x, int y, uint32_t value) { //for debug
    char hex_chars[] = "0123456789ABCDEF";
    for (int i = 7; i >= 0; i--) {
        vga_putchar(x + (7 - i), y, hex_chars[(value >> (i * 4)) & 0xF], 0x0F);
    }
}

// This is called by boot.asm!
void kernel_main(void) {
    // Clear screen
    vga_clear();

    //initialize serial port
    serial_init();
    serial_print("Serial Port is initialized!\n");

    // Initialize IDT
    idt_init();
    serial_print("IDT initialized!\n");

    // Remap PIC
    pic_remap();

    //Initialize PMM (Physical Memory Manager)
    page_init();
    serial_print("PMM initialized!\n");

    //Start Paging
    paging_init();
    serial_print("Paging Enabled!\n");

    // print_header(); // to print first shell header

    


    //Switching to graphics mode
    // set_mode_13h();
    serial_print("Graphics Mode Enabled!\n");

    //testing
    for(int i=0;i<WIDTH;i++){
        for(int j=0; j<HEIGHT;j++){
            write_pixel(i,j,(i+j)*i);
        }
    }
  
    //Testing back buffer
    vga_draw_init();
    serial_print("Backbuffer implimentation sucessfull!\n");

    serial_print("\n");

    //set timer to 60 TPS
    setTPS(60);

    //enable mouse
    mouse_init();
    
    // Enable interrupts!
    enable_interrupts();
    serial_print("Interrupts Enabled!\n");

    //Start the disk (MUST BE AFTER ENABLING INTERRUPTS)
    disk_init();


//The read write test
    uint16_t test_buffer[256];
    uint16_t test_buffer2[256];
// Fill with test data
for(int i = 0; i < 256; i++) {
    test_buffer[i] = i;
}

// Write to sector 100
write_sector(100, test_buffer);

// Read back
read_sector(100, test_buffer2);

// Verify
for(int i = 0; i < 256; i++) {
    if(test_buffer[i] != test_buffer2[i]) {
        serial_print("MISMATCH!\n");
    }
}

serial_print("NICEE");

    // Hang forever (interrupts will still work!)
    while (1) {
        __asm__ volatile("hlt");  // Halt until next interrupt
    }
}