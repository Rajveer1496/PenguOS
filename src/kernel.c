// kernel.c - Our first C kernel!

// VGA text mode buffer address
#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80

// VGA color codes
#define COLOR_BLACK 0
#define COLOR_WHITE 15

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

// This is called by boot.asm!
void kernel_main(void) {
    // Clear screen
    vga_clear();
    
    // Print our message
    unsigned char color = (COLOR_BLACK << 4) | COLOR_WHITE;  // White text on black background
    vga_print(0, 0, "Hello from C kernel!", color);
    vga_print(0, 1, "We are in protected mode!", color);
    vga_print(0, 2, "Next: Set up IDT and keyboard!", color);
    
    // Hang forever
    while (1) {
        // Do nothing
    }
}