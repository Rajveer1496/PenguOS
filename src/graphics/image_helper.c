#include <stdint.h>
#include "graphics.h"
#include "memory.h"
#include "debug.h"

/*
----IMAGE FORMAT---
Each cell = 8 bit (1 byte)
First 4 cells : store width and height (or two 16 bits cells holding width and height)
Then cells to store color value (0-255)

[width] [Height] [(0,0)][(1,0)][(2,0)]......
_______ ________ _______
16 bits 16 bits  8 bits

Max Image size (13h mode)
Width: 320
Height: 200

Total = 64000 bits ~ 63 KB ~ 16 pages 
*/


//Mouse
extern int mouse_x;
extern int mouse_y;
extern int left_hold;

int image_hold =0;


uint8_t * image_canvas_new(uint16_t width, uint16_t height){
    int required_pages = ((width * height)/4096) + 1;
   
    vga_print_hex(15, 17, required_pages); //Debug
    
    //Intiialize to blank
    uint8_t *ptr = (uint8_t *)alloc_continous_pages(required_pages);

    memInit_fast(ptr,required_pages*4096);

    //Store Width and Height
    uint16_t* ptr16 = (uint16_t *)ptr;

    ptr16[0] = width;
    ptr16[1] = height;

    return ptr;
}

void image_delete(uint16_t * img){
    uint32_t count = (((img[0] * img[1])/4096) + 1);
    free_continous_pages(img,count);
}

//Index = (width * y + x) + 4;

void image_load(uint16_t* sourcePtr, uint16_t *ImgPtr){

    if((sourcePtr[0] > ImgPtr[0]) || (sourcePtr[1] > ImgPtr[1])){
        serial_print("ERROR: image_load : Source dimension > Image dimension");
    }

    int width = sourcePtr[0];
    int height = sourcePtr[1];

    uint8_t *ptr8 = (uint8_t *)ImgPtr;
    uint8_t *srcPtr = (uint8_t *)sourcePtr;
    for(int x=0; x<width-1;x++){
        for(int y=0; y<height-1;y++){
            int index = (width*y + x) + 4;
            ptr8[index] = srcPtr[(width*y + x) + 4];
        }
    }
}

void image_load_const(void* sourcePtr, uint16_t *ImgPtr){ //To print color value arrays
    int width = ImgPtr[0];
    int height = ImgPtr[1];

    uint8_t *ptr8 = (uint8_t *)ImgPtr;
    uint8_t *srcPtr = (uint8_t *)sourcePtr;
    for(int x=0; x<width-1;x++){
        for(int y=0; y<height-1;y++){
            int index = (width*y + x) + 4;
            ptr8[index] = srcPtr[(width*y + x)]; //source is just color data (no dimension data)
        }
    }
}


//place_x, place_y -> to put from where on screen
void image_paste_backBuffer(int place_x,int place_y,uint16_t *ImgPtr){
    int width = ImgPtr[0];
    int height = ImgPtr[1];
    
    uint8_t *ptr8 = (uint8_t *)ImgPtr;
    for(int x=0; x<width-1;x++){
        for(int y=0; y<height-1;y++){
            int index = (width*y + x) + 4;
            if(ptr8[index] != 255) write_pixel_BackBuffer(x+place_x, y+place_y, ptr8[index]); //To create transperent effect
        }
    }

    vga_flipBuffer();
}


///---CURSOR UPDATE---

//the pointer
static const uint8_t mouse_ptr[] = {
56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
56,56,56,56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,56,56,56,56,56,56,56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,56,56,56,20,20,56,56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,56,56,20,38,38,20,56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,56,56,20,38,38,38,20,56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,56,56,20,20,38,38,38,56,56,56,56,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,56,56,20,38,38,38,38,56,56,56,56,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,56,56,20,38,38,38,38,20,56,56,56,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,20,38,38,38,56,56,56,56,56,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,56,20,20,20,56,56,56,56,56,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,56,56,56,56,56,56,56,56,56,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,56,56,56,56,56,56,56,56,56,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,56,56,56,56,56,56,56,56,56,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,56,56,56,56,56,56,56,56,56,255
};

void update_mouse(){
    int width = 30;
    int height = 15;

    for(int x=0; x<width-1;x++){
        for(int y=0; y<height-1;y++){
            int index = (width*y + x);
            if(mouse_ptr[index] != 255) write_pixel_BackBuffer(x+mouse_x, y+mouse_y, mouse_ptr[index]); //To create transperent effect
        }
    }

    vga_flipBuffer();
}


//---Image Update--- (drag with mouse)
//ptr is pointer to struct, not struct it self
void image_update(struct image *ptr){
    if(left_hold && (mouse_x>(*ptr).screen_x) && (mouse_x<(*ptr).screen_x + 50) && (mouse_y>(*ptr).screen_y) && (mouse_y<(*ptr).screen_y+20)){
        image_hold = 1;
    }else if(!left_hold){
        image_hold = 0;
    }

    if(image_hold){
        image_paste_backBuffer(mouse_x,mouse_y,(uint16_t *)(*ptr).img);
        (*ptr).screen_x = mouse_x;
        (*ptr).screen_y = mouse_y;
    }

    image_paste_backBuffer((*ptr).screen_x,(*ptr).screen_y,(uint16_t *)(*ptr).img);
}