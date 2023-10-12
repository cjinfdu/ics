---
title: DataLab writeup
date: 2023-10-12 09:40:00
mathjax: true
---

代码编写注意事项：
1. 非浮点数部分的题目，常数只能用`0~0xFF`，类型只能用`int`。
2. 变量声明应该放在函数最开头，否则`dlc`会报错。这其实是C89的标准，现在已经取消。
3. 本实验中容易遇到的运算符优先级问题：`+ -`优先于`<< >>`；**`==`优先于`&`**；`&`优先于`^`；`^`优先于`|`。背不下来只需要无脑打括号就行了。

## P1

>tconst - return a constant value 0xFFFFFFE0
Legal ops: ! ~ & ^ | + << >>
Max ops: 2
Rating: 1

输出一个常数，二进制下最低5位是0，其它全为1。
由于我们只能使用255（8位）及以下的常数，考虑最后5位赋值为0，然后取反。

```c
int tconst(void) {
    int x = 0x1F;
    return ~x;
}
```

## P2

>bitNand - return ~(x&y) using only ~ and | 
Example: bitNand(4, 5) = -5, bitNand(3, 11) = -4
Legal ops: ~ |
Max ops: 6
Rating: 2

按位与非，相当于按位与取反。
由德摩根定律$\overline{x\land y}=\overline{x}\lor\overline{y}$，同理可用~和&表示按位或非。

参考代码：

```c
int bitNand(int x, int y) {
    return (~x)|(~y);
}
```

## P3

>ezOverflow - determine if the addition of two signed positive numbers overflows, and return 1 if overflowing happens and 0 otherwise
You can assume 0 <= x, 0 <= y
Legal ops: ! ~ & ^ | + << >>
Max ops: 6
Rating: 2

两个32位正整数相加，发生溢出返回1，没有溢出返回0。
回忆第一周的课，我们一起从零开始设计了计算机中如何用二进制表示数字。这种设计保证了两数相加，就算结果上溢出了，在执行加法运算的时候也不需要进行额外操作。
来看本题，如果把32位正整数的符号位一定是0，所以是两个31位的二进制数相加。我们的设计保证了就算符号位改变，如果你把它看成一个无符号数，答案就是正确的。我们只能用31位表示两个正整数相加得到的正整数，所以如果第32位不为0，说明计算的结果溢出了。我们只要移位，把第32位移到第1位，其余置零即可。
`int`类型的右移都是算术右移，若符号位为0，右移补0；若符号位为1，右移补1。这是为了保证“右移”操作后依然有意义。
如我们把4，二进制下的`100`右移一位，得到的是2，这和除以2是等价的。把-4算术右移一位，`11111111 11111111 11111111 11111100`变为`11111111 11111111 11111111 11111110`，得到-2，结果依然是除以2的结果。如果我们用逻辑右移，得到的是正数2147483646，意义不大。

>3右移一位的结果是什么？
-3右移一位的结果是什么？
尝试解释右移一位和除以2的关系。

所以题目中两数相加如果溢出，第32位为1，右移31位后前面会全部补1，我们可以用连续两次逻辑取反`!!`或者用`&1`取出第一位。

```c
int ezOverflow(int x,int y) {
    return ((x+y)>>31)&1;
}
```

## P4

>fastModulo - return x%(2^y)
You can assume 0 <= x, 1 <= y <= 31
Example: fastModulo(0x1234567,0xA) = 0x167
Legal ops: ! ~ & ^ | + << >>
Max ops: 4
Rating: 3

输出`x`除以2的`y`次方得到的余数。
先看$2^y$这一部分，2的幂的二进制表示一定是1后面若干个0。模10...0相当于取出原数最后的若干位，这很容易证明——把模数与原数对齐，截取出原数中从1对应位置开始的更高位，后面补0，得到的数是这个10...0的倍数，模的结果为0，所以答案就是所有模数中的0在原数中对应的位。

```c
int fastModulo(int x,int y) {
    return x&~((~0<<y));
}
```

## P5

>findDifference - return a mask that marks the diffrent bits of y compared to x
Example: findDifference(12, 10) = findDifference(10, 12) = 6
Legal ops: ~ & |
Max ops: 9
Rating: 3

构造一个数，若`x`和`y`中某个位的值不一样，则构造数这一位为1，否则这一位为0。
实际上和异或运算的含义是相同的。但是题目不允许我们使用按位异或运算，我们需要用与或非表示异或：对于每一位，返回`x==1`且`y==0`，或`x==0`且`y==1`。

```c
int findDifference(int x,int y) {
    return (x&~y)|(~x&y);
}
```

## P6

>absVal - return the absolute value of x
Examples: absVal(-10) = 10,absVal(5) = 5
Legal ops: ! ~ & ^ | + << >>
Max ops: 6
Rating: 4

返回`x`的绝对值。
取相反数操作相当于取反加一。我们需要只用位运算达到“正数不变，负数取反加一”的效果，为了避免使用到`if`语句，主要思想就是让取反和加的操作数通过符号位变出来，正数不取反并加0，负数取反并加1，而其中是否取反相当于是否让每一位都异或1，加1操作相当于加上符号位对应的值。

```c
int absVal(int x) {
    int minus = x >> 31;
    return (x ^ minus) + (minus & 1);
}
```

## P7

>secondLowBit - return a mask that marks the position of the second least significant 1 bit.
Examples: secondLowBit(0x00000110) = 0x00000100, secondLowBit(0xFEDC1A80) = 0x00000200, secondLowBit(0)  = 0
Legal ops: ! ~ & ^ | + << >>
Max ops: 8
Rating: 4

对于给定的数，保留从低向高数第二个值为1的位，其它位全部赋值为0。
如果对于一个数进行取反操作，那么它的每一位都和原数不同。把取反后的数加上1，那么这个数的后导1（从最低位开始的所有连续1）将会连续进位，直到遇到第一个0。$$\overline{X011\cdots1}+1=\overline{X100\cdots0}$$这相当于又把最后若干位再次取反了。那么我们最后得到的这个数的$X$部分与原数每一位都不同，而$100\cdots0$这一部分与原数都相同。我们把这个新数与原数进行按位与，就相当于只保留了最低的一个1。
我们把这个结果对原数进行异或操作，现在原数第二低的一个1变成了最低的一个。再进行一次上述操作即可。

```c
int secondLowBit(int x) {
    int y = x^(x&(~x+1));
    return y&(~y+1);
}
```

## P8

>byteSwap - swaps the nth byte and the mth byte
Examples: byteSwap(0x12345678, 1, 3) = 0x56341278, byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
You may assume that 0 <= n <= 3, 0 <= m <= 3
Legal ops: ! ~ & ^ | + << >>
Max ops: 20
Rating: 5

交换32位数中的第`m`和第`n`个字节（从0开始计数）。
我们用一个掩码`0xFF`屏蔽掉所有我们不关心的字节，得到第`n`和第`m`个字节，以及第`n`和第`m`个字节被赋值为0之后的数。把第`n`字节右移`n`个字节再左移`m`个字节，把第`m`字节右移`m`个字节再左移`n`个字节，再用与运算把这两个字节填进去即可。

```c
int byteSwap(int x, int n, int m) {
    int expandn = n << 3;
    int expandm = m << 3;
    int fill = 0xFF;
    int maskN = fill << expandn;
    int maskM = fill << expandm;
    int valN = x & maskN;
    int valM = x & maskM;
    int clear = x & (~maskN) & (~maskM);
    return clear | ((valN >> expandn & fill) << expandm) | ((valM >> expandm & fill) << expandn);
}
```

## P9

>byteCheck - Returns the number of bytes that are not equal to 0
Examples: byteCheck(0xFE0A4100) = 3
Legal ops: ! ~ & ^ | + << >>
Max ops: 24
Rating: 5

返回4个字节中不为0的字节个数。
先创建一个字节的掩码`byte = 0xFF`，用于屏蔽掉另外的三个字节。如果`x & byte`的结果不为0，则答案+1，这可以用两次逻辑取反得到。对四个字节依次操作即可。

```c
int byteCheck(int x) {
    int ans = 0;
    int byte = 0xFF;
    ans = ans + !!(x & byte);
    byte = byte << 8;
    ans = ans + !!(x & byte);
    byte = byte << 8;
    ans = ans + !!(x & byte);
    byte = byte << 8;
    ans = ans + !!(x & byte);
    return ans;
}
```

## P10

>fractions - return floor(x*7/16), for 0 <= x, x is an integer 
Example: fractions(20) = 8
Legal ops: ! ~ & ^ | + << >>
Max ops: 8
Rating: 5

返回一个正数*7/16的结果。
我们在国庆节前的习题课讲过位运算和乘除法的关系（<https://cjinfdu.github.io/ics/Assignment1/>，2.77），`x*7`可以拆成`x*(4+2+1)`，改写成位运算就是`(x<<2)+(x<<1)+x`，请注意加法的优先级高于左移。`x/16`也可以改写成`x>>4`。

>结合P3的思考题和第6题的分析，如果此题没有x非负的限制，应该怎么做？

```c
int fractions(int x) {
    return ((x<<2)+(x<<1)+x)>>4;
}
```

## P11

>biggerOrEqual - if x >= y  then return 1, else return 0 
Example: biggerOrEqual(4,5) = 0.
Legal ops: ! ~ & ^ | + << >>
Max ops: 24
Rating: 6

若$x\ge y$则返回1，否则返回0。
如果两数符号位不同，很显然符号位为1的数更小。
如果两数的符号位相同怎么办？我们知道$x\ge y$等价于$x-y\ge0$，当$x$和$y$符号相同时，有$x-y\in[-y,x]$，所以必定不会溢出。所以我们可以通过$x-y$的符号位判断$x\ge y$是否成立。

```c
int biggerOrEqual(int x, int y) {
    int signx = (x >> 31) & 1;
    int signy = (y >> 31) & 1;
    int x_neg_y_pos = signx & (!signy);
    int x_pos_y_neg = (!signx) & signy;
    int minus = (x + (~y) + 1) >> 31;
    return (!x_neg_y_pos) & (x_pos_y_neg | (!minus) | (!(x ^ y)));
}
```

## P12

>hdOverflow - determine if the addition of two signed 32-bit integers overflows, and return 1 if overflowing happens and 0 otherwise
Legal ops: ! ~ & ^ | + << >>
Max ops: 20
Rating: 6

判断两个32位有符号整数相加是否溢出。
我们证明$$两数相加溢出\Leftrightarrow 两数符号位相同，且两数之和的符号位与原数不同$$证明：
充分性：由P11可知，若两数相加溢出，它们一定同号。两个最大的32位有符号正整数相加，结果最大是`0x7FFF FFFF + 0x7FFF FFFF = 0xFFFF FFFE`，在32位的范围内。所以正数相加溢出的范围为`[0x8000 0000, 0xFFFF FFFE]`，符号位一定改变。对于负数同理。
必要性：
若两数同号，相加后肯定同号。所以符号不同时代表结果溢出了。

```c
int hdOverflow(int x,int y) {
    return (!((x ^ y) >> 31)) & (((x + y) ^ x) >> 31) & 1;
}
```

## P13

>overflowCalc - given binary representations of three 32-bit positive numbers and add them together, return the binary representation of the part where bits are higher than 32.
Examples: overflowCalc(0xffffffff,0xffffffff,0xffffffff) = 2
Legal ops: ! ~ & ^ | + << >>
Max ops: 30
Rating: 7

给出3个32位正整数的二进制表示，求他们相加后溢出部分的值。
直接相加用`int`肯定存不下，但是我们可以拆成两段16位（或者一段2位，一段30位）分别计算，这用32位肯定是能装下的。分别加完之后，把低位那一段的进位加到高位那一段中就是结果了。
>实际上这就是高精度加法的思想，python中的整数范围无限大，就要用到这个思想。把一个大整数拆成很多个可以直接表示的小段放在数组中，然后对于每一小段分别计算，最后处理进位。
想想高精度乘法应该怎么做？

```c
int overflowCalc(int x, int y, int z) {
    int top = (x >> 30 & 3) + (y >> 30 & 3) + (z >> 30 & 3);
    int mask = ~(3 << 30);
    int bottom = (x & mask) + (y & mask) + (z & mask);
    int final = top + (bottom >> 30 & 3);
    return final >> 2;
}
```

## P14

>logicalShift - shift x to the right by n, using a arithmetical shift
Can assume that 0 <= n <= 31
Examples: logicalShift(0x87654321,4) = 0x08765432
Legal ops: ! ~ & ^ | + << >>
Max ops: 20
Rating: 8

用算术右移和其他运算实现逻辑右移。
我们实际要做的就是生成一个最高`n`位为0，其余为1的掩码，处理算术右移后的结果。我们可以用32位1左移`32-n`位，再取反来得到这个效果。特别要注意的是，在C语言标准中`int`类型左移32位是未定义行为。
>C语言的标准允许在编译C语言时，假设用户代码是完全符合标准的，编译器无需进行检查。不管编译器如何把这些语句编译成汇编，都不能算他错，类似于数字逻辑中的“无关项”。这里把int左移32位就是不符合标准的，编译器想怎么做都可以。

我们考虑如何特殊处理`32-n=32`的情况，这里给出两种解决办法：

(1)
我们把左移32位当做左移0位处理，并把一个特殊变量`correction`标记为1。由于`correction`的相反数`-1`是`0xFFFFFFFF`，我们给掩码加上这个值即可；`correction`为`0`时我们加上的也是`0`，不影响结果。

```c
int logicalShift(int x, int n) {
    int empty = (33 + ~n) & 31;
    int mask = ~(~0 << empty);
    int correction = !n;
    mask = mask + ((~correction) + 1);
    return x >> n & mask;
}
```

(2)
对于一个左移操作，我们先左移它的一半，再左移剩下一半。
```c
int logicalShift(int x, int n) {
    int empty = (33 + ~n);
    int l1 = empty >> 1;
    int l2 = empty + (~l1 + 1);
    int mask = ~(~0 << l1 << l2);
    return x >> n & mask;
}
```

## P15

>partialFill - given l, h, fill all even digits within the interval [l, h] with 1 (subscripts starting from 0)
Can assume that 0 <= l <= 15, 16 <= h <= 31
Examples: partialFill(13,16) = 0x00014000
Legal ops: ! ~ & ^ | + << >>
Max ops: 24
Rating: 8

把`[l,h]`区间内的偶数位都填充为1，下标从`0`开始计数。
我们先填充出一个所有偶数位全为1，奇数位全为0的数，然后用掩码的思想把`[l,h]`区间外的数全部屏蔽掉即可。

```c
int partialFill(int l, int h) {
    int fill = 0x55, all, lowErase, hiErase;
    all = fill << 16 | fill;
    all = all << 8 | all;
    lowErase = ~0 << l;
    hiErase = ~0 + (1 << h << 1);
    return all & lowErase & hiErase;
}
```

## P16

>float_abs - Return bit-level equivalent of expression |f| (absolute value of f) for floating point argument f.
Both the argument and result are passed as unsigned int's, but they are to be interpreted as the bit-level representations of single-precision floating point values.
When argument is NaN, return argument.
Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
Max ops: 20
Rating: 3

返回一个浮点数的绝对值的二进制表示。如果是`NaN`就返回原本的数字。
判断`NaN`只需判断阶码为255，同时除开前9位外，剩下的不全为0。
计算绝对值只需要将符号位置0即可。

```c
unsigned float_abs(unsigned uf) {
    unsigned expmask = 0x7F800000;
    if((uf & expmask) >> 23 == 255 && uf << 9) return uf;
    return uf & 0x7FFFFFFF;
}
```

## P17

>float_cmp - Return 1 if uf1 > uf2, and 0 otherwise.
Both of the arguments are passed as unsigned int's, but they are to be interpreted as the bit-level representations of single-precision floating point values.
When any of the arguments is NaN, return 0.
Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
Max ops: 40
Rating: 5

比较两个浮点数的大小。如果任意一个是`NaN`就返回0。
首先需要注意的是，浮点数是有`+0`和`-0`的，两个这样的`float`进行比较是相等的。为了用`unsigned`表示时方便比较，可以先把`-0`全部变为`0`。
之后就普通的比较，首先比较符号位，其次比较指数部分，最后比较小数部分。只需要注意对于负数，指数和小数部分越小的数实际越大。

```c
unsigned float_cmp(unsigned uf1, unsigned uf2) {
    unsigned expmask = 0x7F800000, basemask = 0x7FFFFF, signErase = 0x7FFFFFFF;
    unsigned exp1 = (uf1 & expmask) >> 23, exp2 = (uf2 & expmask) >> 23;
    unsigned base1 = uf1 & basemask, base2 = uf2 & basemask;
    if((exp1 == 255 && base1) || (exp2 == 255 && base2)) return 0;
    if((uf1 & signErase) == 0)uf1 = 0;//change -0 to 0
    if((uf2 & signErase) == 0)uf2 = 0;
    if(!(uf1 >> 31)){//uf1 positive
        if(uf2 >> 31)return 1;
        if(exp1 == exp2)return base1 > base2;
        return exp1 > exp2;
    }else if(uf2 >> 31){
        if(exp1 == exp2)return base1 < base2;
        return exp1 < exp2;
    }
    return 0;
}
```

## P18

>float_pow2 - Return bit-level equivalent of expression f*(2^n) for floating point argument f and integer argument n.
Both the argument and result are passed as unsigned int's, but they are to be interpreted as the bit-level representations of single-precision floating point values.
When argument is NaN, return argument.
You can assume 0 <= n <= 255
Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
Max ops: 50
Rating: 6

返回一个浮点数乘以2的n次方以后的值。如果浮点数是`NaN`就返回原本的浮点数。
一般情况下相当于直接把阶码加上`n`，如果阶码大于等于`255`则变成正无穷，抹去小数部分。需要考虑非规约数的特殊情况。
最小的规约数`1 00000001 00000000000000000000000`表示$2^{-126}
$，最大的非规约数`0 00000000 11111111111111111111111`表示$(1-2^{-23})\times2^{-126}$，非规约数部分指数固定在`-126`，整数部分为`0`，需要特殊处理：非规约数首先小数部分左移，如果最高位的`1`超过了第`23`位，转为规约数继续处理。

```c
unsigned float_pow2(unsigned uf,int n) {
    unsigned expmask = 0x7F800000, basemask = 0x7FFFFF, signErase = 0x7FFFFFFF, sign, exp, base;
    if((uf & expmask) >> 23 == 255 && uf << 9) return uf;
    if((uf & signErase) == 0)return uf;
    sign = uf & ~signErase;
    exp = (uf & expmask) >> 23;
    base = uf & basemask;
    if(exp == 0){
        while(!(base >> 23) && n > 0){
            base <<= 1;
            --n;
        }
        if(base >> 23){
            base ^= 1 << 23;
            ++exp;
        }
    }
    exp += n;
    if(exp >= 255)return sign | (0xFF << 23);
    else if(exp == 0)return sign | base;
    return sign | (exp << 23) | base;
}
```

## P19

>float_i2f - Return bit-level equivalent of expression (float) x.
Result is returned as unsigned int, but it is to be interpreted as the bit-level representation of a single-precision floating point values.
Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
Max ops: 40
Rating: 7

把`int`类型的整数`x`转为`float`。
除了`-2147483648`外，其余均可以转为绝对值处理。首先处理有效位数，`float`的小数位数只有`23`位，可能需要进行舍入。这里采用的是IEEE 754规定中的默认方法：将结果舍入为最接近且可以表示的值，但是当存在两个数一样接近的时候，则取其中的偶数。
我们找到原整数的最高的一个`1`，它的代表转换后的`float`的整数部分，所以不算在`float`的小数有效位数里面，同时它的位置代表阶码的偏移量。之后取出后面紧挨着的最多`23`位，代表小数部分。若截取`23`位后，后面还有值，将其用于计算舍入。需要注意，如果舍入的结果是给小数部分`+1`，有可能产生进位，算上整数部分有可能会从`24`位变成`25`位，需要再次右移一位。

```c
unsigned float_i2f(int x) {
    int valid = 30, sign;
    int frac, shift, plus = 0, cut, rounding, check, first, carry;
    if (!x)return x;
    if (x == -2147483648)return 0xCF000000;
    if (x < 0) {
        x = -x;
        sign = 1;
    } else sign = 0;
    while (!(x >> valid))--valid;
    //valid: Number of significant digits after omitting 1 in the integer part
    if (valid > 23) {
        shift = valid - 23;
        frac = x >> shift;
        
        rounding = x & ((1 << shift) - 1);
        check = shift - 1;
        first = rounding >> check;//the first bit to be dismiss
        if (first) {
            if ((first << check) == rounding) {
                plus = frac & 1;
            } else plus = 1;
        }
        
        frac += plus;
        carry = frac >> 24;
        valid += carry;
        frac >>= carry;
    } else frac = x << (23 - valid);
    return (sign << 31) | (((127 + valid) << 23)) | (frac & 0x7FFFFF);
}
```

## P20

>oddParity - return the odd parity bit of x, that is, when the number of 1s in the binary representation of x is even, then the return 1, otherwise return 0.
Examples:oddParity(5) = 1, oddParity(7) = 0
Legal ops: ! ~ & ^ | + << >>
Max ops: 56
Challenge: You will get 1 extra point if you use less than or equal to 34 ops
Rating: 2

当`x`有偶数个`1`时，返回`1`，否则返回`0`。

方法一：计算出所有`1`的个数
我们考虑首先构造出每相邻两位都不相同的数`0x55555555`和`0xAAAAAAAA`用作掩码，对原数进行按位与，就得到了两个数，代表所有奇数位和所有偶数位。我们也可以换一个角度看得到的这两个数，将其解释为每一个奇数位/偶数位上有几个`1`（0个或1个）。我们把用掩码`0xAAAAAAAA`得到的数右移一位，加到用掩码`0x55555555`得到的数上，新的结果可以解释为每两位上有几个`1`（00个，01个，10个）。
对于这个新结果，我们又分别用掩码`0x33333333`和`0xCCCCCCCC`进行按位与，同理，把掩码`0xCCCCCCCC`得到的数右移两位，加到掩码`0x33333333`得到的数上，新的结果可以解释为每4位上有几个`1`（0000~0100个）。以此类推，最后可以统计出一共出现了多少个`1`，看结果的奇偶即可。
我们考虑如何优化操作符个数。如果构造掩码时，对于每个字节分别赋值，显然是最直观的方法，但是有改进空间。
我们假设掩码是64位的，以此突出给每一个字节逐个赋值的方法的缺点。

```c
mask = 0x55 | (0x55 << 8) | (0x55 << 16) | (0x55 << 24) | (0x55 << 32) | (0x55 << 40) | (0x55 << 48) | (0x55 << 56)
```
上面的代码用了14个操作符。

考虑到掩码的每个字节都是相同的，如果我们首先赋值一个字节，然后把它复制一份变成两个字节，再把这两个字节复制一份变成四个字节，再把复制一次就变成八个字节，就减少了赋值的次数。

```c
//(1)
mask = 0x55 | (0x55 << 8);
mask = mask | (mask << 16);
mask = mask | (mask << 32);
//(2)
mask = 0x55;
mask = mask | (mask << 32);
mask = mask | (mask << 16);
mask = mask | (mask << 8);
```
上面两种方法的效果是一样的，而且都只用了6个操作符。

方法(1)的思想是，每次基于已有的内容扩展一倍，扩展$n$次长度就变成$2^n$。我们都知道指数函数增长很快，要赋值$n$个字节只需要$\log_2n$步，就算有2147483648个字节，也只需要31步。
这个思想被称作**倍增**，它能将线性时间内处理的问题转化为对数时间内处理的问题，是解决实际问题中非常有效且常用的方法。

方法(2)的思想是，要处理长为$x$的字节，可以把它分为两段长为$x/2$的字节，这两段需要处理的问题是完全一样的。然后再把每一段分为更小的两段，这更小的两段要处理的问题也是完全一样的，直到分到每一段都变成最小的问题为止。
具体到本问题，我们首先给`[1,8]`字节赋值一个`0x55`，然后我们把问题拆成给`[1,4]`字节和`[5,8]`字节分别赋值`0x55`，通过左移32位把`[1,4]`字节的情况复制给了`[5,8]`字节。随后又把问题拆成了给`[1,2]`、`[3,4]`、`[5,6]`、`[7,8]`，通过把当前状况左移16位使得每一个区间都有一个`0x55`，最后再执行一次这样的操作，使得每一个字节都是`0x55`。
这个思想被称作**分治**。实际上，某些问题可以被拆分成两个完全相同的子问题，子问题又可以拆分成两个完全相同的子问题，直到我们可以直接给出子问题的答案，此时，父问题在知道子问题的答案后可以很方便的解决。类似(1)的分析，对于规模为$n$的问题，最多拆分$\log_2n$次。给一个数组排序就可以用分治的思想来做，在本学期的数据结构课程中，各位将会学到基于分治思想的高效排序方法。

```c
int oddParity(int x) {
    int pattern1 = 0x55, pattern2, pattern3, pattern4, fill1, fill2, fill3, fill4, ans;
    fill1 = pattern1 << 16 | pattern1;
    fill1 = fill1 << 8 | fill1;
    ans = (x & fill1) + (x >> 1 & fill1);

    pattern2 = 0x33;
    fill2 = pattern2 << 16 | pattern2;
    fill2 = fill2 << 8 | fill2;
    ans = (ans & fill2) + (ans >> 2 & fill2);

    pattern3 = 0xF;
    fill3 = pattern3 << 16 | pattern3;
    fill3 = fill3 << 8 | fill3;
    ans = (ans & fill3) + (ans >> 4 & fill3);

    pattern4 = 0xFF;
    fill4 = pattern4 << 16 | pattern4;
    ans = (ans & fill4) + (ans >> 8 & fill4);

    return !(((ans >> 16) ^ ans) & 1);
}
```

方法二：直接统计奇偶性
题目要求的是求出奇偶性，相当于我们的目的是把32位全部异或起来。类似上述的分治思想，我们首先把前16位和后16位异或起来，得到一个16位的数，再把它的前8位和后8位异或起来，以此类推，最后只剩一位，代表了是否有奇数个`1`。

```c
int oddParity(int x) {
    x = x ^ (x >> 16);
    x = x ^ (x >> 8);
    x = x ^ (x >> 4);
    x = x ^ (x >> 2);
    x = x ^ (x >> 1);
    return !(x & 1);
}
```

## P21

>bitReverse - Reverse bits in an 32-bit integer
Examples: bitReverse(0x80000004) = 0x20000001, bitReverse(0x7FFFFFFF) = 0xFFFFFFFE
Legal ops: ! ~ & ^ | + << >>
Max ops: 56
Challenge: You will get 1 extra point if you use less than or equal to 34 ops
Rating: 2

反转一个32位的二进制数。
本题的思想是经典的分治思想，先交换前16位和后16位，随后对于每16位，交换前8位和后8位，随后对于每8位，交换前4位和后4位，以此类推，最后得到的结果就是反转后的结果。
我们通过构造掩码来实现交换，值为1的位和值为0的位进行交换。节约操作符的关键步骤是下一个掩码是可以通过上一个掩码递推出来的，每次只需要两个操作符。

```c
int bitReverse(int x) {
    int pat16 = 0xFF;
    int get16 = (pat16 << 8) | pat16; //00 00 FF FF
    int change16 = (x << 16) | ((x >> 16) & get16);//EFGHABCD

    int get8 = (get16 << 8) ^ get16; //00 FF 00 FF
    int change8 = ((change16 & get8) << 8) | ((change16 >> 8) & get8);//GHEFCDAB

    int get4 = (get8 << 4) ^ get8; //0F 0F 0F 0F
    int change4 = ((change8 & get4) << 4) | ((change8 >> 4) & get4);//HGFEDCBA

    int get2 = (get4 << 2) ^ get4; //33 33 33 33
    int change2 = ((change4 & get2) << 2) | ((change4 >> 2) & get2);

    int get1 = (get2 << 1) ^ get2;
    return ((change2 & get1) << 1) | ((change2 >> 1) & get1);
}
```

## P22

>mod7 - calculate x mod 7 without using %.
Example: mod7(99) = 1, mod7(-101) = -3
Legal ops: ! ~ & ^ | + << >>
Max ops: 100
Challenge: You will get 2 extra point if you use less than or equal to 56 ops
Rating: 2

这次要计算的模数不是2的幂次了。但是`7`的二进制表示是`111`，这给我们提供了思路。观察发现，`001`、`001 000`、`001 000 000`模7的余数都是`1`；`010`、`010 000`、`010 000 000`模7的余数都是`2`；`100`、`100 000`、`100 000 000`模7的余数都是`3`，也就是说，每三位其实是等价的，我们只需要把所有的三位数加起来。虽然加起来这个和一般会超过7，但是其依然满足这个性质，再次把每三位加在一起，重复这个过程直到答案在`[0,7]`之间，需要注意`7%7=0`，所以如果得到`7`，需要把它变成`0`。
我们依然可以用类似P20方法一的方法，采用分治的思想来节约操作数。

```c
int mod7(int x) {
    int signfill = x >> 31, first, fill, fill2, sum1, sum2, sum3, sumtwice, sumthree, overflow;
    int minint = signfill & !((~x+1)^x);
    x += minint;
    x = ((x ^ signfill) + (signfill & 1));//get abs

    fill = 0x7 << 12 | 0x7; //0b000111 000000 000111
    first = (fill << 6) | (0x7 << 24) | fill;//0b000111 x 5
    sum1 = (x & first) + (x >> 3 & first) + (x >> 30);

    fill2 = 0x3F << 12 | 0x3F;
    sum2 = (sum1 & fill2) + (sum1 >> 6 & fill2) + (sum1 >> 24);

    sum3 = (sum2 & 0xFF) + (sum2 >> 12);//merge into one number

    sumtwice = (sum3 & 7) + (sum3 >> 3 & 7) + (sum3 >> 6);
    sumthree = (sumtwice & 7) + (sumtwice >> 3);

    overflow = (~(!(7 ^ sumthree))) + 1;
    sumthree = sumthree + (((~7) + 1)&overflow);

    return (sumthree ^ signfill) + (signfill & 1) + (~minint+1);
}
```
