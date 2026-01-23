//CONTEXT SWITCHING

#include <stdint.h>
#include "thread.h"
#include "drivers.h"

extern void * alloc_page();

struct thread threads[100];
int thread_count = 0;
int running_thread_no = 0;

struct thread* create_thread(void (* function_pointer)){
    thread_count++; //Thread index starting from 1
    struct thread* t = &(threads[thread_count]);

    void* stack = alloc_page();

    uint32_t* stack_ptr = (uint32_t *)((uint32_t)stack + 4096); //point to one past of top of the stack (so can do stack_ptr-- at start)

    // WHAT PROCESSOR PUSHES AUTOMATICALLY UPON INTERRUPT (Replicating that)
    stack_ptr--;
    *stack_ptr = 0x0202; //EFLAGS, bit 9 set

    stack_ptr--;
    *stack_ptr = 0x8; //CS (kernel code segment)

    stack_ptr--;
    *stack_ptr = (uint32_t)function_pointer; // EIP

    // PUSHAD (what pushad instruction pushes to stack)
    
    //all other registers
    uint32_t temp = (uint32_t)stack_ptr;

    stack_ptr--;
    *stack_ptr = 0x0; //EAX

    stack_ptr--;
    *stack_ptr = 0x0; //ECX

    stack_ptr--;
    *stack_ptr = 0x0; //EDX

    stack_ptr--;
    *stack_ptr = 0x0; //EBX

    stack_ptr--;
    *stack_ptr = temp; //ESP (Orignal) (Before PUSHAD instruction)

    stack_ptr--;
    *stack_ptr = 0x0; //EBP

    stack_ptr--;
    *stack_ptr = 0x0; //ESI

    stack_ptr--;
    *stack_ptr = 0x0; //EDI

    //SAVING SEGMENT REGISTERS

    stack_ptr--;
    *stack_ptr = 0x10; //ds (0x10 - Kernel Data Segment)
    
    stack_ptr--;
    *stack_ptr = 0x10; //es

    stack_ptr--;
    *stack_ptr = 0x10; //fs

    stack_ptr--;
    *stack_ptr = 0x10; //gs

    // SAVING INTERRUPT NUMBER

    stack_ptr--;
    *stack_ptr = 0x0; // DUMMY INTERRUPT NUMBER

    stack_ptr--;
    *stack_ptr = 0x0; // DUMMY Second parameter
    
    t->esp = (uint32_t)stack_ptr; //save address as number so cpu can use it
    return t;
}

uint32_t switchThread(uint32_t esp){
    //Save current Thread's ESP
    if(running_thread_no != 0){
        threads[running_thread_no].esp = esp;
    }

    // change STACK POINTER
    if(thread_count <= 0){
        return 1;
    }
    else if(running_thread_no < thread_count){
        running_thread_no++;
        // writeESP((uint32_t)(threads[running_thread_no].esp)); //STACK DISASTER, before calling writeESP we are using thread A's stack and after calling it we will be on Thread B's Stack, All EIP values will be changed
        return (uint32_t)(threads[running_thread_no].esp);
    }else{
        running_thread_no = 1;
        // writeESP((uint32_t)(threads[running_thread_no].esp));
        return (uint32_t)(threads[running_thread_no].esp);
    }
}