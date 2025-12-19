#ifndef GRAPHICS_H
#define GRAPHICS_H

struct image {
    uint8_t* img;
    int screen_x;
    int screen_y;
};

//Image helper
uint8_t * image_canvas_new(uint16_t width, uint16_t height);
void image_paste_backBuffer(int place_x,int place_y,uint16_t *ImgPtr);
void image_load_const(void* sourcePtr, uint16_t *ImgPtr);
void update_mouse();
void image_update(struct image *ptr);
void image_load(uint16_t* sourcePtr, uint16_t *ImgPtr);
void image_delete(uint16_t * img);

//vga_draw
void vga_flipBuffer();
void vga_clear_backBuffer();
void draw_Rectangle_solid(int x,int y,int width,int height,uint8_t color);

//vga_graphics
void vga_flipBuffer();
void write_pixel_BackBuffer(int x, int y, uint8_t color);
void write_pixel(int x, int y, uint8_t color);

#endif