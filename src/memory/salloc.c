// Small alloc on top of alloc_page();

/*
BIG MAP -> stores addresses of BIG PAGES
BIG PAGE -> 4KB BIG PAGE allocated by alloc_page()

BIG PAGE
    |
    V
First 128 bytes -> Map of sectors
remaining 3968 bytes -> Space for storage (HEAP)
sector -> 4 bytes
Total 1024 sectors -> 32 in MAP -> 992 To store things

salloc -> returns address of each sector

*/

#include <stdint.h>
#include "memory.h"
#include "debug.h"

uint32_t ** bigmap; //store address of big paages
uint32_t counter_big_page = 0; //to count how many big pages are allocated to salloc

int new_big_page(){ //to get new big page
    uint32_t * big_page = (uint32_t *)alloc_page();
    counter_big_page++; //starting to store from index 1
    bigmap[counter_big_page] = big_page;

    //initializing map for a individual big page
    for(int i=0;i<4;i++) big_page[i]=0XFFFFFFFF;
    for(int i=4;i<128;i++) big_page[i] = 0x0;

    return 0;
}

void * salloc(uint32_t bytes,uint32_t multiplier){ //NOTE: WATCH BOUNDARIES
    start:
    for(int i=1;i<=counter_big_page;i++){
        uint32_t * temp_ptr_big_page = bigmap[i];

        //first 128 bytes are bit map
        for(int j=33;j<992;j++){ //checking to get free sector
            //Get no of bit in bitmap
            //check if its free or not

            int word_index = j/32;
            int bit_index = j%32;

            if(!(temp_ptr_big_page[word_index] & (1<<bit_index))){// jth sector is not in use

                //allocate enough sectors for all bytes
                int sector_needed = (bytes*multiplier)/4;
                // if(sector_needed==0) sector_needed++;
                for(int k=0;k<sector_needed;k++){ //check if needed number of sectors are available or not
                    int word_index_2 = (j+k)/32;
                    int bit_index_2 = (j+k)%32;

                    if(temp_ptr_big_page[word_index_2] & (1<<bit_index_2)){ // j+k sector is in use
                        j += (k-1); //now look at next available big chunk (-1 cuz loop will do j++)
                        goto loop_end;
                    }
                }

                temp_ptr_big_page[word_index] = (temp_ptr_big_page[word_index]) | (1<<bit_index); //set sector in use
                for(int k=0;k<sector_needed;k++){ //Mark all sectors in use
                    int word_index_2 = (j+k)/32;
                    int bit_index_2 = (j+k)%32;

                    temp_ptr_big_page[word_index_2] = (temp_ptr_big_page[word_index_2]) | (1<<bit_index_2);
                }
                return (temp_ptr_big_page + j);
            }

            loop_end:
        }
    }
    //ALL BIG PAGES USED, get new one
    serial_print("getting new Big page\n");
    new_big_page();
    goto start;
}

int sfree(void * address, uint32_t bytes,uint32_t multiplier){ 

// NOTE: Here Base_Address + 1024 will give whole 4KB range cuz pointer is of 32 bit (4 bytes)

for(int i=1; i<=counter_big_page;i++){
    uint32_t * temp_ptr_big_page = bigmap[i];
    if((address >= bigmap[i]) && (address <= (bigmap[i] + 1024 ))){ //in the range of current BIG page
        uint32_t sector_no = ((uint32_t)address - (uint32_t)bigmap[i])/4;
        int sector_needed = (bytes*multiplier)/4;

        for(int k=0;k<sector_needed;k++){ //Mark all sectors free
            int word_index_2 = (sector_no+k)/32;
            int bit_index_2 = (sector_no+k)%32;

            temp_ptr_big_page[word_index_2] = (temp_ptr_big_page[word_index_2]) & ~(1<<bit_index_2);
        }
    }
}
    return 0;
}

int init_salloc(){
    //big map init
    bigmap = (uint32_t **)alloc_page(); //4KB can hold upto 1024 page addresses
    memInit_fast(bigmap, 4096);
    new_big_page(); //creating a new big page

    serial_print("[init]: salloc initialization complete\n");

    return 0;
}

/*
Things needed:
Big page( 4KB by alloc_page())
Bit map per big page - (Track every 4 Bytes) - will be stored in each big page (will take first 128 bytes)
Tracking every 4 bytes cuz we majorly store integers and stuff and with this way the map will only take about
3.13% of big page

Map of Big pages - to know how many pages are under use of salloc (EYE ON LIMITS 4KB)
*/