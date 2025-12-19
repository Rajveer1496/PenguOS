#ifndef MEMORY_H
#define MEMORY_H

//paging.c
void* alloc_continous_pages(uint32_t count);
void free_continous_pages(void *address,uint32_t count);

//memtools.c
void memcpy_fast(void* src, void* dest,uint32_t n);
void memInit_fast(void* ptr, uint32_t Nobytes);

#endif