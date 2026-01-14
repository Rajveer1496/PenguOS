#ifndef THREADING_H
#define THREADING_H

extern struct thread* create_thread(void (* function_pointer));
extern uint32_t switchThread(uint32_t esp);

#endif