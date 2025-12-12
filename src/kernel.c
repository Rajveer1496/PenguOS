// kernel.c - Our first C kernel!

#include <stdint.h>   //for debug

// VGA text mode buffer address
#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80

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
    
    // Print our message
    // unsigned char color = (COLOR_BLACK << 4) | COLOR_WHITE;  // White text on black background
    // vga_print(0, 0, "PenguOS - Protected Mode Kernel", color);
    
    // Initialize IDT
    idt_init();
    // vga_print(0, 2, "Type something on the keyboard:", color);

    // Remap PIC
    pic_remap();
    
    // Enable interrupts!
    enable_interrupts();

    //Initialize PMM (Physical Memory Manager)
    page_init();

    //DEBUG: TESTING PAGING
    vga_clear();
    vga_print(0, 1, "Before Paging!", 0x0f);

    paging_init();
    vga_print(0, 3, "After Paging", 0x0f);

    // Test: Allocate a page and use it through virtual address
    void* test_page = alloc_page();
    vga_print(0, 5, "Allocated page at:", 0x0F);
    vga_print_hex(25, 5, (uint32_t)test_page);

    // Write through virtual address (identity mapped)
    int* ptr = (int*)test_page;
    *ptr = 0xDEADBEEF;

    // Read it back
    vga_print(0, 6, "Read back value:", 0x0F);
    vga_print_hex(25, 6, *ptr);

    if (*ptr == 0xDEADBEEF) {
        vga_print(0, 7, "SUCCESS! Paging works!", 0x0A);
    }
    
    
    // print_header(); // to print first shell header
    // vga_print(0, 1, "IDT initialized, interrupts enabled!", color);
    
    // Hang forever (interrupts will still work!)
    while (1) {
        __asm__ volatile("hlt");  // Halt until next interrupt
    }
}