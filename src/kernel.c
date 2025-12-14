// kernel.c - Our first C kernel!

#include <stdint.h>   //for debug

// VGA text mode buffer address
#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_GRAPHICS_MEMORY 0xA0000

// VGA color codes
#define COLOR_BLACK 0
#define COLOR_WHITE 15

//Timer
extern void setTPS(uint16_t TPS);
extern uint32_t timer;

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

    
    outb(0x21, 0xFF);  // Mask all IRQs on master (EXPERIMENT!)
    outb(0xA1, 0xFF);  // Mask all IRQs on slave
    // Remap PIC
    pic_remap();
    pic_unmask(1);   // Unmask Keyboard
    pic_unmask(2);  // Unmask the cascade line


    //set timer to 60 TPS
    setTPS(60);

    //enable mouse
    mouse_init();
    
    // Enable interrupts!
    enable_interrupts();
    serial_print("Interrupts Enabled!\n");

    

    // Read actual PIC masks from hardware
uint8_t actual_mask1 = inb(0x21);  // Master PIC
uint8_t actual_mask2 = inb(0xA1);  // Slave PIC

serial_print("Master PIC mask: ");
vga_print_hex(0, 22, actual_mask1);
serial_print("\nSlave PIC mask: ");
vga_print_hex(0, 23, actual_mask2);
serial_print("\n");

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

    //Debug
    // vga_print(0, 15, "timer(on inter)", 0x0E);
    // vga_print(2, 17, "last_second:", 0x0E);
    // vga_print(2, 18, "timer:", 0x0E);
    // vga_print(2, 19, "last_second + 60:", 0x0E);

    //Testing Timer
    // uint32_t last_second = 0;
    // while(1){
    // vga_print_hex(15, 17, last_second);
    // vga_print_hex(15, 18, timer);
    // vga_print_hex(20, 19, last_second + 60);
    
    // if(timer >= last_second + 60){
    //     printToscreen("A second Elapsed!",0x0E);
    //     serial_print("A second Elapsed!\n");
    //     last_second = timer;
    // } 
    // }

    serial_print("\n");
    

    // Hang forever (interrupts will still work!)
    while (1) {
        __asm__ volatile("hlt");  // Halt until next interrupt
    }
}