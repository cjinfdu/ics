---
title: 11.30 第五章习题
date: 2023-11-30 08:00:00
---

### 5.6

这道题说明了最小化一个计算中的操作数量不一定会提高它的性能。

A. 这个函数执行`n`个乘法和`n`个加法，是原始函数`poly`中乘法数量的一半
B. 我们可以看到，这里的性能限制计算是反复地计算表达式`result=a[i]+x*result`。从来自上一次的迭代的`result`的值开始，我们必须先把它乘以`x`（5个时钟周期），然后把它加上`a[i]`（3个时钟周期）。因此，每次迭代需要8个时钟周期。正好等于我们测量到的CPE。
C. 虽然函数`poly`中每次迭代需要两个乘法，而不是一个，但是只有一条乘法是在每次迭代的关键路径上出现。

### 5.9

这道题说明了编码风格上的小变化能够让编译器更容易地察觉到使用条件传送的机会：

```c
while(i1 < n && i2 < n){
    long v1 = src1[i1];
    long v2 = src2[i2];
    long take1 = v1 < v2;
    dst[i1+i2] = take1 ? v1 : v2;
    i1 += take1;
    i2 += (1-take1);
}
```


### 5.13

A. 画图

![Alt text](image.png){ width=50% height=50% }
![Alt text](image-1.png){ width=50% height=50% }
![Alt text](image-2.png){ width=50% height=50% }
关键路径为第三幅图加粗部分

B. 
关键路径上只有浮点数加法CPE为3.0

C.
整数加法的延迟界限，CPE 为 1.00

D.
关键路径上只有浮点加法,结合图理解

### 5.14

```c
void inner4(vec_ptr u, vec_ptr v, data_t *dest) {
  long i;
  long length = vec_length(u);
  data_t *udata = get_vec_start(u);
  data_t *vdata = get_vec_start(v);
  data_t sum = (data_t) 0;
  for (i = 0; i < length-6; i+=6) {
    sum = sum + udata[i] * vdata[i]
              + udata[i+1] * vdata[i+1]
              + udata[i+2] * vdata[i+2]
              + udata[i+3] * vdata[i+3]
              + udata[i+4] * vdata[i+4]
              + udata[i+5] * vdata[i+5];
  }
  for(; i < length; i++) {
    sum = sum + udata[i] * vdata[i];
  }
  *dest = sum;
}
```

A.
虽然迭代次数减少为6/n，但是每次迭代中有6个乘法操作，所以关键路径上还是一共有n个乘法操作。

B.
同上

### 5.16

```c
void inner7(vec_ptr u, vec_ptr v, data_t *dest) {
    long i;
    long length = vec_length(u);
    long limit = length - 5;
    data_t *udata = get_ver_start(u);
    data_t *vdata = get_vec_start(v);
    data_t sum = (data_t)0;

    for (i = 0; i < limit; i += 6) {
        sum = sum + (udata[i] * vdata[i] +
            (udata[i + 1] * vdata[i + 1] +
            (udata[i + 2] * vdata[i + 2] +
            (udata[i + 3] * vdata[i + 3] +
            (udata[i + 4] * vdata[i + 4] +
            udata[i + 5] * vdata[i + 5])))));
    }

    for (; i < length; i++) {
        sum = sum + udata[i] * vdata[i];
    }

    *dest = sum;
}
```

只需要用括号将后面两两括起来，括哪两个都可以，只要是两两括起来就可以

### 5.17

```c
void *new_memset(void *s, int c, size_t n) {
    unsigned long w;
    unsigned char *pw = (unsigned char *)&w;
    size_t cnt = 0;
    while (cnt < K) {
        *pw++ = (unsigned char)c;
        cnt++;
    }

    size_t i;
    unsigned char *schar = s;
    for (i = 0; (size_t)schar % K != 0 || i == n; i++) {
        *schar++ = (unsigned char)c;
    }

    size_t limit = n - K + 1;
    for (; i < limit && (int)limit > 0; i += K) {
        *(unsigned long *)schar = w;
        schar += K;
    }

    for (; i < n; i++) {
        *schar++ = (unsigned char)c;
    }

    return s;
}
```

注意地址对齐

**实际后面的题目比较开放，同学们也可以应用学到的程序优化技巧对自己以前写过的代码尝试优化。**

### 5.18

```c
double poly_6_3a(double a[], double x, long degree) {
  long i = 1;
  double result = a[0];
  double result1 = 0;
  double result2 = 0;

  double xpwr = x;
  double xpwr1 = x * x * x;
  double xpwr2 = x * x * x * x * x;

  double xpwr_step = x * x * x * x * x * x;
  for (; i < degree - 5; i+=6) {
    result = result + (a[i]*xpwr + a[i+1]*xpwr*x);
    result1 = result1 + (a[i+2]*xpwr1 + a[i+3]*xpwr1*x);
    result2 = result2 + (a[i+4]*xpwr2 + a[i+5]*xpwr2*x);

    xpwr *= xpwr_step;
    xpwr1 *= xpwr_step;
    xpwr2 *= xpwr_step;
  }

  for (; i <= degree; i++) {
    result = result + a[i]*xpwr;
    xpwr *= x;
  }

  return result + result1 + result2;
}
```

### 5.19

```c
void psumnew(float a[], float p[], long n) {
  long i;
  float val, last_val;
  float tmp1, tmp2, tmp3, tmp4;
  last_val = p[0] = a[0];
  for (i = 1; i < n - 4; i++) {
    temp1 = last_val + a[i];
    temp2 = temp1 + a[i+1];
    temp3 = temp2 + a[i+2];
    temp4 = temp3 + a[i+3];
    p[i] = temp1;
    p[i+1] = temp2;
    p[i+2] = temp3;
    p[i+3] = temp4;
    last_val = temp4;
  }
  for (; i < n; i++) {
    last_val += a[i];
    p[i] = last_val;
  }
}
```

