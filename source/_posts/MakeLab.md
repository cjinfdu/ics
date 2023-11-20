---
title: MakeLab
date: 2023-11-23 08:00:00
---

> Deadline: 2023-12-13 23:59:59

## 实验简介

`Fudan ICS` 原创课程项目，对应 `CSAPP` 中的链接单元，旨在通过 `Makefile` 帮助同学们了解编译与链接的过程。

### 下载

下载链接：[makelab-handout.tar](makelab-handout.tar)

```
makelab-handout/
├── task0   // 15%
├── task1   // 25%
├── task2   // 25%
├── task3   // 25%
├── task4   // 5%   H
├── task5   // 5%   H
└── testAll.sh
```

### 运行

在所有 `taskAll` 文件夹以外有一个测试脚本，使用它运行测试：

```bash
sh test.sh
```

### 提交

提交内容：包含 `report.pdf` 和 `makelab-handout` 的 `<your_ID>.tar`（< 3 MB）

实验报告要求：

1. 包含`sh test.sh` 的输出截图
2. 包含每个 `task` 的解决方案与附带问题的回答
3. 内容简洁，条理清晰

## 什么是 make 和 Makefile

> make 是一个在软件开发中所使用的工具程序（Utility software），经由读取“makefile”的文件以自动化建构软件。它是一种转化文件形式的工具，转换的目标称为“target”；与此同时，它也检查文件的依赖关系，如果需要的话，它会调用一些外部软件来完成任务。它的依赖关系检查系统非常简单，主要根据依赖文件的修改时间进行判断。大多数情况下，它被用来编译源代码，生成结果代码，然后把结果代码连接起来生成可执行文件或者库文件。它使用叫做“makefile”的文件来确定一个 target 文件的依赖关系，然后把生成这个 target 的相关命令传给 shell 去执行。
>
> 许多现代软件的开发中（如 Microsoft Visual Studio），集成开发环境已经取代 make，但是在 Unix 环境中，仍然有许多工程师采用 make 来协助软件开发。
>
>（from [Wikipedia](https://zh.wikipedia.org/wiki/Make))

`Makefile` 包含了一系列的规则和命令。

在含有 `Makefile` 的文件夹内，执行如下命令来使用 `Makefile` 内部的规则和指令：

```bash
make <target>
```

其中，`<target>` 表示在 `Makefile` 中定义的数个目标之一。

定义的格式如下：

```makefile
<target> ...: <prerequisites> ...
    command
    ...
```

例如：

```makefile
main: main.cpp
    g++ main.cpp -o main
```

在上述 `Makefile` 所在的目录在执行 `make main` 命令时，由于存在 `<prerequisites>`，`make` 程序将先检查 `<prerequisites>`（`main.cpp`）是否有更新。

此外，如果 `<prerequisites>` 中存在其他 `<target>`，则递归检查其 `<prerequisites>` 是否有更新。

如果所有 `<prerequisites>` 都没有更新，则本次执行不进行操作，如果 `<prerequisites>` 中其他 `<target>`有更新，则先执行有更新的 `<target>` 下的命令。

最后，`main` 下的命令将被执行。

注意，命令使用缩进与 `Makefile` 的指令进行区分，带有缩进的行最终是由 `shell` 来执行的，`make` 内部执行的指令在 `Makefile` 中是没有缩进的。

命令不限于编译命令，任何能够在 `shell` 中执行的命令都可以执行。

想要深入了解 `Makefile` 的同学可以阅读[“跟我一起写Makefile”](https://seisman.github.io/how-to-write-makefile/overview.html)，或自行查阅 `Makefile` 的官方标准文档。

## Task 0 - Warm Up

### 知识

编译 `C++` 程序一般使用 `g++` 命令。

如下的命令能够将 `main.cpp` 编译为可执行文件 `main`：

```bash
g++ main.cpp -o main
```

### TODO（15%）

```
task0
├── Makefile    // TODO
├── main.cpp    // TODO
├── some.cpp
└── some.h
```

修改 `main.cpp` 中的错误代码，在 `Makefile` 中编写编译命令，使用如下命令编译并运行程序：

```bash
make clean
make main
./main
make clean
```

## Task 1 - DEBUG

### 知识

在 `Makefile` 中，使用如下格式可以定义变量：

```makefile
<var name> = <value>
```

所有的变量值都是字符串值，使用 `$(<var name>)` 可以展开变量值到变量所处位置，示例如下：

```makefile
foo = c
prog.o: prog.$(foo)
    g$(foo)$(foo) -$(foo) prog.$(foo)
```

等价于：

```makefile
prog.o: prog.c
    gcc -c prog.c
```

此外，在 `Makefile` 中，可以使用如下命令字符串之间相等的条件判断：

```bash
ifeq (<string 1>,<string 2>)
    [commands]
else
    [commands]
endif
```

最后，在 `make` 运行时，变量可以通过如下格式进行值的指定：

```bash
make <target> <var name>=<value>
```

而在 `g++` 中，使用如下命令相当于在源代码中 `#define <var name>`：

```bash
g++ <source> -D<var name>
```

例如，使用如下命令相当于在源代码中 `#define DEBUG`：

```bash
g++ main.cpp -DDEBUG
```

### TODO（20%）

```
task1
├── Makefile    // TODO
├── function0.cpp
├── function0.h
├── function1.cpp
├── function1.h
├── main0.cpp
├── main1.cpp
├── shared.cpp
└── shared.h
```

在 `Makefile` 中，包含了两个编译目标：以 `main0.cpp` 为入口文件的 `main0` 和以 `main1.cpp` 为入口文件的 `main1`。

要求 `main1` 目标能够指定变量 `debug` 为 `True` 或者 `False`，来开启调试开关。

在 `Makefile` 中编写编译命令，使用如下命令编译并运行程序：

```bash
make clean
make main0
./main0
make main1
./main1
make clean
make main1 debug=True
./main1
make clean
```

### 问题（5%）

1. 为什么两个 `function.h` 都引用了 `shared.h` 而没有出问题？本来有可能出什么问题。

2. 如果把 `shared.h` 中注释掉的变量定义取消注释会出什么问题？为什么？

3. 通常使用 `shared.h` 中另外被注释掉的宏命令来规避重复引用的风险，原理是什么？取消这些注释之后上一题的问题解除了吗？不管解没解除背后的原因是什么？

## Task 2 - Static Link Library

### 知识

有些时候 `Makefile` 会因为工程的扩大而过于庞大，我们会希望减小单个 `Makefile` 内指令的数量。

很自然的，就像写代码一样，我们希望能够将一个 `Makefile` 拆分成多个。

很遗憾的是，每一个目录只能有一个 `Makefile` 文件，所以大多数项目将不同部分的代码分装在不同的目录下，每一个目录指定一个 `Makefile`，以期将这个目录里的文件例如源代码转换成其他的文件格式例如静态链接库。

使用如下命令可以在顶层的 `Makefile` 里启用子目录里的 `Makefile` ：

```makefile
cd <path to folder> && make <target name>
```

使用如下命令将一个或多个编译过的文件打包为静态链接库：

```makefile
ar -r <lib name> [<filename> ...]
```

例如使用如下命令将编译好的 `B.o` 打包为静态链接库：

```makefile
ar -r libB.a B.o
```

在 `Linux` 系统中，通常使用 `lib<name>.a` 来命名静态链接库

在 `gcc/g++` 中，可以简单认为静态链接库与源代码的地位一样。

例如，如下命令就可以把当前目录下的 A 静态链接库和 `main.cpp` 编译并链接在一起：

```makefile
g++ libA.a main.cpp -o main
```

### TODO（20%）

```
task2
├── A
│   ├── A.cpp
│   ├── A.h
│   └── Makefile    // TODO
├── B
│   ├── B.h
│   ├── B.o
│   └── libB.a
├── C
│   ├── C.cpp
│   ├── C.h
│   └── Makefile    // TODO
├── Makefile        // TODO
└── main.cpp
```

编写 `A/Makefile` 和 `C/Makefile`，编译目标是 `A/libA.a` 和 `C/libC.a`。

编写 `Makefile`，调用 `A/Makefile` 和 `C/Makefile`，使用如下命令编译链接并运行程序：

```
make clean
make main
size main
./main
make clean
```

### 问题（5%）

1. 若有多个静态链接库需要链接，编写命令时需要考虑静态链接库和源文件在命令中的顺序吗？是否需要考虑是由什么决定的？

2. `size main` 命令可以查看可执行文件 A 所占的空间，输出结果的每一项是什么意思？

## Task 3 - Dynamic Link Library

### 知识

使用如下命令可以将 `B.cpp` 编译为动态链接库：

```makefile
g++ -shared -fPIC B.cpp -o libB.so
```

在 `Linux` 系统中，通常使用 `lib<name>.so` 来命名动态链接库。

编译时链接动态链接库的语法与静态链接库一样。

### TODO（20%）

```
task3
├── A
│   ├── A.cpp
│   ├── A.h
│   └── Makefile    // TODO
├── B
│   ├── B.h
├── C
│   ├── C.cpp
│   ├── C.h
│   └── Makefile    // TODO
├── Makefile        // TODO
├── libB.so
└── main.cpp
```

代码同 `task2`，编写 `A/Makefile` 和 `C/Makefile`，编译目标是 `libA.so` 和 `libC.so`。

编写 `Makefile`，调用 `A/Makefile` 和 `C/Makefile`，使用如下命令编译链接并运行程序：

```
make clean
make main
size main
./main
make clean
```

### 问题（5%）

1. 动态链接库在运行时也需要查找库的位置，在 `Linux` 系统中，运行时动态链接库的查找顺序是怎样的？

2. 使用 `size main` 查看编译出的可执行文件占据的空间，与使用静态链接库相比占用空间有何变化？哪些部分的哪些代码会导致编译出文件的占用空间发生这种变化？

3. 编译动态链接库时 `-fPIC` 的作用是什么，不加会有什么后果？

4. 在 `Linux` 系统中，现在被广泛使用的公开的动态链接库如何进行版本替换或共存？

## Task 4 (H) - Cool Down

### TODO（3%）

```
task4
├── Makefile    // TODO
├── main.cpp    // TODO
├── some.cpp
└── some.h
```

代码同 `task0`，修改 `main.cpp` 中的错误代码，在 `Makefile` 中编写编译命令，使用如下命令编译并运行程序：

```bash
make clean
make main
./main
make clean
```

但是，不允许使用 `g++` 编译好整个完整的程序。

要求先使用 `gcc -c` 将 `C++` 代码编译为各自的 `elf` 文件（`.o`文件），再使用 `ld` 命令手动进行链接。

### 问题（2%）

1. 添加的动态链接库分别是什么，起什么作用？

2. 一个操作系统中只需要一个动态链接器吗？为什么？

## Task 5 (H) - New World

### TODO（5%）

```
task5
├── login
├── task5.sh    // TODO
└── your_code.* // TODO
```

运行已经编译好的 `login` 程序之后能够输入一个密码字符串，如果密码与预先设定的密码一样，就会输出 `login successful`；如果密码错误，就会输出 `incorrect password`。此外还会输出两个数字，无需在意。

目标是在不更改 `login` 程序的情况下，让 `login` 程序输出 `login successful`。

将进行的操作写入 `task5.sh` 中。

## 作者

- Ximing Yang (2019 Fall; 2020 Fall)
- Zhibo Zhang (2023 Fall)