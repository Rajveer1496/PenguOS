;paging.asm - to enable paging


section .text

global enable_paging
enable_paging:
    mov eax, [esp+4] ;the first argument of function
    mov cr3, eax           ; Load CR3

    mov eax, cr0
    or eax, 0x80000000     ; Set bit 31
    mov cr0, eax           ; Enable paging

    ret ;without ret the CPU would just keep executing whatever random instructions came after your function in memory → instant crash!