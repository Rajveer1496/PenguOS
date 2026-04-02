#ifndef T3D_H
#define T3D_H

struct obj{
    int no_vertex; //index starting from 1
    float * vertex_buffer;

    int no_face; //index starting from 1
    int * face_buffer;
};

void ObjInit(char * source,struct obj* destination);
void objDestroy(struct obj* obj);
void obj_store(char * source,struct obj* destination);

//Engine
void render_obj(struct obj * obj,float x,float y,float z,float scale);


#endif