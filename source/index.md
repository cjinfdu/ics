---
title: ICS 2023
---

Fall, 2023

Instructor: [金城](https://cjinfdu.github.io/)

School of Computer Science, Fudan University

---


## News

- **[Nov 23, 2023]** [Final Project](PJ) is pre-released.
- **[Nov 23, 2023]** [Lab5-MakeLab](MakeLab) is released. 
- **[Nov 09, 2023]** [Lab4-CacheLab](CacheLab) is released. 
- **[Nov 02, 2023]** [Lab2-BombLab writeup](BombLab-Comment) is released.
- **[Oct 26, 2023]** [Lab3-CoroutineLab](CoroutineLab) is released. 
- **[Oct 12, 2023]** [Lab1-DataLab writeup](datalab-comment) is released.
- **[Oct 07, 2023]** [Lab2-BombLab](BombLab) is released. 
- **[Sep 14, 2023]** [Lab1-DataLab](DataLab) is released. 
- **[Sep 08, 2023]** [ICS实验入门手册](ICS实验入门手册) is released.
- **[Sep 07, 2023]** Course Website is online.

## Schedule

| Num  | Date       | Subject                       | Assignment                                          |
| :--- | :--------- | :---------------------------- | :-------------------------------------------------- |
| 01   | 09/08/2023 | 数的表示 I                    |                                                     |
| 02   | 09/14/2023 | 环境配置 & DataLab发布        | [Lab1-DataLab](DataLab) (Due: 10/11/2023)           |
| 03   | 09/15/2023 | 数的表示 II                   |                                                     |
| 04   | 09/21/2023 | DataLab答疑                   |                                                     |
| 05   | 09/22/2023 | 机器码 I                      |                                                     |
| 06   | 09/28/2023 | 第二章习题讲解                | [第二章习题讲义](Assignment1)                       |
| 07   | 10/07/2023 | BombLab发布                   | [Lab2-BombLab](BombLab) (Due: 11/01/2023)           |
| 08   | 10/08/2023 | 机器码 II                     |                                                     |
| 09   | 10/12/2023 | DataLab讲解                   | [Lab1-DataLab writeup](datalab-comment)             |
| 10   | 10/13/2023 | 机器码 III                    |                                                     |
| 11   | 10/19/2023 | BombLab答疑                   |                                                     |
| 12   | 10/20/2023 | 机器码 IV                     |                                                     |
| 13   | 10/26/2023 | CoroutineLab发布              | [Lab3-CoroutineLab](CoroutineLab) (Due: 12/06/2023) |
| 14   | 10/27/2023 | 处理器 I                      |                                                     |
| 15   | 11/02/2023 | BombLab讲解                   | [Lab2-BombLab writeup](BombLab-Comment)             |
| 16   | 11/03/2023 | 处理器 II                     |                                                     |
| 17   | 11/09/2023 | CacheLab发布                  | [Lab4-CacheLab](CacheLab) (Due: 12/13/2023)         |
|      |            | 第三、四章习题讲解            | [第三章习题讲义](Assignment2)                       |
| 18   | 11/10/2023 | 优化 I                        |                                                     |
| 19   | 11/16/2023 | CoroutineLab答疑              |                                                     |
| 20   | 11/17/2023 | 优化 II                       |                                                     |
| 21   | 11/23/2023 | MakeLab发布                   | [Lab5-MakeLab](MakeLab) (Due: 12/13/2023)           |
|      |            | Final Project发布             | [Final Project](PJ) (Due: 12/20/2023)               |
| 22   | 11/24/2023 | 存储器 I                      |                                                     |
| 23   | 11/30/2023 | 第五章习题讲解                | [第五章习题讲义](Assignment5)                       |
| 24   | 12/01/2023 | 存储器 II                     |                                                     |
| 25   | 12/07/2023 | CoroutineLab讲解              |                                                     |
| 26   | 12/08/2023 | 链接 I                        |                                                     |
| 27   | 12/14/2023 | MakeLab讲解                   |                                                     |
|      |            | CacheLab讲解                  |                                                     |
|      |            | 第七章习题讲解                |                                                     |
| 28   | 12/15/2023 | 异常控制流 I                  |                                                     |
| 29   | 12/21/2023 | 第八章习题讲解                |                                                     |
|      |            | Final Project Pre             |                                                     |
| 30   | 12/22/2023 | 异常控制流 II                 |                                                     |

---

```cpp
#include <stdio.h>

void trans(int M, int N, int A[M][N], int B[N][M])
{
    int i, j, tmp;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

static int A[256][256];
static int B[256][256];

int main()
{
    printf("try it, and wait for a year\n");
    trans(256, 256, A, B);
}
```

<!--
```cpp
#include <stdio.h>

#include "generator.h"

int main() {
    auto coro = []() -> libco::generator<const char> {
        const char str[] = "Hello, ICS 2023!\n";
        for (const auto ch : str) {
            co_yield ch;
        }
    }();

    for (const auto ch : coro) {
        std::cout << ch;
    }
}
```
-->
