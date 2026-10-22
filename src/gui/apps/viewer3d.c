// 3D viewer app - renders .obj files (from kernel BIN) inside a window using 3D engine

#include <stddef.h>
#include "apps.h"
#include "graphics.h"
#include "T3D.h"
#include "debug.h"

#define VIEWER_WIDTH 480
#define VIEWER_HEIGHT 360
#define NO_OF_MODELS 2

extern uint32_t timer;
extern uint16_t current_tps;

struct window * viewer_window = NULL;

struct model{
    char name[16];
    struct obj obj;
    float camera_z;
    float scale;
};

struct model models[NO_OF_MODELS];
int models_loaded = 0;
int current_model = 0;

float viewer_angle = 0;
int viewer_paused = 0;
uint32_t viewer_last_frame = 0;

void copy_name(char * dest,const char * src){
    int i=0;
    for(;src[i] != '\0' && i<15;i++) dest[i] = src[i];
    dest[i] = '\0';
}

void load_models(){ //parse .obj only once, takes time for big files
    if(models_loaded) return;

    OBJ_INIT(ak47_normal)
    models[0].obj = ak47_normal;
    copy_name(models[0].name,"ak47");
    models[0].camera_z = 60;
    models[0].scale = 100;

    OBJ_INIT(ship)
    models[1].obj = ship;
    copy_name(models[1].name,"ship");
    models[1].camera_z = 1000;
    models[1].scale = 1;

    models_loaded = 1;
    serial_print("[3D viewer]: models loaded\n");
}

void viewer_draw(struct window * w){
    struct model * m = &models[current_model];

    window_draw_start(w);
    vbe_draw_rect_solid(0,0,w->width,w->height,0,0,0);

    render_obj(&m->obj,0,0,m->camera_z,m->scale,viewer_angle/4,viewer_angle/2,0);

    vbe_draw_string(8,8,m->name,80,220,100);
    vbe_draw_string(8,w->height-20,"TAB: model  +/-: zoom  SPACE: pause",150,150,150);
    window_draw_end(w);
}

void viewer_update(struct window * w){
    if(timer < viewer_last_frame + (current_tps/30)) return; //30 FPS is enough
    viewer_last_frame = timer;

    if(!viewer_paused) viewer_angle += 0.1;
    viewer_draw(w);
}

void viewer_key(struct window * w,char c){
    struct model * m = &models[current_model];

    if(c == '\t'){ //next model
        current_model = (current_model + 1) % NO_OF_MODELS;
    }else if(c == '+' || c == '='){
        m->scale *= 1.25;
    }else if(c == '-'){
        m->scale *= 0.8;
    }else if(c == ' '){
        viewer_paused = !viewer_paused;
    }
    viewer_draw(w);
}

void viewer_close(struct window * w){
    (void)w;
    viewer_window = NULL;
}

void open_viewer3d(){
    if(viewer_window != NULL){
        window_focus(viewer_window);
        return;
    }

    load_models();

    viewer_window = create_window("3D Viewer",500,80,VIEWER_WIDTH,VIEWER_HEIGHT);
    if(viewer_window == NULL) return;

    viewer_window->on_draw = viewer_draw;
    viewer_window->on_key = viewer_key;
    viewer_window->on_update = viewer_update;
    viewer_window->on_close = viewer_close;

    viewer_draw(viewer_window);
}
