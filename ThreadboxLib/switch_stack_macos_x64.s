    .globl      _switch_stack

    .p2align    4
_switch_stack:
    push        %rbp
    mov         %rsp, (%rdi)
    mov         %rsi, %rsp
    mov         %rdx, %rdi
    mov         %rdx, %rax
    pop         %rbp
    ret
