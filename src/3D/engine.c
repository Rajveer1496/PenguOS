#include <T3D.h>
#include <graphics.h>
#include <stdint.h>

#define floor(y) ((int)y)
#define intensity(x) ((int)((x)*(255)))

void render_obj(struct obj * obj,float x,float y,float z,float scale){
    for(int i =1;i<(obj->no_vertex);i+=3){

        float x_pos = obj->vertex_buffer[i];
        x_pos *= scale;
        x_pos += x;
        float frac_x = x_pos - (int)x_pos;

        float y_pos = obj->vertex_buffer[i+1];
        y_pos *= scale;
        y_pos += y;
        float frac_y = y_pos - (int)y_pos;

        float z_pos = obj->vertex_buffer[i+2];
        z_pos *= scale;
        z_pos += z;

        vbe_write_pixel_BackBuffer(floor(x_pos),floor(y_pos),intensity((1-frac_x)*(1-frac_y)),0,0);
        vbe_write_pixel_BackBuffer(floor(x_pos)+1,floor(y_pos),intensity(frac_x*(1-frac_y)),0,0);

        vbe_write_pixel_BackBuffer(floor(x_pos),floor(y_pos)+1,intensity((1-frac_x)*frac_y),0,0);
        vbe_write_pixel_BackBuffer(floor(x_pos)+1,floor(y_pos)+1,intensity(frac_x*frac_y),0,0);
    }
    vbe_flipBuffer();
}