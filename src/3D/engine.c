#include <T3D.h>
#include "graphics.h"
#include <stdint.h>
#include "maths.h"
#include "debug.h"

#define floor(y) ((int)y)
#define intensity(x) ((int)((x)*(255)))

#define X_2D(x,y) (int)(((((x)*scale) - camera_x)/(camera_z-((y)*scale)))*focal_length + (VBE_WIDTH/2))
#define Y_2D(x,y) (int)(((((x)*scale) - camera_y)/(camera_z-((y)*scale)))*focal_length + (VBE_HEIGHT/2))
#define LINE(k,m) vbe_draw_line(X_2D(x##k##_rot,z##k##_rot),Y_2D(y##k##_rot,z##k##_rot),X_2D(x##m##_rot,z##m##_rot),Y_2D(y##m##_rot,z##m##_rot),1);

extern uint16_t VBE_WIDTH;
extern uint16_t VBE_HEIGHT;

void render_obj(struct obj * obj,float camera_x,float camera_y,float camera_z,float scale,float x_rot_angle,float y_rot_angle,float z_rot_angle){
    //TODO

    float focal_length = VBE_WIDTH/2;

    // Rotation
    float sin_X;
    float cos_X;
    sincos(x_rot_angle,&sin_X,&cos_X);

    float sin_Y;
    float cos_Y;
    sincos(y_rot_angle,&sin_Y,&cos_Y);

    float sin_Z;
    float cos_Z;
    sincos(z_rot_angle,&sin_Z,&cos_Z);



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

            //Rotate X axis
            float x1_rot = x1;
            float y1_rot = (y1 * (cos_X)) - (z1 * (sin_X));
            float z1_rot = (y1 * (sin_X))+ (z1 * (cos_X));

            float x2_rot = x2;
            float y2_rot = (y2 * (cos_X)) - (z2 * (sin_X));
            float z2_rot = (y2 * (sin_X))+ (z2 * (cos_X));

            float x3_rot = x3;
            float y3_rot = (y3 * (cos_X)) - (z3 * (sin_X));
            float z3_rot = (y3 * (sin_X))+ (z3 * (cos_X));

            float x_temp;
            float y_temp;
            float z_temp;

            //--------------Rotate Y axis------------
            x_temp = x1_rot;
            y_temp = y1_rot;
            z_temp = z1_rot;
            
            // y1_rot = y1_rot;
            x1_rot = (x_temp * (cos_Y)) + (z_temp * (sin_Y));
            z1_rot = (-1)*(x_temp * (sin_Y)) + (z_temp * (cos_Y));

            x_temp = x2_rot;
            y_temp = y2_rot;
            z_temp = z2_rot;
            
            // y1_rot = y1_rot;
            x2_rot = (x_temp * (cos_Y)) + (z_temp * (sin_Y));
            z2_rot = (-1)*(x_temp * (sin_Y)) + (z_temp * (cos_Y));

            x_temp = x3_rot;
            y_temp = y3_rot;
            z_temp = z3_rot;
            
            // y1_rot = y1_rot;
            x3_rot = (x_temp * (cos_Y)) + (z_temp * (sin_Y));
            z3_rot = (-1)*(x_temp * (sin_Y)) + (z_temp * (cos_Y));




            //--------------Rotate Z axis--------------
            x_temp = x1_rot;
            y_temp = y1_rot;
            z_temp = z1_rot;
            // z1_rot = z1_rot
            x1_rot = (x_temp * (cos_Z)) - (y_temp * (sin_Z));
            y1_rot = (x_temp * (sin_Z)) + (y_temp * (cos_Z));

            x_temp = x2_rot;
            y_temp = y2_rot;
            z_temp = z2_rot;
            // z2_rot = z2_rot
            x2_rot = (x_temp * (cos_Z)) - (y_temp * (sin_Z));
            y2_rot = (x_temp * (sin_Z)) + (y_temp * (cos_Z));

            x_temp = x3_rot;
            y_temp = y3_rot;
            z_temp = z3_rot;
            // z3_rot = z3_rot
            x3_rot = (x_temp * (cos_Z)) - (y_temp * (sin_Z));
            y3_rot = (x_temp * (sin_Z)) + (y_temp * (cos_Z));


            LINE(1,2);
            LINE(2,3);
            LINE(1,3);

            // serial_auto_debug();
            // vbe_flipBuffer();

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

            //Rotate X axis
            float x1_rot = x1;
            float y1_rot = (y1 * (cos_X)) - (z1 * (sin_X));
            float z1_rot = (y1 * (sin_X))+ (z1 * (cos_X));

            float x2_rot = x2;
            float y2_rot = (y2 * (cos_X)) - (z2 * (sin_X));
            float z2_rot = (y2 * (sin_X))+ (z2 * (cos_X));

            float x3_rot = x3;
            float y3_rot = (y3 * (cos_X)) - (z3 * (sin_X));
            float z3_rot = (y3 * (sin_X))+ (z3 * (cos_X));

            float x4_rot = x4;
            float y4_rot = (y4 * (cos_X)) - (z4 * (sin_X));
            float z4_rot = (y4 * (sin_X))+ (z4 * (cos_X));

            float x_temp;
            float y_temp;
            float z_temp;

            //--------------Rotate Y axis------------
            x_temp = x1_rot;
            y_temp = y1_rot;
            z_temp = z1_rot;
            
            // y1_rot = y1_rot;
            x1_rot = (x_temp * (cos_Y)) + (z_temp * (sin_Y));
            z1_rot = (-1)*(x_temp * (sin_Y)) + (z_temp * (cos_Y));

            x_temp = x2_rot;
            y_temp = y2_rot;
            z_temp = z2_rot;
            
            // y2_rot = y2_rot;
            x2_rot = (x_temp * (cos_Y)) + (z_temp * (sin_Y));
            z2_rot = (-1)*(x_temp * (sin_Y)) + (z_temp * (cos_Y));

            x_temp = x3_rot;
            y_temp = y3_rot;
            z_temp = z3_rot;
            
            // y3_rot = y3_rot;
            x3_rot = (x_temp * (cos_Y)) + (z_temp * (sin_Y));
            z3_rot = (-1)*(x_temp * (sin_Y)) + (z_temp * (cos_Y));

            x_temp = x4_rot;
            y_temp = y4_rot;
            z_temp = z4_rot;
            
            // y4_rot = y4_rot;
            x4_rot = (x_temp * (cos_Y)) + (z_temp * (sin_Y));
            z4_rot = (-1)*(x_temp * (sin_Y)) + (z_temp * (cos_Y));




            //--------------Rotate Z axis--------------
            x_temp = x1_rot;
            y_temp = y1_rot;
            z_temp = z1_rot;
            // z1_rot = z1_rot
            x1_rot = (x_temp * (cos_Z)) - (y_temp * (sin_Z));
            y1_rot = (x_temp * (sin_Z)) + (y_temp * (cos_Z));

            x_temp = x2_rot;
            y_temp = y2_rot;
            z_temp = z2_rot;
            // z2_rot = z2_rot
            x2_rot = (x_temp * (cos_Z)) - (y_temp * (sin_Z));
            y2_rot = (x_temp * (sin_Z)) + (y_temp * (cos_Z));

            x_temp = x3_rot;
            y_temp = y3_rot;
            z_temp = z3_rot;
            // z3_rot = z3_rot
            x3_rot = (x_temp * (cos_Z)) - (y_temp * (sin_Z));
            y3_rot = (x_temp * (sin_Z)) + (y_temp * (cos_Z));

            x_temp = x4_rot;
            y_temp = y4_rot;
            z_temp = z4_rot;
            // z4_rot = z4_rot
            x4_rot = (x_temp * (cos_Z)) - (y_temp * (sin_Z));
            y4_rot = (x_temp * (sin_Z)) + (y_temp * (cos_Z));

            LINE(1,2);
            LINE(2,3);
            LINE(3,4);
            LINE(1,4);

            // vbe_flipBuffer();
        }
    }
}