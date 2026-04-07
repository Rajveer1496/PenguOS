#include <T3D.h>
#include <graphics.h>
#include <stdint.h>

#define floor(y) ((int)y)
#define intensity(x) ((int)((x)*(255)))

#define X_2D(x,y) (int)(((((x)*scale) - camera_x)/(camera_z-((y)*scale)))*focal_length + (VBE_WIDTH/2))
#define Y_2D(x,y) (int)(((((x)*scale) - camera_y)/(camera_z-((y)*scale)))*focal_length + (VBE_HEIGHT/2))
#define LINE(k,m) vbe_draw_line(X_2D(x##k,z##k),Y_2D(y##k,z##k),X_2D(x##m,z##m),Y_2D(y##m,z##m),1);

extern uint16_t VBE_WIDTH;
extern uint16_t VBE_HEIGHT;

void render_obj(struct obj * obj,float camera_x,float camera_y,float camera_z,float scale){
    //TODO

    float focal_length = VBE_WIDTH/2;

    for(int i=1;i<(obj->no_face);i+=12){
        int * face = &obj->face_buffer[i];

        if(face[10] == 0){ // face of 3 vertex
            float x1 = obj->vertex_buffer[((face[0]-1)*3)+1];
            float y1 = obj->vertex_buffer[((face[0]-1)*3)+2];
            float z1 = obj->vertex_buffer[((face[0]-1)*3)+3];

            float x2 = obj->vertex_buffer[((face[3]-1)*3)+1];
            float y2 = obj->vertex_buffer[((face[3]-1)*3)+2];
            float z2 = obj->vertex_buffer[((face[3]-1)*3)+3];

            float x3 = obj->vertex_buffer[((face[6]-1)*3)+1];
            float y3 = obj->vertex_buffer[((face[6]-1)*3)+2];
            float z3 = obj->vertex_buffer[((face[6]-1)*3)+3];

            LINE(1,2);
            LINE(2,3);
            LINE(1,3);

        }else{
            float x1 = obj->vertex_buffer[((face[0]-1)*3)+1];
            float y1 = obj->vertex_buffer[((face[0]-1)*3)+2];
            float z1 = obj->vertex_buffer[((face[0]-1)*3)+3];

            float x2 = obj->vertex_buffer[((face[3]-1)*3)+1];
            float y2 = obj->vertex_buffer[((face[3]-1)*3)+2];
            float z2 = obj->vertex_buffer[((face[3]-1)*3)+3];

            float x3 = obj->vertex_buffer[((face[6]-1)*3)+1];
            float y3 = obj->vertex_buffer[((face[6]-1)*3)+2];
            float z3 = obj->vertex_buffer[((face[6]-1)*3)+3];

            float x4 = obj->vertex_buffer[((face[9]-1)*3)+1];
            float y4 = obj->vertex_buffer[((face[9]-1)*3)+2];
            float z4 = obj->vertex_buffer[((face[9]-1)*3)+3];

            LINE(1,2);
            LINE(2,3);
            LINE(3,4);
            LINE(1,4);
        }
    }
}