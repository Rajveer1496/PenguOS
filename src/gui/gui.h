#ifndef GUI_H
#define GUI_H

#include <stdint.h>

#define MAX_PLANES 100
#define ALWAYS_ON_TOP 100000 //Z_POS for taskbar
#define TASKBAR_HEIGHT 32

struct plane{
    int x_size;
    int y_size;

    int x_pos;
    int y_pos;

    uint8_t * buffer;
    uint32_t pages; //no of pages used by buffer (to free it later)

    int Z_POS; // for render priority
    int in_use;
    int visible; //hidden planes are not rendered (minimized windows)
};

#define MAX_WINDOWS 20
#define MAX_TITLE 32
#define TITLE_BAR_HEIGHT 24
#define CLOSE_BTN_SIZE 18

struct window{
    char title[MAX_TITLE];
    struct plane * p;
    int width; //client area (without title bar)
    int height;
    int in_use;
    int minimized;

    //app callbacks (NULL if not needed)
    void (*on_draw)(struct window * w);
    void (*on_click)(struct window * w,int x,int y); //x,y inside client area
    void (*on_key)(struct window * w,char c);
    void (*on_update)(struct window * w); //every frame
    void (*on_close)(struct window * w);

    void * data; //app specific data
};

//planes.c
struct plane * create_plane(int x_pos,int y_pos,int x_size,int y_size);
void delete_plane(struct plane * p);
void plane_draw_start(struct plane * p);
void plane_draw_end();
void render_planes();

//window.c
struct window * create_window(const char * title,int x,int y,int width,int height);
void close_window(struct window * w);
void window_focus(struct window * w);
void draw_window_frame(struct window * w);
void window_draw_start(struct window * w);
void window_draw_end(struct window * w);
void window_clear(struct window * w,uint8_t RED,uint8_t GREEN,uint8_t BLUE);
struct window * window_at(int x,int y);
void window_click(int x,int y);
void window_drag(int mouse_x,int mouse_y);
void windows_update();
void window_minimize(struct window * w);
void window_restore(struct window * w);

//desktop.c
void gui_main();

#endif
