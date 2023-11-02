---
title: 11.2 第三章习题
date: 2023-11-02 08:00:00
---

## 3.46 (p196)

### A

```
00 00 00 00 00 40 00 76
01 23 45 67 89 ab cd ef
?? ?? ?? ?? ?? ?? ?? ??
?? ?? ?? ?? ?? ?? ?? ?? <- rsp指向这个八字节
```

### B

```
00 00 00 00 00 40 00 34
33 32 31 30 39 38 37 36
35 34 33 32 31 30 39 38
37 36 35 34 33 32 31 30 <- rsp指向这个八字节
```

### C

在 gets 后，程序返回地址被溢出数据修改为了 0x400034

### D

由于 rbx 的值保存在了栈上，且被覆盖掉了，所以 rbx 的值被破坏了。

### E

1. result 应该要分配 strlen + 1 大小的空间，因为 strlen 返回的是字符串字符内容的长度，**还需要额外一位存放 NULL Byte**。
2. 没有检查 malloc 返回的指针是否是 NULL。

## 3.58

```c
long decode2(long x, long y, long z) {
    y -= z;
    x *= y;
    return ((y<<63)>>63) ^ x;
}
```

## 3.60

### A

由函数调用规约可知：x 和 n 分别存放在 rdi 和 rsi 中；result 显然存放在 rax 中。
然后就很明显了——mask 存放在 rdx 中。

### B

> movl $1, %edx
>
> movl $0, %eax

result 初始值为 0，mask 初始值为 1。

### C

> testq %rdx, %rdx
>
> jne .L3

如果 mask 不为 0，则继续循环。

### D

> movl %esi, %ecx
>
> ...
>
> salq %cl, %rdx

每轮循环中，mask 都会左移 n 位。

### E

> movq %rdi, %r8
>
> andq %rdx, %r8
>
> orq %r8, %rax

每轮循环中，result 都会与 x&mask 进行或运算。

### F

```c
long loop(long x, int n)
{
    long result = 0;
    long mask;
    for (mask = 1; mask != 0; mask = mask << n) {
        result = result | (x & mask);
    }
    return result;
}
```

## 3.63

```c
long switch_prob(long x, int n) {
    long result = x;
    switch(n) {
        case 0x3c:
        case 0x3e:
            result *= 8;
            break;
        case 0x3f:
            result >> 3;
            break;
        case 0x40:
            x = (result << 4) - x;
        case 0x41:
            result = x * x + 0x4b
            break
        // case 0x3d: 这个可以归属到default里
        default:
            result += 0x4b;
    }
    return result;
}
```

## 3.70

### A

| 字段    | 偏移 |
| ------- | ---- |
| e1.p    | 0x00 |
| e1.y    | 0x08 |
| e2.x    | 0x00 |
| e2.next | 0x08 |

### B

0x10，也即 16 个字节。

### C

```c
void proc (union ele *up) {
    up->e2.x = *(up->e2.next->e1.p) - up->e2.next->e1.y;
}
```

