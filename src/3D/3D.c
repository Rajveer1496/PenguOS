#include <stdint.h>
#include "graphics.h"
#include "memory.h"
#include "drivers.h"
#include "debug.h"

#define D3_line(x1,y1,z1,x2,y2,z2) draw_line(((x1)/(z1))+160,((y1)/(z1))+100,((x2)/(z2))+160,((y2)/(z2))+100)
#define mod(x) ((x)<0) ? ((-1)*(x)):(x) // |x|
#define p(h) plot(cube->x##h,cube->y##h,cube->z##h)
#define connect(h,k) D3_line(cube->x##h,cube->y##h,cube->z##h,cube->x##k,cube->y##k,cube->z##k)
#define pinit(h,k) cube->h = (k) //to initialise cube vertex
#define pupdate(h,k) cube->h += (k) //to update cuve vertex

extern uint32_t timer;
extern uint16_t current_tps;

struct cube{
    int x1,y1,z1;
    int x2,y2,z2;
    int x3,y3,z3;
    int x4,y4,z4;
    
    int x11,y11,z11;
    int x22,y22,z22;
    int x33,y33,z33;
    int x44,y44,z44;
};

void plot(int x,int y,int z){
    write_pixel_BackBuffer(((x)/(z))+160,((y)/(z))+100, 0x0E);
    return;
}

void draw_line(int x1, int y1, int x2, int y2){
    //Mid point line algorithm
    uint8_t color = 0x0D;

    int del_x = mod(x2-x1); // |x2 - x1| 

    int del_y = mod(y2-y1); // |y2 - y1|

    int step_x = (x2>x1) ? 1:(-1);
    int step_y = (y2>y1) ? 1:(-1);

    int major_delta = (del_x >= del_y) ? del_x:del_y;
    int minor_delta = (del_x < del_y) ? del_x:del_y;

    int initial_decision_para = 2*minor_delta - major_delta;

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

struct cube * create_cube(int X0,int Y0, int Z0, int length,int lengthz){
    struct cube * cube = (struct cube *)salloc(4,24);

    pinit(x1,X0);
    pinit(y1,Y0);
    pinit(z1,Z0);

    pinit(x2,X0);
    pinit(y2,Y0+length);
    pinit(z2,Z0);

    pinit(x3,X0+length);
    pinit(y3,Y0+length);
    pinit(z3,Z0);

    pinit(x4,X0+length);
    pinit(y4,Y0);
    pinit(z4,Z0);

    pinit(x11,X0);
    pinit(y11,Y0);
    pinit(z11,Z0+lengthz);

    pinit(x22,X0);
    pinit(y22,Y0+length);
    pinit(z22,Z0+lengthz);

    pinit(x33,X0+length);
    pinit(y33,Y0+length);
    pinit(z33,Z0+lengthz);

    pinit(x44,X0+length);
    pinit(y44,Y0);
    pinit(z44,Z0+lengthz);

    return cube;

}

void draw_cube(struct cube * cube){
    //Front Face
    p(1);
    p(2);
    p(3);
    p(4);
    
    //Back Face
    p(11);
    p(22);
    p(33);
    p(44);

    //Front face lines
    connect(1,2);
    connect(2,3);
    connect(3,4);
    connect(1,4);

    //Back face lines
    connect(11,22);
    connect(22,33);
    connect(33,44);
    connect(11,44);

    //Inbetween lines
    connect(1,11);
    connect(2,22);
    connect(3,33);
    connect(4,44);

    return;
}

void cube_move(int X0, int Y0, int Z0, struct cube * cube){
    pupdate(x1,X0);
    pupdate(y1,Y0);
    pupdate(z1,Z0);

    pupdate(x2,X0);
    pupdate(y2,Y0);
    pupdate(z2,Z0);

    pupdate(x3,X0);
    pupdate(y3,Y0);
    pupdate(z3,Z0);

    pupdate(x4,X0);
    pupdate(y4,Y0);
    pupdate(z4,Z0);

    pupdate(x11,X0);
    pupdate(y11,Y0);
    pupdate(z11,Z0);

    pupdate(x22,X0);
    pupdate(y22,Y0);
    pupdate(z22,Z0);

    pupdate(x33,X0);
    pupdate(y33,Y0);
    pupdate(z33,Z0);

    pupdate(x44,X0);
    pupdate(y44,Y0);
    pupdate(z44,Z0);

    return;
}

void cube_rotate(){
    //TODO
}



void render_test(){    

    vga_clear_backBuffer();

    struct cube * cube = create_cube(25,25,2,100,1);

    uint32_t last_frame = 0;
    setTPS(120);
    uint16_t FPS=10; //Animation is drawing "FPS" frames every second
    int x = 150;
    int y = x;
    
    while(1){
        //temp
        if(timer >= last_frame + (current_tps/FPS)){ 
            vga_clear_backBuffer();

            draw_cube(cube);

            cube_move(-5,-3,0,cube);

            vga_flipBuffer();
            last_frame = timer;
        } 
    }

}