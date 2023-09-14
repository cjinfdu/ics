---
title: ICS实验入门手册
date: 2023-09-08 00:00:00
---

## I. Linux系统

本学期的实验需要使用Linux系统。如果你的电脑不是Linux系统，建议使用虚拟机安装Linux系统。

手册中介绍VMWare Workstation Player和WSL 2两种虚拟机方案。 **建议Windows系统的同学选择WSL 2。**

**VMWare Workstation Player**

VMWare Workstation Player供个人免费使用，可在[官方网站](https://www.vmware.com/cn/products/workstation-player.html)下载。VMWare Workstation Player的图形界面设计较为清晰，不需要额外教学，大家可以自行尝试。

VMWare Workstation Player使用的系统镜像可以从各Linux发行版官网下载，如[Ubuntu](https://www.ubuntu.com)。

**WSL 2**

WSL 2是Microsoft推出的虚拟机方案，仅支持Windows系统，使用体验极佳。对于Windows 10 2004以上版本或Windows 11系统，可以参考[WSL官方文档](https://learn.microsoft.com/zh-cn/windows/wsl/install)安装WSL 2。该文档中还包含其它一些有用的信息，推荐感兴趣的同学完整浏览，特别是跨系统文件访问和跨系统网络访问。（当然，完整浏览不是实验必要的）

> 如果直接wsl --install失败，请参考文档中提供的手动安装步骤和常见问题。仍有疑问欢迎联系GPT和助教。对于C盘可用容量堪忧的同学，可以参考[这篇知乎文章](https://zhuanlan.zhihu.com/p/621873601)来把WSL迁移到别的盘。
> 注：用户名不要用大写，不要用中文！（经热心同学踩坑后加上）

> WSL已默认使用WSL2，其与WSl1之间的差异见[官方文档](https://learn.microsoft.com/zh-cn/windows/wsl/compare-versions)。

**Mac等特殊情况**

VMWare Workstation Player不支持Mac。如有使用Mac系统的同学，请尝试[Docker](https://www.docker.com/)或[VirtualBox](https://www.virtualbox.org/)。

后续部分实验基于x86-64架构设计。**如有使用Mac M1芯片，或其它非x86-64指令集CPU（如龙芯——~~不会真有人用吧~~）的同学，建议联系助教为你安排服务器进行实验，也可尝试UTM或QEMU，但性能可能略低。**

**Linux命令行**

对于虚拟机和Windows 11版本的WSL 2，可以自行为Linux系统安装合适的图形界面应用，但使用命令行在后续实验中是必须掌握的基础技能。命令行的基本用法可以参考[Linux Manual](https://man7.org/linux/man-pages/man1/intro.1.html)。如果你不想硬啃这个官方手册，我们也推荐通过[MIT Missing Semester](https://missing-semester-cn.github.io/)上的第一、二节入门Linux命令行使用。

一般来说，各种命令都可以通过在后面加上```--help```的方式输出用法，如运行```ls --help```可输出```ls```命令的用法。
如果你需要更加详细的说明，可以通过```man```查询命令对应的文档，如```man tar```，也可以在[GNU网站](https://www.gnu.org/software/software.html)上找到相关软件的具体说明。
另外推荐一个命令常用用法速查的命令行工具 `tldr` ，这是[GITHUB仓库地址](https://github.com/tldr-pages/tldr)，可以自行安装。（看不懂怎么安装的话 `pip3 install tldr` 即可）

下列基本命令和操作较为有用，推荐同学可以试试看：

* cd ls mv rm cp cat echo mkdir rmdir sudo pwd
* 输出重定向（>） 输入重定向（<） 管道（|） 结束程序（Ctrl+C） 自动补全（Tab）
* chgrp chmod chown jobs kill fg bg ln top grep find export
* 后台运行（&） 暂停程序（Ctrl+Z） 引用job（%） EOF（Ctrl+D） 临时环境变量（=）

## II. 包管理器

> 以Ubuntu系统的apt包管理器为例，其它发行版可以自行查找有关资料。

包管理器全称是软件包管理器，顾名思义是用来管理软件包的软件。在大家熟悉的Windows系统中，通常下载软件就是去软件的官网上下载。而在Linux系统中，最常见的安装软件的方式是使用软件包管理器从“软件仓库”中下载。包管理器会负责一个软件的全生命周期，包括下载、安装、依赖关系、卸载、更新等等。

Ubuntu发行版中带有apt和dpkg包管理器，我们一般使用apt，基本用法可以参考[Ubuntu包管理器文档](https://ubuntu.com/server/docs/package-management)中的apt一节。完整的官方文档可以运行man apt查阅。

**请注意使用apt时一般都需要sudo。**

你也可以自行安装aptitude包管理器，官方文档中也有对其的介绍。该管理器用法与apt类似，且提供了图形化界面。

### 换源（非常重要！！！！！！！！！！）

apt默认的软件源服务器在国外，访问速度较慢。建议将其更换为科大镜像（P.S. 咱学校的CSLG未来或许会搭建复旦自己的镜像站），Ubuntu的更换方法如下：

```shell
sudo cp /etc/apt/sources.list /etc/apt/sources.list.bak
sudo sed -i 's@//.*archive.ubuntu.com@//mirrors.ustc.edu.cn@g' /etc/apt/sources.list
sudo apt update
```

其中第一行是将原来的文件进行备份，这是一个好习惯！
上面几行的详细说明可以参考[这个链接](https://mirrors.ustc.edu.cn/help/ubuntu.html)，如果你使用其他发行版，可以去这个链接中寻找相关说明。

> 如需要在命令行下使用代理，建议了解一下proxychains。

## III. 常用软件

### GNU Tools

请在Linux系统中安装常用的GNU工具链。参考命令：```sudo apt install binutils diffutils gcc gdb grep g++ make tar```。

> GNU工具链中包含了许多的常用工具，如grep、tar、gcc、g++、make、objdump、gdb等。我们在后续课程中会重点介绍其中的一些工具。

### Vim

Vim的基本用法为运行```vim something.txt```打开文件，然后按```i```键编辑文件，此时可以直接打字，完成后按```ESC```键，输入```:wq```保存并退出，或输入```:q!```不保存直接退出。

因为学习难度较大，我们更推荐大家使用开箱即用的VS Code。

Vim的用法非常丰富，如有兴趣可以通过Vim自带的教程程序 `vimtutor` 学习（直接在命令行输入这个即可），也可以通过[Vim学习网站](https://vim-adventures.com/)学习，也可参考[视频](https://www.bilibili.com/video/BV13t4y1t7Wg)。

### Git

Git的常用操作可以通过[Git学习网站](https://learngitbranching.js.org/?locale=zh_CN)学习，也可参考[视频](https://www.bilibili.com/video/BV1r3411F7kn)。完整的官方文档可以运行man git查阅。

你也可以使用图形化的Git界面，如VS Code的git插件。

此外，要将代码库上传到云端，你可能需要注册一个[GitHub](www.github.com)或其它类似网站的账号。本课程暂时不会涉及需要使用git的场景~~，但或许你可以注册一个账号给我们的课程网站仓库一个star~~。

### SSH

在ICS课程中没有直接使用SSH连接服务器的内容，但你可能会需要[在Git中使用SSH](https://docs.github.com/cn/authentication/connecting-to-github-with-ssh/)。SSH的完整文档可以通过man ssh查阅。

如果你从未使用过SSH，你可能需要为自己创建一个密钥对，参考指令：```ssh-keygen -t rsa -b 4096 -C "your_email@example.com"```。密钥对的默认保存位置为 `~/.ssh/`，其中的 `id_rsa` 为私钥，`id_rsa.pub` 为公钥。 **请务必妥善保管你的私钥，不要将其发给别人！**

### VS Code

建议安装一个好用的代码编辑器，如[VS Code](https://code.visualstudio.com/)。如果你使用的是VMWare，需要将VS Code安装在虚拟机里，并可以在本机上也安装一份，以便编写代码。如果你使用的是WSL 2，建议直接将VSCode装在本机上，可[使用本机上的VS Code编辑和运行虚拟机中的代码](https://code.visualstudio.com/docs/remote/wsl)。

VS Code会提示你安装常用插件，本文不再给出插件安装的建议。

关于VS Code安装与配置的更多问题，可参考[VS Code官方文档](https://code.visualstudio.com/docs)。

> WSL2常用用法：
> `code some.txt` 即可用Windows上的VSCode打开文件。
> `code .` 即可用Windows上的VSCode打开当前文件夹。
