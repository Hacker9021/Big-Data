# AT&T 格式
# lab1.s
.data    # 数据段声明
result  .int 0

.text
.global _start

_start:
   
   movl $0b0110011, %eax  # x1
   movl $0b1011010, %ebx  # x2

   addl %ebx, %eax

    # 退出程序
    movl $1, %eax   # 退出系统调用号
    int $0x80