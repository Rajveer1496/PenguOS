#include <stdint.h>
#include "graphics.h"
#include "memory.h"
#include "drivers.h"
#include "debug.h"

#define D3_line(x1,y1,z1,x2,y2,z2) draw_line(((x1)/(z1))+160,((y1)/(z1))+100,((x2)/(z2))+160,((y2)/(z2))+100)

extern uint32_t timer;
extern uint16_t current_tps;

void plot(int x,int y,int z){
    write_pixel_BackBuffer(((x)/(z))+160,((y)/(z))+100, 0x0E);
    return;
}

void draw_line(int x1, int y1, int x2, int y2){
    //Mid point line algorithm
    uint8_t color = 0x0D;

    int del_x = x2-x1;
    if(del_x<0) del_x*=(-1);

    int del_y = y2-y1;
    if(del_y<0) del_y*=(-1);

    int step_x = (x2>x1) ? 1:(-1);
    int step_y = (y2>y1) ? 1:(-1);

    int major_delta = (del_x >= del_y) ? del_x:del_y;
    int minor_delta = (del_x < del_y) ? del_x:del_y;


    int initial_decision_para = 2*minor_delta - major_delta;
    // int del_decision_para = 2*(del_y - del_x);

    int x_next = x1;
    int y_next = y1;
    int new_decision_para = initial_decision_para;

    write_pixel_BackBuffer(x_next,y_next,color);

    if(del_x >= del_y){
        // X is driving axis

        while(1){
        if((x_next == x2)) break;
            if(new_decision_para >= 0){
                x_next += step_x;
                y_next += step_y;

                new_decision_para = new_decision_para + 2*(del_y - del_x);
                write_pixel_BackBuffer(x_next,y_next,color);
            }else{
                x_next += step_x;
                new_decision_para = new_decision_para + 2*del_y;
                write_pixel_BackBuffer(x_next,y_next,color);
            }
        if((x_next == x2)) break;
        }
    }else{
        // Y is driving axis

        while(1){
        if((y_next ==y2)) break;
            if(new_decision_para >= 0){
                x_next += step_x;
                y_next += step_y;

                new_decision_para = new_decision_para + 2*(del_x - del_y);
                write_pixel_BackBuffer(x_next,y_next,color);
            }else{
                y_next += step_y;
                new_decision_para = new_decision_para + 2*del_x;
                write_pixel_BackBuffer(x_next,y_next,color);
            }

        if((y_next ==y2)) break;
        }
    }
    
    return;
}


void render_test(){    

    vga_clear_backBuffer();
    // write_pixel_BackBuffer(raster(x,z), raster(y,z), 0x0E);

    uint32_t last_frame = 0;
    setTPS(120);
    uint16_t FPS=10; //Animation is drawing "FPS" frames every second
    int z = 1;
    int x = 150;
    int y = x;
    
    while(1){
        //temp
        if(timer >= last_frame + (current_tps/FPS)){ 
            vga_clear_backBuffer();

            if(z>20) z=1;

            plot(0,0,z); //1
            plot(0,y,z); //2
            plot(x,y,z); //3
            plot(x,0,z); //4

            plot(0+50,0+20,5+z); //1`
            plot(0+50,y+20,5+z); //2`
            plot(x+50,y+20,5+z); //3`
            plot(x+50,0+20,5+z); //4`

            D3_line(0,0,z,0,y,z); //1-2
            D3_line(0,y,z,x,y,z); // 2-3
            D3_line(x,y,z,x,0,z); // 3-4
            D3_line(0,0,z,x,0,z); //1-4

            D3_line(0+50,0+20,z+5,0+50,y+20,z+5); //1`-2`
            D3_line(0+50,y+20,z+5,x+50,y+20,z+5); // 2`-3`
            D3_line(x+50,y+20,z+5,x+50,0+20,z+5); // 3`-4`
            D3_line(0+50,0+20,z+5,x+50,0+20,z+5); //1`-4`

            D3_line(0,0,z,50,20,5+z); //1-1`
            D3_line(0,y,z,50,y+20,5+z); //2-2`
            D3_line(x,y,z,x+50,y+20,5+z);
            D3_line(x,0,z,x+50,0+20,5+z);

            // D3_line(0,0,z,x,y,z);


            z++;
            vga_flipBuffer();
            last_frame = timer;
        } 
    }

}