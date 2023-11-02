---
title: BombLab writeup
date: 2023-11-02 08:00:00
mathjax: true
---

## 题目解析

### Phase1（函数调用）

函数与汇编对应
看函数名猜功能
在命令行把 rdi、rsi 截下来
一个是你的输入，一个是答案
唯一答案是 `Computer science is not a boring subject`

### Phase2（循环）

函数与汇编对应
phase_2_nums 结构体，处于数据段，不是代码段 (-D)
该代码段中有对应的默认值

```
struct {
    int nums[6] = {2, 4, 8, 16, 32, 64};
    char is_pass_six = '\0'; // for secret phase
    int base = BASE_IN_2;
    int add_num = BASE_ADD_2;
} phase_2_nums;

0000000000406360 <phase_2_nums>:
  406360:	02 00                	add    (%rax),%al # num[0]
  406362:	00 00                	add    %al,(%rax)
  406364:	04 00                	add    $0x0,%al  # num[4]
  406366:	00 00                	add    %al,(%rax)
  406368:	08 00                	or     %al,(%rax)  # num[8]
  40636a:	00 00                	add    %al,(%rax)
  40636c:	10 00                	adc    %al,(%rax) # num[16]
  40636e:	00 00                	add    %al,(%rax)
  406370:	20 00                	and    %al,(%rax) # num[32]
  406372:	00 00                	add    %al,(%rax)
  406374:	40 00 00             	add    %al,(%rax) # num[64]
  406377:	00 00                	add    %al,(%rax)
  406379:	00 00                	add    %al,(%rax)
  40637b:	00 fc                	add    %bh,%ah   
  40637d:	ff                   	(bad)  
  40637e:	ff                   	(bad)  
  40637f:	ff 01                	incl   (%rcx) # fc到ff是BASE_IN_2(-4)的补码，01 是BASE_ADD_2的值
```

题目要求大家输入的每一个数字是前一个*(-4)+1, 即
$$answer_i = answer_{i-1}*(-4)+1,answer_0 != 0$$

一个参考答案是 `1 -3 13 -51 205 -819`

### Phase3（分支）

```asm
  401627:	48 89 7d d8          	mov    %rdi,-0x28(%rbp)
  40162b:	48 8d 75 ef          	lea    -0x11(%rbp),%rsi
  40162f:	48 8d 4d f0          	lea    -0x10(%rbp),%rcx
  401633:	48 8d 55 f4          	lea    -0xc(%rbp),%rdx
  401637:	48 8b 45 d8          	mov    -0x28(%rbp),%rax
  40163b:	49 89 f0             	mov    %rsi,%r8
  40163e:	48 8d 35 03 1c 00 00 	lea    0x1c03(%rip),%rsi        # 403248 <_ZNSt8__detailL19_S_invalid_state_idE+0x8>
  401645:	48 89 c7             	mov    %rax,%rdi
  401648:	b8 00 00 00 00       	mov    $0x0,%eax
  40164d:	e8 0e fb ff ff       	callq  401160 <__isoc99_sscanf@plt>
```

```
 403230 09000000 0a000000 0b000000 0c000000  ................
 403240 ffffffff ffffffff 25642025 64202563  ........%d %d %c
 403250 00000000 2ae5ffff 7ee4ffff 98e4ffff  ....*...~.......
```

函数开头调用了 `sscanf(input, "%d %d %c", -0xc(%rbp), -0x10(%rbp), -0x11(%rbp))`，从 input 字符串中读取了两个数字和一个字符。下记这三个变量为 vc, v10, v11。

在经过返回值检查后，进入了一大堆比较和跳转，根据 vc 的值跳转到不同的位置：

```asm
  401660:	8b 45 f4             	mov    -0xc(%rbp),%eax
  401663:	3d 62 02 00 00       	cmp    $0x262,%eax
  401668:	0f 84 fb 00 00 00    	je     401769 <phase_3+0x14e>
  40166e:	3d 62 02 00 00       	cmp    $0x262,%eax
  401673:	0f 8f 05 01 00 00    	jg     40177e <phase_3+0x163>   # boom (signed vc > 0x262)
  401679:	3d e9 00 00 00       	cmp    $0xe9,%eax
  40167e:	0f 84 d0 00 00 00    	je     401754 <phase_3+0x139>
  401684:	3d e9 00 00 00       	cmp    $0xe9,%eax
  401689:	0f 8f ef 00 00 00    	jg     40177e <phase_3+0x163>   # boom (signed vc > 0xe9)
  40168f:	83 f8 22             	cmp    $0x22,%eax
  401692:	7f 34                	jg     4016c8 <phase_3+0xad>
  401694:	85 c0                	test   %eax,%eax
  401696:	0f 8e e2 00 00 00    	jle    40177e <phase_3+0x163>   # boom (signed vc <= 0)
  40169c:	83 f8 22             	cmp    $0x22,%eax
  40169f:	0f 87 d9 00 00 00    	ja     40177e <phase_3+0x163>   # boom (unsigned vc > 0x22)
```

目前为止还很正常，但在确认 unsigned vc <= 0x22 之后就来了一些奇奇怪怪的代码：

```asm
  4016a5:	89 c0                	mov    %eax,%eax
  4016a7:	48 8d 14 85 00 00 00 	lea    0x0(,%rax,4),%rdx        # rdx = 4 * vc
  4016ae:	00 
  4016af:	48 8d 05 9e 1b 00 00 	lea    0x1b9e(%rip),%rax        # 403254
  4016b6:	8b 04 02             	mov    (%rdx,%rax,1),%eax       # eax = *(int*)(0x403254 + 4 * vc)
  # ((int*)0x403254)[vc]
  4016b9:	48 98                	cltq
  4016bb:	48 8d 15 92 1b 00 00 	lea    0x1b92(%rip),%rdx        # 403254
  4016c2:	48 01 d0             	add    %rdx,%rax                # rax = 0x403254 + ((int*)0x403254)[vc]
  4016c5:	3e ff e0             	notrack jmpq *%rax
```

```
 403250 00000000 2ae5ffff 7ee4ffff 98e4ffff  ....*...~.......
 403260 2ae5ffff 2ae5ffff b2e4ffff 2ae5ffff  *...*.......*...
 403270 2ae5ffff 2ae5ffff 2ae5ffff 2ae5ffff  *...*...*...*...
 403280 2ae5ffff 2ae5ffff c5e4ffff 2ae5ffff  *...*.......*...
 403290 2ae5ffff 2ae5ffff 2ae5ffff 2ae5ffff  *...*...*...*...
 4032a0 2ae5ffff 2ae5ffff 2ae5ffff 2ae5ffff  *...*...*...*...
 4032b0 2ae5ffff 2ae5ffff 2ae5ffff 2ae5ffff  *...*...*...*...
 4032c0 2ae5ffff 2ae5ffff 2ae5ffff 2ae5ffff  *...*...*...*...
 4032d0 2ae5ffff 2ae5ffff 2ae5ffff d8e4ffff  *...*...*.......
```

让我们假设 vc == 0x22，那么首先 `*(int*)(0x403254 + 4 * vc)` 就是 0xffffd8e4（注意小端法），经过符号拓展（cltq），此时 rax 的值为 0xffffffffffffd8e4，也就是补码表示的 -10012。我们将这个值加到 0x403254 上，可以得到 0x400B38，一个位于 phase_3 函数中的地址。

这里其实就是书本 3.6.8 中提到的 switch 的跳转表机制（P159）。0x403254 处存放的就是 `switch(vc)` 的**跳转表**，其中记录的偏移（比如 0xffffd8e4）加上跳转表自己的地址（0x403254）就可以得到跳转目标的地址。

但这里会产生一个疑惑：为什么对同一个变量 vc 的比较，一会采用跳转表，一会又采用直接比较的方式呢？

书上提到：“GCC 根据 switch 目标的数量和稀疏程度来翻译 switch 语句”，因此 GCC 这里使用了一种优化策略，将部分 switch 目标装载到跳转表中，部分差得过大的目标就用比较来实现分支跳转。试想如果把 0x262 也装入跳转表，那跳转表要特别特别大才行，这对于空间来说太浪费了。

总之，我们可以给每个 switch 跳转目标都打上注释：

```asm
  4016d2:	c6 45 ff 6f          	movb   $0x6f,-0x1(%rbp)         # vc == 0x1
  4016d6:	8b 45 f0             	mov    -0x10(%rbp),%eax
  4016d9:	83 f8 01             	cmp    $0x1,%eax
  4016dc:	0f 84 a3 00 00 00    	je     401785 <phase_3+0x16a>
  4016e2:	e8 61 09 00 00       	callq  402048 <explode_bomb>
  4016e7:	e9 99 00 00 00       	jmpq   401785 <phase_3+0x16a>
  4016ec:	c6 45 ff 76          	movb   $0x76,-0x1(%rbp)         # vc == 0x2
  4016f0:	8b 45 f0             	mov    -0x10(%rbp),%eax
  4016f3:	83 f8 03             	cmp    $0x3,%eax
  4016f6:	0f 84 8c 00 00 00    	je     401788 <phase_3+0x16d>
  4016fc:	e8 47 09 00 00       	callq  402048 <explode_bomb>
  401701:	e9 82 00 00 00       	jmpq   401788 <phase_3+0x16d>
  401706:	c6 45 ff 65          	movb   $0x65,-0x1(%rbp)         # vc == 0x5
  40170a:	8b 45 f0             	mov    -0x10(%rbp),%eax
  40170d:	83 f8 08             	cmp    $0x8,%eax
  401710:	74 79                	je     40178b <phase_3+0x170>
  401712:	e8 31 09 00 00       	callq  402048 <explode_bomb>
  401717:	eb 72                	jmp    40178b <phase_3+0x170>
  401719:	c6 45 ff 72          	movb   $0x72,-0x1(%rbp)         # vc == 0xd
  40171d:	8b 45 f0             	mov    -0x10(%rbp),%eax
  401720:	83 f8 15             	cmp    $0x15,%eax
  401723:	74 69                	je     40178e <phase_3+0x173>
  401725:	e8 1e 09 00 00       	callq  402048 <explode_bomb>
  40172a:	eb 62                	jmp    40178e <phase_3+0x173>
  40172c:	c6 45 ff 66          	movb   $0x66,-0x1(%rbp)         # vc == 0x22
  401730:	8b 45 f0             	mov    -0x10(%rbp),%eax
  401733:	83 f8 37             	cmp    $0x37,%eax
  401736:	74 59                	je     401791 <phase_3+0x176>
  401738:	e8 0b 09 00 00       	callq  402048 <explode_bomb>
  40173d:	eb 52                	jmp    401791 <phase_3+0x176>
  40173f:	c6 45 ff 6c          	movb   $0x6c,-0x1(%rbp)         # vc == 0x59
  401743:	8b 45 f0             	mov    -0x10(%rbp),%eax
  401746:	3d 90 00 00 00       	cmp    $0x90,%eax
  40174b:	74 47                	je     401794 <phase_3+0x179>
  40174d:	e8 f6 08 00 00       	callq  402048 <explode_bomb>
  401752:	eb 40                	jmp    401794 <phase_3+0x179>
  401754:	c6 45 ff 6f          	movb   $0x6f,-0x1(%rbp)         # vc == 0xe9
  401758:	8b 45 f0             	mov    -0x10(%rbp),%eax
  40175b:	3d 79 01 00 00       	cmp    $0x179,%eax
  401760:	74 35                	je     401797 <phase_3+0x17c>
  401762:	e8 e1 08 00 00       	callq  402048 <explode_bomb>
  401767:	eb 2e                	jmp    401797 <phase_3+0x17c>
  401769:	c6 45 ff 77          	movb   $0x77,-0x1(%rbp)         # vc == 0x262
  40176d:	8b 45 f0             	mov    -0x10(%rbp),%eax
  401770:	3d db 03 00 00       	cmp    $0x3db,%eax
  401775:	74 23                	je     40179a <phase_3+0x17f>
  401777:	e8 cc 08 00 00       	callq  402048 <explode_bomb>
  40177c:	eb 1c                	jmp    40179a <phase_3+0x17f>
  40177e:	e8 c5 08 00 00       	callq  402048 <explode_bomb>    # vc == 跳转表其他目标
```

在上面这些代码中，首先根据 vc 的值不同，为 `-0x1(%rbp)` 赋了值，这个变量我们记作 v1；然后对 v10 的值进行了不同的检测，总得来说需要满足这些条件其中之一：

- vc == 0x01 && v10 == 0x01
- vc == 0x02 && v10 == 0x03
- vc == 0x05 && v10 == 0x08
- vc == 0x0d && v10 == 0x15
- vc == 0x22 && v10 == 0x37
- vc == 0x59 && v10 == 0x90
- vc == 0xe9 && v10 == 0x179
- vc == 0x262 && v10 == 0x3db

然后，经过一些无意义的跳转，我们来到了 phase3 的最后一部分代码：

```asm
  40179b:	0f b6 45 ef          	movzbl -0x11(%rbp),%eax
  40179f:	38 45 ff             	cmp    %al,-0x1(%rbp)
  4017a2:	74 05                	je     4017a9 <phase_3+0x18e>
  4017a4:	e8 9f 08 00 00       	callq  402048 <explode_bomb>
  4017a9:	90                   	nop
  4017aa:	c9                   	leaveq 
  4017ab:	c3                   	retq   
```

这里检测了 v11 是否与 v1 相等，如果不相等的话就会爆炸。因此，我们可以扩充上面的表，得到所有可能的答案组合：

- vc == 0x01 && v10 == 0x01 && v11 == 0x6f
- vc == 0x02 && v10 == 0x03 && v11 == 0x76
- vc == 0x05 && v10 == 0x08 && v11 == 0x65
- vc == 0x0d && v10 == 0x15 && v11 == 0x72
- vc == 0x22 && v10 == 0x37 && v11 == 0x66
- vc == 0x59 && v10 == 0x90 && v11 == 0x6c
- vc == 0xe9 && v10 == 0x179 && v11 == 0x6f
- vc == 0x262 && v10 == 0x3db && v11 == 0x77

在文档中有提示说 secret_phase 的线索隐藏在 phase_3 中，其实这里的提示就是 v11 的取值。将这些十六进制数理解为 ASCII，就可以得到一个英文单词。

- vc == 0x01 && v10 == 0x01 && v11 == 'o'
- vc == 0x02 && v10 == 0x03 && v11 == 'v'
- vc == 0x05 && v10 == 0x08 && v11 == 'e'
- vc == 0x0d && v10 == 0x15 && v11 == 'r'
- vc == 0x22 && v10 == 0x37 && v11 == 'f'
- vc == 0x59 && v10 == 0x90 && v11 == 'l'
- vc == 0xe9 && v10 == 0x179 && v11 == 'o'
- vc == 0x262 && v10 == 0x3db && v11 == 'w'

由于汇编层面中没有类型信息，所以汇编语言不会主动告诉你哪些数据是字符，哪些数据是数字。因此，在进行逆向工程时，注意 0x20~0x7f 之间的数据，这个范围是可见字符的范围。

### Phase4（递归）

第四阶段是一个递归调用自身的函数，我们需要构造出特定的参数，使其返回特定的值。
我们首先分析 phase_4 函数：

```asm
  401800:	48 89 7d e8          	mov    %rdi,-0x18(%rbp)
  401804:	48 8d 55 f0          	lea    -0x10(%rbp),%rdx
  401808:	48 8b 45 e8          	mov    -0x18(%rbp),%rax
  40180c:	48 8d 35 cd 1a 00 00 	lea    0x1acd(%rip),%rsi        # 4032e0 <_ZNSt8__detailL19_S_invalid_state_idE+0xa0>
  401813:	48 89 c7             	mov    %rax,%rdi
  401816:	b8 00 00 00 00       	mov    $0x0,%eax
  40181b:	e8 40 f9 ff ff       	callq  401160 <__isoc99_sscanf@plt>
  401820:	83 f8 01             	cmp    $0x1,%eax
  401823:	0f 95 c0             	setne  %al
  401826:	84 c0                	test   %al,%al
  401828:	74 05                	je     40182f <phase_4+0x3b>
  40182a:	e8 19 08 00 00       	callq  402048 <explode_bomb>
```

```
 4032d0 2ae5ffff 2ae5ffff 2ae5ffff d8e4ffff  *...*...*.......
 4032e0 256c6c64 00257320 256400e6 9d80e69d  %lld.%s %d......
 4032f0 80e69d80 efbc8100 e98080e9 8080e980  ................
```

首先，函数读入一个长整型，并存放到 `-0x10(%rbp)` 中，我们将其记为 v10。然后函数进行了一些赋值，将这个长整型拆了开来：

```asm
  40182f:	48 8b 45 f0          	mov    -0x10(%rbp),%rax
  401833:	48 c1 f8 20          	sar    $0x20,%rax
  401837:	89 45 fc             	mov    %eax,-0x4(%rbp)    # v4 = signed v10 >> 32
  40183a:	48 8b 45 f0          	mov    -0x10(%rbp),%rax
  40183e:	89 45 f8             	mov    %eax,-0x8(%rbp)    # v8 = (int) v10
```

然后，函数进行了一系列的检查。

```asm
  401841:	83 7d fc 00          	cmpl   $0x0,-0x4(%rbp)
  401845:	0f 9e c2             	setle  %dl                # dl = v4 <= 0
  401848:	83 7d fc 0e          	cmpl   $0xe,-0x4(%rbp)
  40184c:	0f 9f c0             	setg   %al                # al = v4 >= 0xe
  40184f:	09 d0                	or     %edx,%eax
  401851:	0f b6 d0             	movzbl %al,%edx           # edx = (v4 <= 0) || (v4 >= 0xe)

  401854:	83 7d f8 00          	cmpl   $0x0,-0x8(%rbp)
  401858:	0f 9e c0             	setle  %al                # al = v8 <= 0
  40185b:	0f b6 c0             	movzbl %al,%eax
  40185e:	09 c2                	or     %eax,%edx          # edx = (v4 <= 0) || (v4 >= 0xe) || (v8 <= 0)
  401860:	83 7d f8 0e          	cmpl   $0xe,-0x8(%rbp)
  401864:	0f 9f c0             	setg   %al                # al = v8 > 0xe
  401867:	0f b6 c0             	movzbl %al,%eax
  40186a:	09 d0                	or     %edx,%eax          # eax = (v4 <= 0) || (v4 > 0xe) || (v8 <= 0) || (v8 > 0xe)
  40186c:	85 c0                	test   %eax,%eax
  40186e:	74 05                	je     401875 <phase_4+0x81>
  401870:	e8 d3 07 00 00       	callq  402048 <explode_bomb>  # if (eax) boom
```

检查需要 v4 和 v8 都是介于 $(0,Eh]$ 之间的数字。

然后我们来看末尾：

```asm
  401875:	8b 45 fc             	mov    -0x4(%rbp),%eax
  401878:	89 c7                	mov    %eax,%edi
  40187a:	e8 2d ff ff ff       	callq  4017ac <_ZL4hopei>
  40187f:	3d 00 00 00 01       	cmp    $0x1000000,%eax
  401884:	0f 95 c0             	setne  %al
  401887:	84 c0                	test   %al,%al
  401889:	74 05                	je     401890 <phase_4+0x9c>
  40188b:	e8 b8 07 00 00       	callq  402048 <explode_bomb>
  401890:	90                   	nop
  401891:	c9                   	leaveq 
  401892:	c3                   	retq   
```

我们需要让 `_ZL4hopei(v4)`（下称hope）返回 0x1000000 来通关本关。我们来看 hope 函数。

```asm
  4017b8:	89 7d ec             	mov    %edi,-0x14(%rbp)
  4017bb:	83 7d ec 00          	cmpl   $0x0,-0x14(%rbp)
  4017bf:	75 07                	jne    4017c8 <_ZL4hopei+0x1c>
  4017c1:	b8 01 00 00 00       	mov    $0x1,%eax
  4017c6:	eb 2a                	jmp    4017f2 <_ZL4hopei+0x46>
```

如果参数（下称 v14）为 0 的话，直接返回 1；否则继续处理。

```asm
  4017c8:	8b 45 ec             	mov    -0x14(%rbp),%eax
  4017cb:	d1 f8                	sar    %eax
  4017cd:	89 c7                	mov    %eax,%edi
  4017cf:	e8 d8 ff ff ff       	callq  4017ac <_ZL4hopei>
  4017d4:	89 45 fc             	mov    %eax,-0x4(%rbp)      # v4 = hope(n >> 1)
```

这里递归调用了自身，并把返回值存放到了 v4 中。

```asm
  4017d7:	8b 45 ec             	mov    -0x14(%rbp),%eax
  4017da:	83 e0 01             	and    $0x1,%eax
  4017dd:	85 c0                	test   %eax,%eax
  4017df:	74 0b                	je     4017ec <_ZL4hopei+0x40>

  4017e1:	8b 45 fc             	mov    -0x4(%rbp),%eax
  4017e4:	0f af c0             	imul   %eax,%eax
  4017e7:	c1 e0 02             	shl    $0x2,%eax
  4017ea:	eb 06                	jmp    4017f2 <_ZL4hopei+0x46>

  4017ec:	8b 45 fc             	mov    -0x4(%rbp),%eax
  4017ef:	0f af c0             	imul   %eax,%eax

  4017f2:	c9                   	leaveq 
  4017f3:	c3                   	retq   
```

这里如果 `v14 & 1 == 0`（v14 是偶数），那么会返回 v4^2，否则会返回 4 \* (v4^2)。

我们可以把这个函数翻译成 C 语言，来更好地理解它：

```c
int hope(int v14) {
    if(v14 == 0) return 1;
    
    int v4 = hope(v14 >> 1);
    
    if (v14 & 1)
        return v4 * v4 * 4;
    else
        return v4 * v4;
}
```

可以看到，这里的 v14 类似于一个 bit vector，每一位都会指使当前层递归如何处理下一层递归返回的数字，直到最后一次递归直接返回 1。

我们可以假设内层递归返回的数是 $2^k$，当前层可以选择返回 $2^{2k}$ 或者 $2^{2k+2}$。我们需要思考的问题是，如何从 1 变成 0x1000000，也就是从 $2^0$ 变成 $2^{24}$，同时需要满足这个 bit vector 的值不超过 0xe。

抽象出这个模型之后，我们很容易想到 k 可以这样变化：0->2->6->12->24，与之对应的 bit vector 也就是 0b1100，即 0xc。其他的路径也存在，但都超过了 0xe。

因此，这道题的答案就是 `(0xc << 0x20) || 一个∈(0, 0xe]的数字`，具体来说就是 $ans\in[51539607553,51539607566]$。

### Phase5（面向对象）

```asm
  4018a0:	48 89 7d b8          	mov    %rdi,-0x48(%rbp)
  4018a4:	48 8d 4d cc          	lea    -0x34(%rbp),%rcx
  4018a8:	48 8d 55 d0          	lea    -0x30(%rbp),%rdx
  4018ac:	48 8b 45 b8          	mov    -0x48(%rbp),%rax
  4018b0:	48 8d 35 2e 1a 00 00 	lea    0x1a2e(%rip),%rsi        # 4032e5
  4018b7:	48 89 c7             	mov    %rax,%rdi
  4018ba:	b8 00 00 00 00       	mov    $0x0,%eax
  4018bf:	e8 9c f8 ff ff       	callq  401160 <__isoc99_sscanf@plt>
  4018c4:	83 f8 02             	cmp    $0x2,%eax
  4018c7:	0f 95 c0             	setne  %al
  4018ca:	84 c0                	test   %al,%al
  4018cc:	74 05                	je     4018d3 <phase_5+0x40>
  4018ce:	e8 75 07 00 00       	callq  402048 <explode_bomb>
```

```
 4032d0 2ae5ffff 2ae5ffff 2ae5ffff d8e4ffff  *...*...*.......
 4032e0 256c6c64 00257320 256400e6 9d80e69d  %lld.%s %d......
 4032f0 80e69d80 efbc8100 e98080e9 8080e980  ................
```

我们可以看到，第五阶段会读取一个字符串和一个数字，并分别存放在 `-0x30(%rbp)` 和 `-0x34(%rbp)` 中，后面我们将其记为 buf 和 v34。

```asm
  4018d3:	48 8d 45 d0          	lea    -0x30(%rbp),%rax
  4018d7:	48 8d 35 0d 1a 00 00 	lea    0x1a0d(%rip),%rsi        # 4032eb
  4018de:	48 89 c7             	mov    %rax,%rdi
  4018e1:	e8 fa f8 ff ff       	callq  4011e0 <strcmp@plt>
  4018e6:	85 c0                	test   %eax,%eax
  4018e8:	75 1b                	jne    401905 <phase_5+0x72>
  4018ea:	bf 10 00 00 00       	mov    $0x10,%edi
  4018ef:	e8 8c f8 ff ff       	callq  401180 <_Znwm@plt>
  4018f4:	48 89 c3             	mov    %rax,%rbx
  4018f7:	48 89 df             	mov    %rbx,%rdi
  4018fa:	e8 c9 05 00 00       	callq  401ec8 <_ZN10worldline1C1Ev>
  4018ff:	48 89 5d e8          	mov    %rbx,-0x18(%rbp)
  401903:	eb 69                	jmp    40196e <phase_5+0xdb>
  401905:	48 8d 45 d0          	lea    -0x30(%rbp),%rax
  401909:	48 8d 35 e8 19 00 00 	lea    0x19e8(%rip),%rsi        # 4032f8
  401910:	48 89 c7             	mov    %rax,%rdi
  401913:	e8 c8 f8 ff ff       	callq  4011e0 <strcmp@plt>
  401918:	85 c0                	test   %eax,%eax
  40191a:	75 1b                	jne    401937 <phase_5+0xa4>
  40191c:	bf 10 00 00 00       	mov    $0x10,%edi
  401921:	e8 5a f8 ff ff       	callq  401180 <_Znwm@plt>
  401926:	48 89 c3             	mov    %rax,%rbx
  401929:	48 89 df             	mov    %rbx,%rdi
  40192c:	e8 17 06 00 00       	callq  401f48 <_ZN10worldline2C1Ev>
  401931:	48 89 5d e8          	mov    %rbx,-0x18(%rbp)
  401935:	eb 37                	jmp    40196e <phase_5+0xdb>
  401937:	48 8d 45 d0          	lea    -0x30(%rbp),%rax
  40193b:	48 8d 35 c3 19 00 00 	lea    0x19c3(%rip),%rsi        # 403305
  401942:	48 89 c7             	mov    %rax,%rdi
  401945:	e8 96 f8 ff ff       	callq  4011e0 <strcmp@plt>
  40194a:	85 c0                	test   %eax,%eax
  40194c:	75 1b                	jne    401969 <phase_5+0xd6>
  40194e:	bf 10 00 00 00       	mov    $0x10,%edi
  401953:	e8 28 f8 ff ff       	callq  401180 <_Znwm@plt>
  401958:	48 89 c3             	mov    %rax,%rbx
  40195b:	48 89 df             	mov    %rbx,%rdi
  40195e:	e8 65 06 00 00       	callq  401fc8 <_ZN10worldline3C1Ev>
  401963:	48 89 5d e8          	mov    %rbx,-0x18(%rbp)
  401967:	eb 05                	jmp    40196e <phase_5+0xdb>
```

这里是一个比较，会根据 buf 进行 strcmp 来进入不同的路径。
但我们直接看这个地址，会发现这是不可见字符，而且最高的那个 bit 是 1。这里我们可以通过使用 gdb 中的 `x/s` 指令来打印这几个字符串：

```gdb
pwndbg> x/s 0x4032eb
0x4032eb:       "杀杀杀！"
pwndbg> x/s 0x4032f8
0x4032f8:       "退退退。"
pwndbg> x/s 0x403305
0x403305:       "冲冲冲~"
```

在每一条路径中，都会首先调用 `_Znwm@plt(0x10)`，然后将其作为参数调用 `_ZN10worldline1C1Ev` 、`_ZN10worldline2C1Ev` 或者 `_ZN10worldline3C1Ev`，并将 rbx 中的值存到 `-0x18(%rbp)` 中（下称 v18）。之所以这里将 rbx 而不是 rax 的值存到栈上，是因为 rbx 属于 callee saved 寄存器，在调用前后一定是不变的.

如果你给 objdump 加上了 `-C` 参数，又或者在 GDB 中查看这几个函数的话，你看到的名字应该是 `<operator new(unsigned long)@plt>`、`<worldline1::worldline1()>`、`<worldline2::worldline2()>` 以及 `<worldline3::worldline3()>`。

> 不过我们发现很多同学没有用到 `-C` 参数，可能是因为我们的文档中还不够强调这一点，给大家先道个歉了（

结合一些 C++的知识，我们可以知道 new 是类似 malloc 的用来动态分配一些空间的运算符，而这几个与类同名的函数则是类的初始化函数。

因此，这里的代码用 C++ 表示就是：

```c
if (strcmp(buf, "杀杀杀！") == 0) {
    v18 = new worldline1();
} else if (strcmp(buf, "退退退。") == 0) {
    v18 = new worldline2();
} else if (strcmp(buf, "冲冲冲~") == 0) {
    v18 = new worldline3();
} else {
    explode_bomb();
}
```

我们先不跟进去看具体的类，继续看 phase_3，来掌握这一阶段的逻辑。

```asm
  40196e:	48 8b 45 e8          	mov    -0x18(%rbp),%rax
  401972:	48 8b 00             	mov    (%rax),%rax
  401975:	48 83 c0 10          	add    $0x10,%rax       # rax = *(uint64_t*)v18 + 0x10
  401979:	48 8b 08             	mov    (%rax),%rcx
  40197c:	8b 55 cc             	mov    -0x34(%rbp),%edx
  40197f:	48 8b 45 e8          	mov    -0x18(%rbp),%rax
  401983:	89 d6                	mov    %edx,%esi
  401985:	48 89 c7             	mov    %rax,%rdi
  401988:	ff d1                	callq  *%rcx
```

我们可以看到这里调用了某个函数，这个函数地址保存在 `*(uint64_t*)v18 + 0x10` 处；调用函数的参数是 v18 和 v34。

```asm
  40198a:	85 c0                	test   %eax,%eax
  40198c:	74 10                	je     40199e <phase_5+0x10b>
  40198e:	48 8b 45 e8          	mov    -0x18(%rbp),%rax
  401992:	48 89 c7             	mov    %rax,%rdi
  401995:	e8 0c 05 00 00       	callq  401ea6 <_ZN9worldline18is_phase5_passableEv>
  40199a:	85 c0                	test   %eax,%eax
  40199c:	75 07                	jne    4019a5 <phase_5+0x112>
  40199e:	b8 01 00 00 00       	mov    $0x1,%eax
  4019a3:	eb 05                	jmp    4019aa <phase_5+0x117>
  4019a5:	b8 00 00 00 00       	mov    $0x0,%eax
  4019aa:	84 c0                	test   %al,%al
  4019ac:	74 05                	je     4019b3 <phase_5+0x120>
  4019ae:	e8 95 06 00 00       	callq  402048 <explode_bomb>
```

我们仔细观察即可发现，如果上面这个未知的函数返回 0，炸弹就会爆炸。

函数然后调用了 `_ZN9worldline18is_phase5_passableEv()`（`worldline::is_phase5_passable()`），参数为 v18。如果这个函数返回 0，炸弹也会爆炸。

因此我们的目标有两个，一个是让未知的函数返回非 0 值，一个是让 `worldline::is_phase5_passable()` 返回非 0 值，这样就能够让 phase_5 函数正常返回。

接下来，我们会深入探究 C++面向对象机制，看看 class 和书上的 struct 到底有啥区别！

#### 面向对象机制的汇编实现

C++的 class 和 C 中的 struct，其最主要的区别就是类可以有成员函数。
一个最典型的成员函数，显然就是一个类的**构造函数**与**析构函数**。

我们首先来观察一下 `worldline1::worldline1()`，这里我们还是看加了 -C 参数得到的美化过的汇编代码。

```asm
0000000000401ec8 <worldline1::worldline1()>:
  401ec8:	f3 0f 1e fa          	endbr64 
  401ecc:	55                   	push   %rbp
  401ecd:	48 89 e5             	mov    %rsp,%rbp
  401ed0:	48 83 ec 10          	sub    $0x10,%rsp
  
Ⅰ 调用 worldline::worldline()
  401ed4:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
  401ed8:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
  401edc:	48 89 c7             	mov    %rax,%rdi
  401edf:	e8 a4 ff ff ff       	callq  401e88 <worldline::worldline()>
  
Ⅱ 将 vtable 存放在 class 实例的第一个八字节
  401ee4:	48 8d 15 85 3e 00 00 	lea    0x3e85(%rip),%rdx        # 405d70 <vtable for worldline1+0x10>
  401eeb:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
  401eef:	48 89 10             	mov    %rdx,(%rax)
  
Ⅲ 将 0x8b690 存放在 class 实例的第二个八字节
  401ef2:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
  401ef6:	48 c7 40 08 90 b6 08 	movq   $0x8b690,0x8(%rax)

  401efd:	00 
  401efe:	90                   	nop
  401eff:	c9                   	leaveq 
  401f00:	c3                   	retq   
  401f01:	90                   	nop
```

再来看看 `worldline::worldline()`，我们看名字可以发现这个函数同样是一个初始化函数：

```asm
0000000000401e88 <worldline::worldline()>:
  401e88:	f3 0f 1e fa          	endbr64 
  401e8c:	55                   	push   %rbp
  401e8d:	48 89 e5             	mov    %rsp,%rbp
  
  401e90:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
  401e94:	48 8d 15 fd 3e 00 00 	lea    0x3efd(%rip),%rdx        # 405d98 <__cxa_pure_virtual@CXXABI_1.3>
  401e9b:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
  401e9f:	48 89 10             	mov    %rdx,(%rax)
  
  401ea2:	90                   	nop
  401ea3:	5d                   	pop    %rbp
  401ea4:	c3                   	retq   
  401ea5:	90                   	nop
```

不过这个函数做的唯一的事就是将 0x405d98 存放到参数的第一个八字节中。我们尝试看看这里有什么：

```
Contents of section .data.rel.ro:
 405d10 00000000 00000000 b05d4000 00000000  .........]@.....
 405d20 02204000 00000000 16204000 00000000  . @...... @.....
 405d30 2a204000 00000000 00000000 00000000  * @.............
 405d40 c85d4000 00000000 821f4000 00000000  .]@.......@.....
 405d50 961f4000 00000000 aa1f4000 00000000  ..@.......@.....
 405d60 00000000 00000000 e05d4000 00000000  .........]@.....
 405d70 021f4000 00000000 161f4000 00000000  ..@.......@.....
 405d80 2a1f4000 00000000 00000000 00000000  *.@.............
 405d90 f85d4000 00000000 00000000 00000000  .]@.............
 405da0 00000000 00000000 00000000 00000000  ................
 405db0 00000000 00000000 20334000 00000000  ........ 3@.....
 405dc0 f85d4000 00000000 00000000 00000000  .]@.............
 405dd0 30334000 00000000 f85d4000 00000000  03@......]@.....
 405de0 00000000 00000000 40334000 00000000  ........@3@.....
 405df0 f85d4000 00000000 00000000 00000000  .]@.............
 405e00 50334000 00000000                    P3@.....    
```

虽然静态时只能看到一大堆 0，但是我们可以注意到这个 section 的名字中带有 rel 字样，在后面的章节中我们会学习动态链接机制，简单来说就是程序运行的时候才会将一些函数的地址填入到这里。我们可以动态调试看看这里的数据：

```gdb
pwndbg> x/8gx 0x405d98
0x405d98 <_ZTV9worldline+16>:   0x00007ffff7e80120      0x00007ffff7e80120
0x405da8 <_ZTV9worldline+32>:   0x00007ffff7e80120      0x00007ffff7fa8c98
0x405db8 <_ZTI10worldline3+8>:  0x0000000000403320      0x0000000000405df8
0x405dc8 <_ZTI10worldline2>:    0x00007ffff7fa8c98      0x0000000000403330

pwndbg> x/i 0x00007ffff7e80120
   0x7ffff7e80120 <__cxa_pure_virtual>: endbr64
```

我们可以看到，这里存放了一些函数地址，具体来说就是 `__cxa_pure_virtual`：

```gdb
   0x7ffff7e80120 <__cxa_pure_virtual>: endbr64
   0x7ffff7e80124 <__cxa_pure_virtual+4>:       push   rax
   0x7ffff7e80125 <__cxa_pure_virtual+5>:       pop    rax
   0x7ffff7e80126 <__cxa_pure_virtual+6>:       mov    edx,0x1b
   0x7ffff7e8012b <__cxa_pure_virtual+11>:      lea    rsi,[rip+0xdc5d2]        # 0x7ffff7f5c704
   0x7ffff7e80132 <__cxa_pure_virtual+18>:      mov    edi,0x2
   0x7ffff7e80137 <__cxa_pure_virtual+23>:      sub    rsp,0x8
   0x7ffff7e8013b <__cxa_pure_virtual+27>:      call   0x7ffff7e72ba0 <write@plt>
   0x7ffff7e80140 <__cxa_pure_virtual+32>:      call   0x7ffff7e6f160 <_ZSt9terminatev@plt>
```

可以看到这个函数不是什么好函数，它直接调用了 `_ZSt9terminatev@plt`，即 [std::terminate](https://en.cppreference.com/w/cpp/error/terminate) 函数。

但其实我们知道这些对解题并没有什么帮助，因为在 `worldline1:worldline1()` 中，这个表被另一个表（vtable for worldline1）覆盖了，我们来看看这个表又是什么情况：

```
 405d70 021f4000 00000000 161f4000 00000000  ..@.......@.....
 405d80 2a1f4000 00000000 00000000 00000000  *.@.............
```

可以看到，这里有三个函数地址，分别为 0x401f02，0x401f16 以及 0x401f2a，我们找到这几个函数，可以找到它们的定义：

```asm
0000000000401f02 <worldline1::isWorldPeace()>:

0000000000401f16 <worldline1::isEveryoneEqual()>:

0000000000401f2a <worldline1::dmail(int)>:
```

至此，讲义可以初步揭晓此前的疑惑了：vtable 是一个**函数表**，它的地址会被存放在对应类的第一个八字节。

之所以 worldline1 的构造函数中调用了 worldline 的构造函数，是因为这两个类是**基类**和**子类**的关系。在 C++中，子类构造函数会先调用其基类的构造函数，然后再进行自己的构造函数，这就是这种机制的汇编实现。

之所以 worldline（基类）的 vtable 中的三个函数都是 terminate 函数，是因为基类将这几个函数声明为了**纯虚函数**，等待子类（worldline123）去实现。

因此，我们可以按照类似的流程找到 worldline2 和 worldline3 的 vtable，其中存放的就是 worldline2 类和 worldline3 类对同样的三个函数 `isWorldPeace()` `isEveryoneEqual()` `dmail()` 的实现。

于是，我们可以发现，phase_5 中调用的未知函数，其实是在类的 vtable 中根据偏移找到对于虚函数 `dmail(int)` 的实现，也就是 `worldline1::dmail(int)` 、`worldline2::dmail(int)` 或者 `worldline3::dmail(int)`。

我们分别查看这三个函数，可以发现：

-  `worldline1::dmail(int)` 对应参数 0x7e2（2018）
-  `worldline2::dmail(int)` 对应参数 0x7e5（2021）
-  `worldline3::dmail(int)` 对应参数 0x7e7（2023）

然后我们查看 is_phase5_passable 函数。

```asm
0000000000401ea6 <worldline::is_phase5_passable()>:
  401ea6:	f3 0f 1e fa          	endbr64 
  401eaa:	55                   	push   %rbp
  401eab:	48 89 e5             	mov    %rsp,%rbp
  401eae:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
  401eb2:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
  401eb6:	48 8b 40 08          	mov    0x8(%rax),%rax
  401eba:	48 3d 3f 42 0f 00    	cmp    $0xf423f,%rax
  401ec0:	0f 9f c0             	setg   %al
  401ec3:	0f b6 c0             	movzbl %al,%eax
  401ec6:	5d                   	pop    %rbp
  401ec7:	c3                   	retq   
```

只有当传入指针的第二个八字节大于 0xf423f（999999）时，才能让这个函数返回非零值。

**一个类实例在虚函数表指针之后，就会顺序存放其各个成员函数域。**
我们知道传入这个函数的指针是一个类实例的指针，因此这里我们可以知道它的第二个八字节是一个成员变量。其初始化可以在各子类的初始化函数找到：

- `worldline1:worldline1()` 对应 0x8b690（571024）
- `worldline2:worldline2()` 对应 0x6f8d2（456914）
- `worldline3:worldline3()` 对应 0x1124fd（1123581）

只有第三条世界线的这一成员变量跨越了 1000000 的界限，达到了通过的条件。

因此，本关需要达到第三条世界线，答案为 `冲冲冲~ 2023`。

最后，介绍一下本关所涉及的面向对象机制：

- 一个类的内存结构由 vtable+成员变量构成。
- 非虚函数的成员函数（在本关中，`worldline::is_phase5_passable()` 就是一个非虚函数的成员函数，直接被子类继承了过去）和普通函数类似，但第一个参数一定是 this 指针。
- 一个虚函数就对应一个 vtable 中的偏移，程序会通过偏移去查找一个类实例的对应虚函数。
- 纯虚函数在 vtable 中被一个库函数代替（用来报错），否则就会指向具体的实现函数。
- 子类继承父类后，构造时会首先调用父类的构造函数，然后再进行自己的构造，比如把 vtable 替换为自己的 vtable。

> 更详细的机制可以阅读这个系列文章：[从汇编角度学习C/C++](https://bbs.kanxue.com/homepage-category-835440-293.htm)

### Phase6（循环队列）

phase_6 主要考察大家一个数据结构，考虑可能部分同学还没有学到树形结构，于是出了一个循环队列

```asm
  401ac2:   e8 53 ff ff ff       callq  401a1a <build_queue>  
  401ac7:   48 89 45 b8          mov    %rax,-0x48(%rbp)  
  401acb:   48 8b 45 b8          mov    -0x48(%rbp),%rax  
  401acf:   48 89 45 90          mov    %rax,-0x70(%rbp)
```

这段代码调用了一个`build_queue`函数，可以猜测其函数是生成了一个队列，结合循环队列的标题，可以猜测这个队列是循环队列。其返回值是0x406400,我们暂时不知道是什么，但是如果去检查`x /3wx 4219904`这个地址，可以发现一个变量名

`0x406400 <initialNodes+64>: 0x00000014 0x00000000 0x004063c0` 是一个结构体，如果这里同学们没有看出来，再后来`getval`函数前检查传入参数也可以看出来，第一项是储存的值0x00000014 = 20，最后一项是下一个node的地址`0x004063c0`，可以通过不断的迭代查找到整个队列的内容

initialnode 读取`build_target()`里面的`rbp - 0x70`的内容，或根据`build_queue()`来找到链表的地址

|406400|4063c0|406410|4063e0|4063d0|4063f0|
|---|---|---|---|---|---|
|20|60|40|50|10|30|
|->|->|->|->|->|->|

找到三个循环

```asm
  401ae6:   0f 8f 65 01 00 00    jg     401c51 <build_target+0x1bb>  
  401aec:   48 89 e0             mov    %rsp,%rax  
  401aef:   48 89 c3             mov    %rax,%rbx  
  401af2:   8b 45 c4             mov    -0x3c(%rbp),%eax
```

第一个循环，最外层的循环，可以找到跳转地址就是比较答案的地址

循环里面内嵌了两个循环，两个循环分别调用`get_val`和`put_val`函数 我们通过检查函数传参和返回的结果来找到其作用

一个`put_val`的函数的例子

```asm
  4019c3:   48 89 7d f8          mov    %rdi,-0x8(%rbp)  
  4019c7:   89 75 f4             mov    %esi,-0xc(%rbp)  
  4019ca:   48 8b 45 f8          mov    -0x8(%rbp),%rax  
  4019ce:   48 8b 00             mov    (%rax),%rax     
  4019d1:   8b 55 f4             mov    -0xc(%rbp),%edx   #将第二个参数的值赋到第一个参数的地址  
  4019d4:   89 10                mov    %edx,(%rax)    
```

输入 5 getval 5次 每次传入的地址分别是 406400，4063c0，406410，4063e0，4063d0 每次返回的值分别是 20，60，40，50，10

```asm
401bf0: 89 04 8a             mov    %eax,(%rdx,%rcx,4) #取出的数字会保存到一个数组
```

每次循环会保存取出来的数字

```asm
  401c06:   8b 14 90             mov    (%rax,%rdx,4),%edx  
  401c09:   8b 45 c4             mov    -0x3c(%rbp),%eax  
  401c0c:   48 98                cltq     
  401c0e:   89 94 85 70 ff ff ff mov    %edx,-0x90(%rbp,%rax,4)  
  401c15:   8b 45 b4             mov    -0x4c(%rbp),%eax  
  401c18:   83 e8 01             sub    $0x1,%eax  
  401c1b:   89 45 cc             mov    %eax,-0x34(%rbp)
```

我们在这里把数组中最后一个数字保存，这里没找出来其实不要紧，在最后面构造的答案里可以检查到

putval 5次 每次传入的地址分别是 406400，4063c0，406410，4063e0，4063d0 每次传入的值分别是 10，50，40，60，20

|406400|4063c0|406410|4063e0|4063d0|4063f0|
|---|---|---|---|---|---|
|10|50|40|60|20|30|
|->|->|->|->|->|->|

输入 6 getval 6次 每次传入的地址分别是 4063f0，406400，4063c0，406410，4063e0，4063d0 每次返回的值分别是 30，10，50，40，60，20

putval 6次 每次传入的地址分别是 4063f0，406400，4063c0，406410，4063e0，4063d0 每次传入的值分别是 20，60，40，50，10，30

|406400|4063c0|406410|4063e0|4063d0|4063f0|
|---|---|---|---|---|---|
|30|10|60|40|06|20|
|->|->|->|->|->|->|

------->找规律 对于输入的每个数，从循环队列中取对应数字的值，然后倒过来传入循环队列 答案数组保存取出的最后一个数字

------->暴力破解 bash、c++、python

最后记录的数组要求不严格递增,答案比较多 标准答案最后的数组是严格递增的`5 6 6 4 4 6`

### Secret Phase（溢出 & 花指令）

#### 溢出

我们观察 `main.cpp`，很容易发现，我们根本没有办法进入 serect_phase，因为 secret_key 作为一个 main 函数中的局部变量，根本没有被其他函数引用的可能性。自始自终，它就是 0 而已。

但是，相信各位有着丰富写 bug 经验的程序员们，一定对各种 **溢出（overflow）** 不会陌生。最常见的溢出就是数组下标多了一个、少了一个；又或者你使用了不安全的 `gets()` 或 `scanf("%s")` 等等……这些都会导致溢出。

在 bomb 中，更准确地说就在 `main.cpp` 的代码最前面，我故意构造了一个溢出的漏洞：

```cpp
void read_line(char* input)
{
    char ch;
    int i;
    for (i = 0; i <= 40; i++) {
        ch = getchar();
        if (ch == '\n') {
            input[i] = '\0';
            return;
        } else {
            input[i] = ch;
        }
    }
    input[i] = '\0';

    while (getchar() != '\n'); // clear the input buffer
}
```

乍一眼一看，这函数并没有什么特别的，但是我们仔细观察循环条件，可以发现**这个循环将会读取 40+1 个字符**。而我们使用这个函数读取输入时，**使用的 buffer（在 main 函数中）却是 40 字节的**。

多读的那个字符会去哪里呢？光看代码我们是无法知道编译器如何安排内存的，我们可以观察 main 函数的汇编代码：

```asm
  4013af:	48 8d 45 d0          	lea    -0x30(%rbp),%rax
  4013b3:	48 89 c7             	mov    %rax,%rdi
  4013b6:	e8 5b ff ff ff       	callq  401316 <read_line>
```

这段代码可以说明 buffer 的位置就在 `-0x30(%rbp)`。

```asm
  401487:	48 83 7d f8 00       	cmpq   $0x0,-0x8(%rbp)
  40148c:	74 4d                	je     4014db <main+0x14c>
  40148e:	48 8d 3d 8b 1c 00 00 	lea    0x1c8b(%rip),%rdi        # 403120 <_IO_stdin_used+0x120>
  401495:	e8 40 0c 00 00       	callq  4020da <slow_put>
  ...
```

这段代码对应进入 secret_phase 之前的检查，可以发现 secret_key 的位置就在 `-0x8(%rbp)`。

这两个变量之间差了 0x28，也就是 40 个字节。因此，**我们溢出的那一个字节会溢出到 secret_key 中**，让 secret_key 不再为 0。

因此，我们在任意一个阶段输入一个超级长的字符串（满 41 字节即可），就能够溢出一个字节到 secret_key 中，从而欺骗这个世界，进入本不可能进入的 secret_phase 中。

> 为了 Lab 的趣味性，请大家不要给认识的学弟/妹剧透本阶段！⊂彡☆))∀\`)

#### 花指令

接下来让我们进入 secret_phase 函数体。

```asm
0000000000401db5 <secret_phase>:
  401db5:	f3 0f 1e fa          	endbr64 
  401db9:	55                   	push   %rbp
  401dba:	48 89 e5             	mov    %rsp,%rbp
  401dbd:	48 83 ec 20          	sub    $0x20,%rsp
  401dc1:	48 89 7d e8          	mov    %rdi,-0x18(%rbp)
  401dc5:	eb ff                	jmp    401dc6 <secret_phase+0x11>
  401dc7:	c0 48 8d 3d          	rorb   $0x3d,-0x73(%rax)
  401dcb:	45 15 00 00 e8 2c    	rex.RB adc $0x2ce80000,%eax
  401dd1:	f4                   	hlt    
  401dd2:	ff                   	(bad)  
```

进入函数的前六句汇编一切正常，但第七句突然出现了我们没有学过的 `rorb` 指令，甚至后面出现了 `hlt` 和 `(bad)` ，难道 secret_phase 是一个恶意的程序，想要让我们的 CPU 停止运行吗？

但我们发现，这些指令又不会运行，因为 0x401dc5 处进行了一次跳转，跳到了 0x401dc6 处。但问题又来了，0x401dc6 是 jmp 指令的中间，我们用 objdump 拿到的汇编代码中并没有这一条。

废话少说，让我们使用 gdb 来调试看看：

如果你为 gdb 安装了 pwndbg 插件，那么你可以直接看到代码的执行流是这样的：

```gdb
 ► 0x401db5 <secret_phase>       endbr64
   0x401db9 <secret_phase+4>     push   rbp
   0x401dba <secret_phase+5>     mov    rbp, rsp
   0x401dbd <secret_phase+8>     sub    rsp, 0x20
   0x401dc1 <secret_phase+12>    mov    qword ptr [rbp - 0x18], rdi
   0x401dc5 <secret_phase+16>    jmp    secret_phase+17
    ↓
   0x401dc6 <secret_phase+17>    inc    eax
   0x401dc8 <secret_phase+19>    lea    rdi, [rip + 0x1545]
   0x401dcf <secret_phase+26>    call   puts@plt
```

如果你没有安装插件，也可以借助 `display/5i $rip` 的力量，来观察这一个奇怪的跳转。

其实，这是一种让反汇编代码失效的技术，即一种对抗逆向工程的技术，被称为花指令（在国外叫做 junk code）。

位于 0x401dc5 处的  `eb ff` 是一条合法的指令 `jmp 0x401dc6`。
位于 0x401dc6 处的  `ff c0` 同样是一条合法的指令 `inc eax`。

我们可以看出，x86 家族的跳转非常灵活，甚至可以跳到一条指令的中间——**指令对于 CPU 来说也只是内存中的数据而已，和别的内存数据并没有什么不同**。

这里的花指令，我是借用内联汇编机制，手动声明字节来完成的：

```c
    /* Junk Instructions
     *  ebff -> jmp -1
     *  ffc0 -> inc eax */
    asm(".byte 0xeb, 0xff, 0xc0" ::: "%eax");
```

在此之后就是正常的逻辑了，程序会读取一个 unsigned int 并与 0xdeadc0de 异或，如果结果不是 0xbaadf00d 就会触发炸弹爆炸。

因此，本关卡的答案就是 0xdeadc0de XOR 0xbaadf00d == 1677734099

> P.S. 由于出题时间较短，本阶段大部分汇编代码实际上还是可以在 objdump 的反汇编中直接看到(´_ゝ\`)

## 非标准解法

如果说标准解法是通过静态分析和动态调试，完全理解程序的逻辑后，再推算构造出满足条件的输入的话；那么非标准解法就是指不完全理解程序逻辑，同样可以找到满足条件的输入。

### 爆破

确实是对本次 BombLab 非常有效的方法，也是在逆向工程中非常喜闻乐见的方法！

> 炸弹就是要用爆破来进行硬碰硬，看看是你爆还是我爆！

我们这里不放爆破脚本了，就简单介绍一些思路：

- 在 Phase4 中，在阅读完 `phase_4()` 函数的代码后，答案空间已经缩小到了 0xe，可以直接进行爆破。
- Phase6 同理，在阅读函数时，你会不断得到一些限制条件，然后就可以进行爆破！

### 符号执行

python 有一个可以求解方程组的库（准确地说叫做约束求解器），叫做 `z3`。

我们可以思考一下，我们做 BombLab 的时候，其实就是从汇编代码中解读出一系列对于输入字符串的**限制**，然后根据这些限制来求解出我们应该输入什么字符串的过程。我们做的事情不就是人脑约束求解吗？

**这个过程可以让程序来完成！** 程序能够读取 bomb，将用到的变量（比如我们的输入）标记为**符号**，在运行过程中，根据我们所加的一些限制条件（比如不能走到 explode_bomb）计算出这些符号所应该满足的约束。这种技术就叫做**符号执行**。

最常用的符号执行工具之一，就是 [angr](https://angr.io/)。它的约束求解功能就是基于 `z3`。

我们这里就简单地使用 phase_4 为例，给大家演示一下流程：

```python
import angr

# 加载 bomb 文件
p = angr.Project("./bomb")

# 配置初始状态
phase4_addr = 0x4017f4
initial_state = p.factory.blank_state(addr=phase4_addr)
# 随便找个地址，构造一个字符串“符号”，作为phase_4的参数
input_addr = 0x114514
phase4_input = initial_state.solver.BVS('phase4_input', 40 * 8)
initial_state.memory.store(input_addr, phase4_input)
initial_state.regs.rdi = input_addr

# 启动模拟
explode_addr = 0x402048
target_addr = 0x401891  # phase4的leave;ret;所在的地址
simulation = p.factory.simgr(initial_state)
simulation.explore(find=target_addr, avoid=explode_addr)

# 打印结果
if simulation.found:
    solution_state = simulation.found[0]
    solution = solution_state.solver.eval(phase4_input, cast_to=bytes)
    print(solution)
else:
    print("Oh...")
```

安装完 angr 后启动这个脚本，等待几分钟，本关的答案就会被程序自动求出！（尽管这个答案看上去有些奇怪，但确实是能通过 phase4 的*一种*答案）

```sh
$ python se.py
...
b'51539607565h2\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
```

我们可以看到这个 python 脚本没有任何内容涉及到 phase_4 函数内部的逻辑，完全是程序自动根据我们设置的成功和避免条件来完成求解的。~~（因此某些逆向手称之为 angr 一把梭）~~

其他的 phase 也可以用符号执行来进行求解，但需要具体问题具体分析：如果某个阶段中存在的分支实在太多，那么会出现**状态爆炸**的问题，需要我们通过一些技巧来避免。没错，符号执行中使用了类似于*状态机*的模型，每次遇到一个分支就会分出两个状态，并为这两个状态分别添加一个不同的约束。

感兴趣的同学可以阅读 [Breaking CMU's Bomblab with Angr for Fun and Profit - Part 1 | Fan Pu Zeng](https://fanpu.io/blog/2020/breaking-cmu-bomblab-with-angr-for-fun-and-profit-part-1/)，来看看这位仁兄是如何用 angr 来一把梭原版 BombLab 的。
如果你想要入门 angr ，可以阅读官方文档中的入门部分：[Core Concepts - angr documentation](https://docs.angr.io/en/latest/core-concepts/toplevel.html)。
