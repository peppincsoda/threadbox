
To disassemble:
`$ objdump --disassemble testapp | less`

Calling convention in clang: System V AMD64 ABI (https://wiki.osdev.org/System_V_ABI#x86-64)
* Order of parameters: rdi rsi rdx rcx r8 r9 stack-rtl
* Caller cleans up
* "The stack is 16-byte aligned just before the call instruction is called."
* "The width of arg slots on the stack is 8, but the first one is at a 16-byte aligned address. But yes, the push %rbp in the function prologue aligned the stack by 16, so subtracting a multiple of 16 when reserving stack space is necessary."
