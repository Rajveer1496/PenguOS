#ifndef MEMORY_H
#define MEMORY_H

//pmm.c
extern void * alloc_page();
extern void free_page(void *address);

//paging.c
void* alloc_continous_pages(uint32_t count);
void free_continous_pages(void *address,uint32_t count);

//memtools.c
void memcpy_fast(void* src, void* dest,uint32_t n);
void memInit_fast(void* ptr, uint32_t Nobytes);

//salloc.c
int init_salloc();
void * salloc(uint32_t bytes,uint32_t multiplier);

#endif