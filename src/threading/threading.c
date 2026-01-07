#include <stdint.h>
#include "thread.h"

struct thread threads[100];
int thread_count =0;

struct thread* create_thread(void (* function_pointer)){
    struct thread* t = &(threads[thread_count]);
    thread_count++;

    void* stack = alloc_page();

    uint32_t* stack_ptr = (uint32_t *)((uint32_t)stack + 4096); //point to one past of top of the stack (so can do stack_ptr-- at start)

    stack_ptr--;
    *stack_ptr = 0x0202; //EFLAGS, bit 9 set

    stack_ptr--;
    *stack_ptr = 0x10; //CS (kernel code segment)

    stack_ptr--;
    *stack_ptr = (uint32_t)function_pointer; // EIP
    
    //all other registers
    // .
    // .
    // .
    // .
    // .
    // .
    // .
    // .
    // .

    t->esp = (uint32_t)stack_ptr; //save address as number so cpu can use it
    return t;
}