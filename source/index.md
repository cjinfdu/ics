---
title: ICS 2023
---

Fall, 2023
Instructor: [金城](https://cjinfdu.github.io/)
School of Computer Science, Fudan University


---


## News

- **[Sep 14, 2023]** [Lab1-DataLab](DataLab) is released. 
- **[Sep 08, 2023]** [ICS实验入门手册](ICS实验入门手册) is released.
- **[Sep 07, 2023]** Course Website is online.

## Schedule

|Num|Date      |Subject                |Assignment                                          |
|:--|:---------|:----------------------|:---------------------------------------------------|
|01 |09/08/2023|数的表示 I             |                                                    |
|02 |09/14/2023|环境配置 & DataLab发布 |[Lab1-DataLab](DataLab) (Due: 10/11/2023)           |
|03 |09/15/2023|数的表示 II            |                                                    |
|04 |09/21/2023|DataLab答疑            |                                                    |
|05 |09/22/2023|机器码 I               |                                                    |


---

```c
#include <stdio.h>
int main()
{
  unsigned version = 0x44fce000; // play --> https://www.h-schmidt.net/FloatConverter/IEEE754.html
  printf("Hello, ICS %f!\n", *(float*)&version);
}
```
