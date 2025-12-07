// keyboard.c - Simple keyboard driver

#include <stdint.h>

// External functions
extern uint8_t inb(uint16_t port);
extern void pic_send_eoi(uint8_t irq);
extern void vga_putchar(int x, int y, char c, unsigned char color);

#define KEYBOARD_DATA_PORT 0x60

// Simple US keyboard scancode to ASCII table (only printable keys)
// This is a simplified version - real keyboards are more complex!
static const char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,  ' '
};

static int cursor_x = 0;
static int cursor_y = 5;  // Start a few lines down

// Keyboard interrupt handler
void keyboard_handler(void) {
    // Read scancode from keyboard
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Only handle key press events (bit 7 = 0 means key pressed)
    if (!(scancode & 0x80)) {
        // Convert scancode to ASCII
        if (scancode < sizeof(scancode_to_ascii)) {
            char c = scancode_to_ascii[scancode];
            
            if (c != 0) {
                // Handle special characters
                if (c == '\n') {
                    cursor_x = 0;
                    cursor_y++;
                } else if (c == '\b') {
                    if (cursor_x > 0) {
                        cursor_x--;
                        vga_putchar(cursor_x, cursor_y, ' ', 0x0F);
                    }
                } else {
                    // Print character
                    vga_putchar(cursor_x, cursor_y, c, 0x0F);
                    cursor_x++;
                    
                    // Wrap to next line
                    if (cursor_x >= 80) {
                        cursor_x = 0;
                        cursor_y++;
                    }
                }
                
                // Simple scroll (wrap to top)
                if (cursor_y >= 25) {
                    cursor_y = 5;
                    cursor_x = 0;
                }
            }
        }
    }
    
    // Send EOI to PIC
    pic_send_eoi(1);  // IRQ 1 is keyboard
}