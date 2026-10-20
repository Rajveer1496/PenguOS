// Paint app - pick color, draw with mouse on canvas

#include <stddef.h>
#include "apps.h"
#include "graphics.h"
#include "debug.h"

#define mod(x) (((x)<0) ? ((-1)*(x)):(x)) // |x|

#define PAINT_WIDTH 560
#define PAINT_HEIGHT 400
#define TOOLBAR_HEIGHT 36
#define SWATCH_SIZE 24
#define NO_OF_COLORS 10

//Mouse
extern int mouse_x;
extern int mouse_y;
extern int left_hold;
extern struct window * dragging_window;

struct window * paint_window = NULL;

//color pallet
static const uint8_t colors[NO_OF_COLORS][3] = {
    {0,0,0},       //black
    {255,255,255}, //white (eraser)
    {220,40,40},   //red
    {250,140,30},  //orange
    {250,220,40},  //yellow
    {60,180,75},   //green
    {40,120,220},  //blue
    {140,70,200},  //purple
    {240,120,180}, //pink
    {120,80,40}    //brown
};

int selected_color = 0;
int brush_size = 4;
int last_paint_x = -1; //last point of stroke (-1 = no stroke)
int last_paint_y = -1;

//toolbar positions
#define BRUSH_BTN_X (8 + NO_OF_COLORS*(SWATCH_SIZE+4) + 16)
#define CLEAR_BTN_X (PAINT_WIDTH - 64)

void paint_draw_toolbar(struct window * w){
    window_draw_start(w);
    vbe_draw_rect_solid(0,0,w->width,TOOLBAR_HEIGHT,200,200,205);
    vbe_draw_rect_solid(0,TOOLBAR_HEIGHT-1,w->width,1,120,120,130);

    //color swatches
    for(int i=0;i<NO_OF_COLORS;i++){
        int x = 8 + i*(SWATCH_SIZE+4);
        vbe_draw_rect_solid(x,6,SWATCH_SIZE,SWATCH_SIZE,colors[i][0],colors[i][1],colors[i][2]);
        if(i == selected_color){
            vbe_draw_rect_hollow(x-2,4,SWATCH_SIZE+4,SWATCH_SIZE+4,0,0,0);
            vbe_draw_rect_hollow(x-3,3,SWATCH_SIZE+6,SWATCH_SIZE+6,255,255,255);
        }else{
            vbe_draw_rect_hollow(x,6,SWATCH_SIZE,SWATCH_SIZE,80,80,80);
        }
    }

    //brush sizes (2,4,8)
    for(int i=0;i<3;i++){
        int size = 2 << i;
        int x = BRUSH_BTN_X + i*(SWATCH_SIZE+4);
        vbe_draw_rect_solid(x,6,SWATCH_SIZE,SWATCH_SIZE,240,240,240);
        vbe_draw_rect_solid(x + (SWATCH_SIZE-size)/2,6 + (SWATCH_SIZE-size)/2,size,size,0,0,0);
        if(size == brush_size) vbe_draw_rect_hollow(x,6,SWATCH_SIZE,SWATCH_SIZE,40,120,220);
        else vbe_draw_rect_hollow(x,6,SWATCH_SIZE,SWATCH_SIZE,80,80,80);
    }

    //clear button
    vbe_draw_rect_solid(CLEAR_BTN_X,6,56,SWATCH_SIZE,240,240,240);
    vbe_draw_rect_hollow(CLEAR_BTN_X,6,56,SWATCH_SIZE,80,80,80);
    vbe_draw_string(CLEAR_BTN_X+8,10,"Clear",0,0,0);

    window_draw_end(w);
}

void paint_clear_canvas(struct window * w){
    window_draw_start(w);
    vbe_draw_rect_solid(0,TOOLBAR_HEIGHT,w->width,w->height-TOOLBAR_HEIGHT,255,255,255);
    window_draw_end(w);
}

void paint_draw(struct window * w){
    paint_clear_canvas(w);
    paint_draw_toolbar(w);
}

//one brush stamp at x,y (only inside canvas)
void paint_dot(int x,int y){
    int half = brush_size/2;
    for(int j=y-half;j<y-half+brush_size;j++){
        if(j < TOOLBAR_HEIGHT) continue; //dont paint on toolbar
        for(int i=x-half;i<x-half+brush_size;i++){
            vbe_write_pixel_BackBuffer(i,j,colors[selected_color][0],colors[selected_color][1],colors[selected_color][2]);
        }
    }
}

//Bresenham line with brush stamps (mouse moves many pixels between frames)
void paint_line(int x1,int y1,int x2,int y2){
    int del_x = mod(x2-x1);
    int del_y = mod(y2-y1);
    int step_x = (x2>x1) ? 1:(-1);
    int step_y = (y2>y1) ? 1:(-1);
    int error = del_x - del_y;

    while(1){
        paint_dot(x1,y1);
        if(x1 == x2 && y1 == y2) break;

        int e2 = 2*error;
        if(e2 > -del_y){
            error -= del_y;
            x1 += step_x;
        }
        if(e2 < del_x){
            error += del_x;
            y1 += step_y;
        }
    }
}

void paint_click(struct window * w,int x,int y){
    if(y >= TOOLBAR_HEIGHT) return; //canvas is handled in update (needs hold)

    //color swatches
    for(int i=0;i<NO_OF_COLORS;i++){
        int sx = 8 + i*(SWATCH_SIZE+4);
        if(x >= sx && x < sx+SWATCH_SIZE){
            selected_color = i;
            paint_draw_toolbar(w);
            return;
        }
    }

    //brush sizes
    for(int i=0;i<3;i++){
        int bx = BRUSH_BTN_X + i*(SWATCH_SIZE+4);
        if(x >= bx && x < bx+SWATCH_SIZE){
            brush_size = 2 << i;
            paint_draw_toolbar(w);
            return;
        }
    }

    //clear
    if(x >= CLEAR_BTN_X && x < CLEAR_BTN_X+56){
        paint_clear_canvas(w);
    }
}

void paint_update(struct window * w){
    //paint only when mouse is held on this window (and not dragging some window)
    if(!left_hold || dragging_window != NULL || window_at(mouse_x,mouse_y) != w){
        last_paint_x = -1;
        return;
    }

    int x = mouse_x - w->p->x_pos;
    int y = mouse_y - w->p->y_pos - TITLE_BAR_HEIGHT;

    if(y < TOOLBAR_HEIGHT){ //on toolbar or title bar
        last_paint_x = -1;
        return;
    }

    window_draw_start(w);
    if(last_paint_x == -1) paint_dot(x,y);
    else paint_line(last_paint_x,last_paint_y,x,y);
    window_draw_end(w);

    last_paint_x = x;
    last_paint_y = y;
}

void paint_close(struct window * w){
    (void)w;
    paint_window = NULL;
}

void open_paint(){
    if(paint_window != NULL){
        window_focus(paint_window);
        return;
    }

    paint_window = create_window("Paint",300,120,PAINT_WIDTH,PAINT_HEIGHT);
    if(paint_window == NULL) return;

    paint_window->on_draw = paint_draw;
    paint_window->on_click = paint_click;
    paint_window->on_update = paint_update;
    paint_window->on_close = paint_close;

    last_paint_x = -1;
    paint_draw(paint_window);
}
