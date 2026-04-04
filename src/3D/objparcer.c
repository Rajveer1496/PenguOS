// TO read and store .obj data

//TODO: Map index perfectly so we can find the vertex needed by faces

#include <stdint.h>
#include "memory.h"
#include "strings.h"
#include "T3D.h"


void ObjInit(char * source,struct obj* destination){//get obj size in bytes and allocate space
    destination->no_face=1;
    destination->no_vertex=1;

    int vertex_no_floats= 0; //Net number of floats of all vertex
    int face_no_int = 0; //Net number of int of all faces

    for(int i=0;i<100000; i++){ // NOTEEEEEEEEEE: limit to 100000 just temp for safety

        if(source[i] == '\n'){
            if((source[i+1] != 'v') && source[i+1] != 'f' && source[i+1] != 's'){
                //EOF
                destination->vertex_buffer = (float *)alloc_continous_pages(((vertex_no_floats*4)/4096)+1);
                destination->face_buffer = (int *)alloc_continous_pages(((face_no_int*4)/4096)+1);
                break;
            }
        }

        if(source[i] == 'v'){
            if(source[i+1] != ' '){
                continue; // ignoring vt and vn
            }
            //traverse whole line
            for(int j=1;(source[i+j] != '\n') && (j<100); j++){ // j<100 for infinite loop safety
                if(source[i+j] == ' '){
                    vertex_no_floats++;
                }
            }
        }

        if(source[i] == 'f'){
            //traverse whole line
            for(int j=1;(source[i+j] != '\n') && (j<100); j++){ // j<100 for infinite loop safety
                if(source[i+j] == ' '){
                    face_no_int++;
                }
            }
        }
    }
}

void objDestroy(struct obj* obj){
    free_continous_pages(obj->vertex_buffer,(((obj->no_vertex)*4)/4096)+1);
    free_continous_pages(obj->face_buffer,(((obj->no_face)*4)/4096)+1);
}

void obj_store(char * source,struct obj* destination){ 

    ObjInit(source,destination);

    // there should be 'v' or 'f' just after '\n' or its EOF
    for(int i=0;i<100000 ; i++){ // NOTEEEEEEEEEE: limit to 100000 just temp for safety

        if(source[i] == '\n'){
            if((source[i+1] != 'v') && source[i+1] != 'f' && source[i+1] != 's')
                return; //EOF
        }

        if(source[i] == 'v'){
            if(source[i+1] != ' '){
                continue; // ignoring vt and vn
            }
            //traverse whole line
            for(int j=1;(source[i+j] != '\n') && (j<100); j++){ // j<100 for infinite loop safety

                if(source[i+j] == ' '){
                    j++;
                    // traverse one vertex
                    int counter = 0;
                    char char_buffer[20]; //buffer to store one float number as string
                    while(source[i+j+counter] != ' ' && source[i+j+counter] != '\n'){
                        char_buffer[counter] = source[i+j+counter];
                        counter++;
                    }
                    char_buffer[counter] = '\0'; //end of string
                    float vertex_float = stringToFloat(char_buffer);
                    destination->vertex_buffer[destination->no_vertex] = vertex_float;
                    destination->no_vertex++;
                }
                
            }
        }

        if(source[i] == 'f'){
            //traverse whole line
            int face_per_line=0;
            for(int j=1;(source[i+j] != '\n') && (j<100); j++){ // j<100 for infinite loop safety
                if(source[i+j] == ' '){
                    j++;
                    // traverse one face
                    int counter = 0;
                    char char_buffer[20]; //buffer to store one float number as string
                    while(source[i+j+counter] != ' ' && source[i+j+counter] != '\n'){
                        char_buffer[counter] = source[i+j+counter];
                        counter++;
                    }
                    char_buffer[counter] = '\0'; //end of string
                    int face_int = stringToInt(char_buffer);
                    destination->face_buffer[destination->no_face] = face_int;
                    destination->no_face++;
                    face_per_line++;
                }
                
            }
            //some faces are made of 3 vertex and some are from 4. so appending 0 where each face ends in buffer
            if(face_per_line==3){
                destination->face_buffer[destination->no_face] = 0;
                destination->no_face++;
            }
        }
    }
}