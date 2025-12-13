#include <stdint.h>
#include <stddef.h>

#define maxPages 4194304 //for 16 GB RAM

extern uint32_t _kernel_end; //label which is at end of kernel
uint32_t *bitmap = (uint32_t *)&_kernel_end; // start bitmap exactly where kernel ends

void set_page_free(uint32_t pagenum){
    uint32_t word_index = pagenum/32; //to get which 32 bit word has this page, e.g bitmap[0] : 0-31 pages
    uint32_t bit_index = pagenum % 32; //to get the page bit

    //Manipulating the exact bit that refers to given page
    bitmap[word_index] &= ~(1 << bit_index); //setting it to 0, free
}

void set_page_inUse(uint32_t pagenum){
    uint32_t word_index = pagenum/32;
    int32_t bit_index = pagenum % 32;

    bitmap[word_index] = (bitmap[word_index]) | (1<<bit_index);
}

int check_page_usage(uint32_t pagenum){
    uint32_t word_index = pagenum/32;
    int32_t bit_index = pagenum % 32;

    if((bitmap[word_index]) & (1<<bit_index)) return 1;
    else return 0;
}

//to allocate the page
void * alloc_page(){
    uint32_t pagenum = 0;
    while(pagenum < maxPages){
        if(!check_page_usage(pagenum)){
            set_page_inUse(pagenum);
            return (void *)(4096 * pagenum); //Actual Physical adress of the page
        }
        pagenum++;
    }
    return NULL;
}

//to free the page
void free_page(void *address){
    uint32_t addr = (uint32_t)address; //convert pointer to int, for adress
    uint32_t pageNo = addr/4096;
    set_page_free(pageNo);
}


//Initialize the bitmap
/*Reserved pages:
0-255 (BIOS, VGA, etc)
256 - where kernel Ends 
Bitmap pages (where kernel ends to 128 pages)
*/

void page_init(){

    //all pages to free
    for(int i=0; i< (maxPages/32);i++){ //loop trhough all words
        bitmap[i] = 0;
    }

    //reserved pages
    uint32_t kernel_end_address = (uint32_t)&_kernel_end; //_kernel_end is a symbol, gotta convert it into adress
    uint32_t kernelEndpage = (kernel_end_address) / 4096;
    for(uint32_t i=0; i < kernelEndpage + 200 ; i++){ //can do 128 but for safety using 200
        set_page_inUse(i);
    }
}

void* alloc_continous_pages(uint32_t count){
    uint32_t pagenum = 0;
    while(pagenum < maxPages-count){ //make sure we have enough pages
        outer:
        if(!check_page_usage(pagenum)){ //first unused page
            for(int i=0;i<count;i++){//check next pages for continous block
                if(check_page_usage(pagenum + i)){ //search in next space if any used page found in between
                    pagenum = pagenum + i;
                    goto outer;
                }
            }
            //if continous block found
            for(int i=0;i<count;i++){//mark whole block in use
                set_page_inUse(pagenum+i);
            }
            return (void *)(4096 * pagenum); //Actual Physical adress of the page
        }
        pagenum++;
    }
    return NULL;
}

void free_continous_pages(void *address,uint32_t count){
    uint32_t addr = (uint32_t)address; //convert pointer to int, for address
    uint32_t pageNo = addr/4096;

    for(int i=0;i<count;i++){ //free the whole continous block
        set_page_free(pageNo + i);
    }
}