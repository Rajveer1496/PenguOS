section .text

extern serial_print

%macro SERIAL_PRINT 1
    push dword %1
    call serial_print
    add esp,4
    push dword NEW_LINE
    call serial_print
    add esp,4
%endmacro

global checkFPU
checkFPU:
    mov edx,CR0
    and edx,0xFFFFFFF3 ;clear TS and EM bit
    mov CR0,edx
    fninit  ;load default FPU state
    mov ax,0x77 ;store garbage to detect the change
    fnstsw ax ;load fpu status word
    cmp ax,0
    jne .noFPU
    SERIAL_PRINT FPU_PRESENT
    ret

.noFPU:
    SERIAL_PRINT FPU_NOT_PRESENT
    ret

FPU_PRESENT db "[FPU]: FPU IS PRESENT",0
FPU_NOT_PRESENT db "[FPU]: FPU NOT PRESENT!",0
NEW_LINE db 10,0