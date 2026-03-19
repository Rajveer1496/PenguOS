#include<stdint.h>
#include "memory.h"

#define PRESENT 0x1
#define WRITABLE 0x2
#define USER 0x4

extern void * alloc_page();
extern void enable_paging(uint32_t* directory);
extern void printToscreen(const char *str,unsigned char color);

uint32_t* directory;
uint32_t* page_table;

//For kernel
void paging_init(){ 

    //not keeping alloc_page() at global declaration, cuz when c runtime will start it will set up gloabal variables and try to call alloc_page(), but the pmm will not be initialized then, so it will cause the issue

    directory = (uint32_t *)alloc_page();

    for(uint32_t i=0;i<1024;i++){
        page_table = (uint32_t *)alloc_page();
        directory[i] = (uint32_t)page_table | PRESENT | WRITABLE;

        for(uint32_t j=0; j<1024; j++){
            page_table[j] = ((0x1000 * j) + (0x400000 * i)) | PRESENT | WRITABLE;
        }
    }

    enable_paging(directory);
}



//For processes ( LATER )
// void * create_page_directory(){

// }

// void switch_page_directory(void * directory){

// }
