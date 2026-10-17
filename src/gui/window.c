// Windows on top of planes
// window = plane + title bar + close button + app callbacks

#include <stddef.h>
#include "gui.h"
#include "graphics.h"
#include "debug.h"

extern uint16_t VBE_WIDTH;
extern uint16_t VBE_HEIGHT;
extern uint8_t VBE_BYTES_PER_PIXEL;

struct window windows[MAX_WINDOWS];
struct window * focused_window = NULL;
int top_z = 1; //Z_POS of top most window (background is 0)

void copy_title(char * dest,const char * src){
    int i=0;
    for(;src[i] != '\0' && i<MAX_TITLE-1;i++){
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

struct window * create_window(const char * title,int x,int y,int width,int height){
    struct window * w = NULL;
    for(int i=0;i<MAX_WINDOWS;i++){ //find free slot
        if(!windows[i].in_use){
            w = &windows[i];
            break;
        }
    }

    if(w == NULL){
        serial_print("[GUI]: MAX windows reached!\n");
        return NULL;
    }

    //plane includes title bar
    w->p = create_plane(x,y,width,height + TITLE_BAR_HEIGHT);
    if(w->p == NULL) return NULL;

    copy_title(w->title,title);
    w->width = width;
    w->height = height;
    w->in_use = 1;

    //no app attached yet
    w->on_draw = NULL;
    w->on_click = NULL;
    w->on_key = NULL;
    w->on_update = NULL;
    w->on_close = NULL;
    w->data = NULL;

    window_focus(w);
    window_clear(w,230,230,230);

    return w;
}

void close_window(struct window * w){
    if(w->on_close != NULL) w->on_close(w);

    delete_plane(w->p);
    w->in_use = 0;

    if(focused_window == w){ //give focus to next top most window
        focused_window = NULL;
        struct window * next = NULL;
        for(int i=0;i<MAX_WINDOWS;i++){
            if(windows[i].in_use && (next == NULL || windows[i].p->Z_POS > next->p->Z_POS)){
                next = &windows[i];
            }
        }
        if(next != NULL) window_focus(next);
    }
}

void draw_window_frame(struct window * w){
    vbe_set_draw_target(w->p->buffer,w->p->x_size,w->p->y_size);

    //title bar (blue when focused, grey when not)
    if(w == focused_window) vbe_draw_rect_solid(0,0,w->width,TITLE_BAR_HEIGHT,50,90,160);
    else vbe_draw_rect_solid(0,0,w->width,TITLE_BAR_HEIGHT,110,110,120);

    vbe_draw_string(8,4,w->title,255,255,255);

    //close button
    int close_x = w->width - CLOSE_BTN_SIZE - 3;
    vbe_draw_rect_solid(close_x,3,CLOSE_BTN_SIZE,CLOSE_BTN_SIZE,200,60,60);
    vbe_draw_char(close_x+5,2,'x',255,255,255);

    //border
    vbe_draw_rect_hollow(0,0,w->p->x_size,w->p->y_size,20,20,30);

    vbe_reset_draw_target();
}

void window_focus(struct window * w){
    struct window * old = focused_window;
    focused_window = w;

    top_z++;
    w->p->Z_POS = top_z; //bring on top

    if(old != NULL && old != w && old->in_use) draw_window_frame(old); //grey title
    draw_window_frame(w);
}

//to draw inside window (0,0 is just below title bar)
void window_draw_start(struct window * w){
    uint8_t * client_area = w->p->buffer + (TITLE_BAR_HEIGHT * w->p->x_size * VBE_BYTES_PER_PIXEL);
    vbe_set_draw_target(client_area,w->width,w->height);
}

void window_draw_end(struct window * w){
    vbe_reset_draw_target();
    draw_window_frame(w); //so app cant draw over border
}

void window_clear(struct window * w,uint8_t RED,uint8_t GREEN,uint8_t BLUE){
    window_draw_start(w);
    vbe_draw_rect_solid(0,0,w->width,w->height,RED,GREEN,BLUE);
    window_draw_end(w);
}

//top most window at x,y of screen
struct window * window_at(int x,int y){
    struct window * top = NULL;
    for(int i=0;i<MAX_WINDOWS;i++){
        struct window * w = &windows[i];
        if(!w->in_use) continue;
        if(x >= w->p->x_pos && x < w->p->x_pos + w->p->x_size && y >= w->p->y_pos && y < w->p->y_pos + w->p->y_size){
            if(top == NULL || w->p->Z_POS > top->p->Z_POS) top = w;
        }
    }
    return top;
}

//---MOUSE CLICK---
void window_click(int x,int y){
    struct window * w = window_at(x,y);
    if(w == NULL) return;

    if(w != focused_window) window_focus(w);

    //position inside window plane
    int local_x = x - w->p->x_pos;
    int local_y = y - w->p->y_pos;

    if(local_y < TITLE_BAR_HEIGHT){ //title bar
        int close_x = w->width - CLOSE_BTN_SIZE - 3;
        if(local_x >= close_x && local_x < close_x + CLOSE_BTN_SIZE){
            close_window(w);
        }
        return;
    }

    if(w->on_click != NULL) w->on_click(w,local_x,local_y - TITLE_BAR_HEIGHT);
}

//call update of all windows (every frame)
void windows_update(){
    for(int i=0;i<MAX_WINDOWS;i++){
        if(windows[i].in_use && windows[i].on_update != NULL){
            windows[i].on_update(&windows[i]);
        }
    }
}
