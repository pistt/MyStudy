[toc]



# LINUX

## GCC 的使用

**- c** ：只编译，不链接成为可执行文件，生成 .o 目标文件

**- o** ：生成可执行文件，默认后缀名为 .out

**- E**：预编译后停下来，生成后缀为 .i 的预编译文件。

**- S**：汇编后停下来，生成后缀为 .s 的汇编源文件。

**- $l$dirname**：将 dirname 所指出的目录加入到程序**头文件目录列表**中，是在预编译过程中使用的参数。

**- $l$libname**：静态链接库，从系统目录找。

> gcc -o test.o main.c -lmath 表示链接 math.a 这个库，注意没有空格

**- L**：静态链接库，从指定目录找。首先到 -L 指定的目录中去寻找，然后到系统默认路径中寻找，

## Makefile 的编写

文章博客教程：

> 1.http://c.biancheng.net/makefile/
>
> 2.https://blog.csdn.net/weixin_38391755/article/details/80380786/
>
> 3.https://www.zhaixue.cc/makefile/makefile-intro.html
>
> 4.http://www.codebaoku.com/makefile/makefile-file-format.html

视频教程：

> 1.https://www.bilibili.com/video/BV1L741187pF

### 我的实践

参见`./code/xxxxxxx`



## 生成静态链接库

使用 ar 命令

## 生成动态链接库

### **soname**（ Short for shared object name）

> [原博客](https://www.cnblogs.com/wangshaowei/p/11285332.html)

其是应用程序加载dll 时候，其寻找共享库用的文件名。其格式为

```
lib + math+.so + ( major version number)
```

​	其只包含major version number，换句话说，也就是只要其接口没有变，应用程序都可以用，不管你其后minor build version or build version。

简单来说：

​	-Wl,-soname -Wl 告诉编译器将后面的参数传递到连接器。而 -soname 指定了
共享库的 soname这个soname的存在是为了兼容方便。

比如：

  	有一个程序ap1,以及一个库libtest.so.1
  	
  	ap1启动的时候需要libtest.so.1
  	
  	如果链接的时候直接把libtest.so.1传给了ap1，那么将来库升级为libtest.so.2的时候，ap1仍然只能使用libtest.so.1的代码，并不能得到升级的好处。而如果指定了**soname为libtest.so,那么ap1启动的时候将查找的就是libtest.so而不是其在被链接时实际使用的库libtest.so.1这个文件名。**
  	
  	**在开始时我们建立一个链接:ln -sf libtest.so.1 libtest.so**
  	
  	**而在库升级后，我们重新：ln -sf libtest.so.2 libtest.so即可**，这样ap1不需要任何变动就能享受升级后的库的特性了。而libtest.so.1,libtest.so.2可以同时存在于系统内，不必非得把libtest.so.2的名字改成libtest.so.1

### real name 共享库本身的文件名

lib+库名+so.主版本号.小版本号.build号

主版本号，代表当前动态库的版本，如果动态库的接口有变 化，那么这个版本号就要加1；后面的两个版本号（小版本号 和 build 号）是告诉你详细的信息，比如为一个hot-fix 而生成的一个版本，其小版本号加1，build号也应有变化。 这个文件名包含共享库的代码。

 

### link name 共享库的连接名

lib + math +.so

link name，顾名思义，就是在编译过程，link 阶段用的文件名。 其将soname 和real name 关联起来。

在共享库编译过程中，连接（link） 阶段，编译器将生成一个共享库及real name，同时将共享库的soname，写在共享库文件里的文件头里面。当应用程序加载时就会通过soname 去给定的路径下寻找该共享库

可以用命令 readelf -d sharelibrary 去查看。



