#include<stdint.h>

//copy 4 bytes at a time
void memcpy_fast(void* src, void* dest,uint32_t n){
    uint32_t *s = (uint32_t *)src;
    uint32_t *d = (uint32_t *)dest;

    for(int i=0; i< n/4; i++){
        d[i] = s[i];
    }
}