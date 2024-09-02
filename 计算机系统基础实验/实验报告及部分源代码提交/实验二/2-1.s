.section .data   # 优化前计时  as  -g  --32  -o 2-1.o 2-1.s     ld  -m  elf_i386  -dynamic-linker  /lib/ld-linux.so.2  -o 2-1 2-1.o -lc





LOWF:   .fill 1000000,4,0       # 存储低于100的数据



MIDF:   .fill 1000000,4,0         # 存储等于100的数据



HIGHF:  .fill 1000000,4,0        # 存储高于100的数据



sdmid:  .fill  300000, 4, 1000

        .fill  300000, 4, 2540

        .fill  300000, 4, 5000



time: .long 0



msg: .ascii  "the time is %d us \n"





.section .text



.global _start



    call clock



    movl %eax, time

_start:

   

    mov $300000, %ecx        # 设置组数，这里假设有每组数据执行10000次



    lea sdmid, %esi      # 加载数据的地址



    mov $LOWF, %edi      # 设置初始的存储区



    mov $MIDF, %ebx



    mov $HIGHF, %edx











process_data:



    movl 0(%esi), %eax   # 加载a的值到寄存器







    # 计算f = (5a + b - c + 100) / 128



    imull $5, %eax       # 5a



    addl 4(%esi), %eax      # 5a + b



    subl 8(%esi), %eax      # 5a + b - c



    addl $100, %eax      # 5a + b - c + 100



    sarl $7, %eax        # (5a + b - c + 100) / 128       0x804916c







    # 判断f的值，根据条件复制到不同的存储区



    cmp $100, %eax



    jl copy_to_LOWF



    je copy_to_MIDF



    jg copy_to_HIGHF







copy_to_LOWF:



    movl 0(%esi), %eax    



    add %eax, (%edi)



    movl 4(%esi), %eax    



    add %eax, 4(%edi)



    movl 8(%esi), %eax    



    add %eax, 8(%edi)



    add $12, %edi



    jmp end_of_loop







copy_to_MIDF:



    movl 0(%esi), %eax    



    add %eax, (%ebx)



    movl 4(%esi), %eax    



    add %eax, 4(%ebx)



    movl 8(%esi), %eax    



    add %eax, 8(%ebx)



    add $12, %ebx



    jmp end_of_loop







copy_to_HIGHF:



    movl 0(%esi), %eax    



    add %eax, (%edx)



    movl 4(%esi), %eax    



    add %eax, 4(%edx)



    movl 8(%esi), %eax    



    add %eax, 8(%edx)



    add $12, %edx



    jmp end_of_loop







end_of_loop:



    add $12, %esi        # 移动到下一组数据



    dec %ecx             # 减小循环计数器



    jnz process_data     # 继续处理下一组数据







    # 退出程序

    call clock



    movl time, %ebx



    subl %ebx, %eax



    movl %eax, time



    pushl time



    pushl $msg



    call printf



    mov $1, %eax



    int $0x80