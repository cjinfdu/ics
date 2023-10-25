---
title: ICS 2023
---

Fall, 2023

Instructor: [金城](https://cjinfdu.github.io/)

School of Computer Science, Fudan University

---


## News

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
| 13   | 10/26/2023 | CoroutineLab发布              | [Lab3-CoroutineLab](CoroutineLab) (Due: 11/15/2023) |
| 14   | 10/27/2023 |                               |                                                     |
| 15   | 11/02/2023 | BombLab讲解 & 第三章习题讲解  |                                                     |
| 16   | 11/03/2023 |                               |                                                     |
| 17   | 11/09/2023 | CacheLab发布 & 第四章习题讲解 |                                                     |
| 18   | 11/10/2023 |                               |                                                     |

---

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
