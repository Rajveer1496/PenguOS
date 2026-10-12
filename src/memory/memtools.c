#include<stdint.h>
#include "memory.h"

//copy 4 bytes at a time
void memcpy_fast(void* src, void* dest,uint32_t n){
    uint32_t *s = (uint32_t *)src;
    uint32_t *d = (uint32_t *)dest;

    for(uint32_t i=0; i< n/4; i++){
        d[i] = s[i];
    }

    //remaining bytes (if n is not multiple of 4)
    uint8_t *s8 = (uint8_t *)src;
    uint8_t *d8 = (uint8_t *)dest;
    for(uint32_t i=(n/4)*4; i<n; i++){
        d8[i] = s8[i];
    }
}

//fast initialize to 0 (4 bytes at a time)
void memInit_fast(void* ptr, uint32_t Nobytes){
    uint32_t *p = (uint32_t *)ptr;

    for(uint32_t i=0; i < (Nobytes)/4; i++){
        p[i] = 0;
    }
}