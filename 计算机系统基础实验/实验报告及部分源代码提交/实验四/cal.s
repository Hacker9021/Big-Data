.code32
.section .data

start_t:.long 0
end_t:.long 0
time: .ascii "run time: %d\n"
d: .ascii "%d\n"

n: .long 10
lowfn:.long 0
midfn:.long 0
highfn:.long 0

MSG:
.fill 225,1,1
.byte 0,0,0,0,0,0,0,1,0
.long 0
.long 12800
.long 100

LOWF:
.fill 5000000,1,0
MIDF:
.fill 5000000,1,0
HIGHF:
.fill 5000000,1,0
.section .text
.type cal, @function
.global cal
cal:
pushl %ebp
movl %esp, %ebp
pushl %edi
pushl %esi
pushl %ebx 



//----------function code start----------

//调用系统函数clock函数
call clock
mov %eax, start_t

mov $0,%esi
mov n, %edi

lopa:
add $9, %esi
mov MSG(%esi), %eax

//汇编乘法效率较低
//mov $5, %ecx
//imul %ecx, %eax
//优化将乘法转化为加法，提高执行效率
add %eax, %eax
add %eax, %eax
add MSG(%esi), %eax

add $4, %esi
add MSG(%esi), %eax
add $4, %esi
sub MSG(%esi), %eax
add $100, %eax

//汇编除法效率较低
//cdq 
//mov $128, %ebx
//idiv %ebx
//优化将除法转化为加法和移位，提高执行效率
cdq
xor %edx, %edx
add %edx, %eax
sar $7, %eax

add $4, %esi
mov %eax, MSG(%esi)

//判断结果与100的大小，跳转到对应的部分对数据组进行拷贝
sub $21, %esi
cmp $100, %eax
jg highf
je midf
//else -> lowf

lowf:
//去除循环结构，优化为流水线模式
//拷贝每个数据组id(9个字节)
//采用顺序结构，每次拷贝4个字节，耗时较短
movl lowfn, %ebx
movl MSG(%esi), %eax
movl %eax, LOWF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, LOWF(%ebx)
add $4, %esi
add $4, %ebx
movb MSG(%esi), %al
movb %al, LOWF(%ebx)
inc %esi
inc %ebx
//依次拷贝每个数据组的数据a,b,c,sf(每个占4个字节)
//采用顺序结构，每次拷贝4个字节，耗时较短
movl MSG(%esi), %eax
movl %eax, LOWF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, LOWF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, LOWF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, LOWF(%ebx)
add $4, %ebx
movl %ebx, lowfn
jmp next

midf:
movl midfn, %ebx
movl MSG(%esi), %eax
movl %eax, MIDF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, MIDF(%ebx)
add $4, %esi
add $4, %ebx
movb MSG(%esi), %al
movb %al, MIDF(%ebx)
inc %esi
inc %ebx

movl MSG(%esi), %eax
movl %eax, MIDF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, MIDF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, MIDF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, MIDF(%ebx)
add $4, %ebx
movl %ebx, midfn
jmp next

highf:
movl highfn, %ebx
movl MSG(%esi), %eax
movl %eax, HIGHF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, HIGHF(%ebx)
add $4, %esi
add $4, %ebx
movb MSG(%esi), %al
movb %al, HIGHF(%ebx)
inc %esi
inc %ebx

movl MSG(%esi), %eax
movl %eax, HIGHF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, HIGHF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, HIGHF(%ebx)
add $4, %esi
add $4, %ebx
movl MSG(%esi), %eax
movl %eax, HIGHF(%ebx)
add $4, %ebx
movl %ebx, highfn
jmp next

next:
add $4, %esi
dec %edi
jne lopa

//调用系统函数clock函数和printf函数
call clock
mov %eax, end_t
sub start_t, %eax
pushl %eax
pushl $time
call printf

mov $0, %ecx
mov midfn, %edx
lop2print:
pushl MIDF(%ecx)
pushl $d
call printf
add $9, %ecx
pushl MIDF(%ecx)
pushl $d
call printf
add $4, %ecx
pushl MIDF(%ecx)
pushl $d
call printf
add $4, %ecx
pushl MIDF(%ecx)
pushl $d
call printf
add $4, %ecx
cmp midfn, %ecx
je lop2print


//mov $MIDF, %eax
//mov 8(%ebp), %edi
//mov midfn, %ebx
//mov %ebx, (%edi) 

//mov $1, %eax
//mov $0, %ebx
//int $0x80

//----------function code end----------

popl %ebx
popl %esi
popl %edi
movl %ebp, %esp
popl %ebp
ret
