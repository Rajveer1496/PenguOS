#include<stdint.h>

//copy 4 bytes at a time
void memcpy_fast(void* src, void* dest,uint32_t n){
    uint32_t *s = (uint32_t *)src;
    uint32_t *d = (uint32_t *)dest;

    for(int i=0; i< n/4; i++){
        d[i] = s[i];
    }
}

//fast initialize to 0 (4 bytes at a time)
void memInit_fast(void* ptr, uint32_t Nobytes){
    uint32_t *p = (uint32_t *)ptr;

    for(int i=0; i < (Nobytes)/4; i++){
        p[i] = 0;
    }
}