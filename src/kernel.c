// kernel.c - Our first C kernel!

#include <stdint.h> 
#include <stddef.h>
#include "debug.h"
#include "files.h"
#include "drivers.h"
#include "threading.h"
#include "graphics.h"
#include "memory.h"

void break_ppp(){ //debug
    return;
}

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

extern uint32_t timer;
extern uint16_t current_tps;

//Disable APIC
extern void disable_apic();

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
extern void image_paste_backBuffer(int place_x,int place_y,uint16_t *ImgPtr);
extern void image_load_const(void* sourcePtr, uint16_t *ImgPtr);
extern void animation();

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
 
void vga_print_hex(int x, int y, uint32_t value) { //for debug
    char hex_chars[] = "0123456789ABCDEF";
    for (int i = 7; i >= 0; i--) {
        vga_putchar(x + (7 - i), y, hex_chars[(value >> (i * 4)) & 0xF], 0x0F);
    }
}

void testProcess1(){
    uint32_t last_frame = 0;
    int height = 50;
    int width = 50;
    uint8_t color = 1;
    int x = 10;
    int y = 10;
    setTPS(120);
    uint16_t FPS=120; //Animation is drawing "FPS" frames every second
     while(1){

    //temp
    if(timer >= last_frame + (current_tps/FPS)){ 
        vga_clear_backBuffer();
        draw_Rectangle_solid(x,y,height,width,color);
        if(x<WIDTH && y < HEIGHT){
            x+=2;
            y+=2;
        }else{
            x=10;
            y=10;

            if(width < WIDTH && height<HEIGHT){
                width++;
                height++;

            }else{
                width = 40;
                height = 20;
                serial_print("Dimension reset\n");
            }
        }
        vga_flipBuffer();
        last_frame = timer;
    } 
    }
}

void testProcess2(){
    uint32_t last_frame = 0;
    int height = 50;
    int width = 50;
    uint8_t color = 2;
    int x = 10;
    int y = 10;
    setTPS(120);
    uint16_t FPS=120; //Animation is drawing "FPS" frames every second
     while(1){

    //temp
    if(timer >= last_frame + (current_tps/FPS)){ 
        vga_clear_backBuffer();
        draw_Rectangle_solid(x,y,height,width,color);
        if(x<WIDTH && y < HEIGHT){
            x++;
            y++;
        }else{
            x=10;
            y=10;

            if(width < WIDTH && height<HEIGHT){
                width++;
                height++;

            }else{
                width = 40;
                height = 20;
                serial_print("Dimension reset\n");
            }
        }
        vga_flipBuffer();
        last_frame = timer;
    } 
    }
}

//TESTING
extern void render_test();

// This is called by kernel_entry.asm!
void kernel_main(void) {


    // // Clear screen
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

    print_header(); // to print first shell header

    //Switching to graphics mode
    // set_mode_13h();
    serial_print("Graphics Mode Enabled!\n");

    // testing
    for(int i=0;i<WIDTH;i++){
        for(int j=0; j<HEIGHT;j++){
            write_pixel(i,j,(i+j)*i);
        }
    }
  
    //Testing back buffer
    vga_draw_init();
    serial_print("Backbuffer implimentation sucessfull!\n");

    //enable mouse
    mouse_init();
    serial_print("Right before interrupt!\n");

    break_ppp();

    // Enable interrupts!
    enable_interrupts();
    serial_print("Interrupts Enabled!\n");

    serial_print_number(36);
    serial_print_number(364555678);


    //Start the disk (MUST BE AFTER ENABLING INTERRUPTS)
    // disk_init();

    serial_print("NICEE\n");

    setTPS(120);

    // animation();

    // Threading testing

    // void *fn1 = &testProcess1;
    // void *fn2 = &testProcess2;

    // create_thread(fn1);
    // create_thread(fn2);


    //The read write test
    // uint16_t test_buffer[256];
    // uint16_t test_buffer2[256];
    // // Fill with test data
    // for(int i = 0; i < 256; i++) {
    //     test_buffer[i] = i;
    // }

    // // Write to sector 100
    // write_sector(100, test_buffer);

    // // Read back
    // read_sector(100, test_buffer2);

    // // Verify
    // for(int i = 0; i < 256; i++) {
    //     if(test_buffer[i] != test_buffer2[i]) {
    //         serial_print("MISMATCH!\n");
    //     }
    // }


    // //File system
    // initializeDriveBitmap(); //File system Initialization

    // // Check file system Bitmap initialisation
    // uint16_t test_buffer3[256];
    // for(int i=1;i<1024;i++){
    //     read_sector(i,test_buffer3);
    //     for(int j=0;j<256;j++){
    //         if(test_buffer3[j] != 0){
    //             serial_print("Bitmap Not initialized correctly!\n");
    //             serial_print("At sector: ");
    //             serial_print_number(i);
    //         }
    //     }
    // }

    // //Check Bitmap functionalities
    // set_sector_inUse(10000);
    // set_sector_inUse(10001);

    // int b = check_sector_usage(10000);

    // if(b==1){
    //         serial_print("sector in use!\n");
    //     }else if(b == 0){
    //         serial_print("Sector is free\n");
    // }


    // set_sector_free(10001);
    // int c = check_sector_usage(10001);

    // if(c==1){
    //         serial_print("sector in use!\n");
    //     }else if(c == 0){
    //         serial_print("Sector is free\n");
    // }


    init_salloc(); //Salloc init
    uint32_t * debug_int = salloc(4,5);
    

    uint32_t * debug_int_2 = salloc(4,5);
    // sfree(debug_int_2,4,5);

    sfree(debug_int,4,5);

    uint32_t * debug_int_3 = salloc(4,5);
    sfree(debug_int_3,4,5);

    for(int i=0;i<5000;i++){
        salloc(4,5);
    }

    set_mode_13h();
    render_test();

    serial_print("Kernel END\n");
    // Hang forever (interrupts will still work!)
    while (1) {
        __asm__ volatile("hlt");  // Halt until next interrupt
    }
}