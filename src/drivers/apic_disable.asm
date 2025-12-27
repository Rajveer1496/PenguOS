; To disable APIC and tell hardware that we will be using legacy mode PIC for interrupts

section .text

global disable_apic
disable_apic:
mov ecx,0x1B
RDMSR
push ebx
mov ebx,1 ;1
shl ebx,11 ; (1<<11)
not ebx ; ~(1<<11)
and eax,ebx ; value & ~(1<<11)
pop ebx
WRMSR
ret