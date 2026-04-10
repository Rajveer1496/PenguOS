#ifndef T3D_H
#define T3D_H

#include <stdint.h>

struct obj{
    int no_vertex; //index starting from 1
    float * vertex_buffer;

    int no_normal; //start index from 1
    float * normal_buffer;

    int no_face; //index starting from 1 //also includs normal indexes
    int * face_buffer;
};

#define OBJ_INIT(x) \
    extern uint32_t _binary_3D_asset_##x##_obj_start;   \
    extern uint32_t _binary_3D_asset_##x##_obj_size;    \
    char * obj_##x = (char *)&_binary_3D_asset_##x##_obj_start;   \
    struct obj x; \
    obj_store(obj_##x,&x,(uint32_t)&_binary_3D_asset_##x##_obj_size);

void ObjInit(char * source,struct obj* destination, uint32_t size_bytes);
void objDestroy(struct obj* obj);
void obj_store(char * source,struct obj* destination, uint32_t size_bytes);

//Engine
void render_obj(struct obj * obj,float camera_x,float camera_y,float camera_z,float scale,float x_rot_angle,float y_rot_angle,float z_rot_angle);


#define OBJ_INFO_PRINT(x) \
    serial_print_number(x.no_vertex/3); \
    NEW_LINE    \
    serial_print_number(x.no_face/4);   \
    NEW_LINE    \
    serial_print("v "); \
    for(int i=1;i<x.no_vertex;i++){ \
        serial_print_float(x.vertex_buffer[i],6);   \
        serial_print(" ");  \
        if(i%3 == 0 && i!=0) {  \
            NEW_LINE \
            serial_print("v "); \
        }   \
    }   \
    NEW_LINE    \
    serial_print("vn "); \
    for(int i=1;i<x.no_normal;i++){ \
        serial_print_float(x.normal_buffer[i],6);   \
        serial_print(" ");  \
        if(i%3 == 0 && i!=0) {  \
            NEW_LINE \
            serial_print("vn "); \
        }   \
    }   \
    NEW_LINE    \
    serial_print("f "); \
    for(int i=1;i<x.no_face;i+=12){   \
        for(int j=0;j<12;j++){  \
            serial_print_number(x.face_buffer[i+j]);  \
            if((i+j)%3 == 0) serial_print(" ");  \
            else if(i+j != 12) serial_print("/"); \
        }   \
        NEW_LINE \
        serial_print("f "); \
    }



#endif