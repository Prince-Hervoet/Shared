<a name="ZXtXA"></a>
# 用户态和内核态
linux系统分成了用户态和内核态，CPU可以在两个状态之间切换：

- 内核态拥有完全的底层资源控制权限，可以执行任何CPU指令，访问任何内存地址，其占用的处理器是不能被抢占的，经典的指令包含：启动IO、内存清零、修改程序
- 用户态是用户程序能够使用的指令，不能直接访问底层硬件和内存地址，用户态运行的程序必须委托系统调用来访问硬件和内存，经典指令包含：控制转移、算数运算

用户态切换到内核态有三种方式：

- 系统调用：这是用户态进程主动要求切换到内核态的一种方式，用户态进程通过系统调用申请使用操作系统提供的服务程序完成工作，比如fork()实际上就是执行了一个创建新进程的系统调用，而系统调用的机制核心还是使用了操作系统为用户特别开放的一个中断来实现
- 出现异常：当CPU在执行用户态下的程序时，发生了某些事先不可预知的异常，就会触发由当前运行进程切换到处理此异常的内核相关程序种，也就转化到了内核态，比如缺页异常

![](https://cdn.nlark.com/yuque/0/2022/png/22646069/1665750848288-1633b491-496c-4492-bdd8-e979d4730854.png#averageHue=%23faf6f6&clientId=ua7ed809b-6697-4&from=paste&id=ucb205655&originHeight=341&originWidth=690&originalType=url&ratio=1&rotation=0&showTitle=false&status=done&style=none&taskId=u05f75076-cb7d-442e-b6ed-b9d77cfd261&title=)

<a name="TfOYn"></a>
# linux下的五种IO模型

- 阻塞IO
- 非阻塞IO
- IO复用
- 信号驱动IO
- 异步IO

<a name="nBTPE"></a>
## 阻塞IO
![](https://cdn.nlark.com/yuque/0/2022/jpeg/22646069/1665739249334-974d8c83-4fb3-4ae6-ad42-0913bac92093.jpeg#averageHue=%23f5f5f5&clientId=ua7ed809b-6697-4&from=paste&id=ucbcd4219&originHeight=404&originWidth=780&originalType=url&ratio=1&rotation=0&showTitle=false&status=done&style=none&taskId=u8135a945-7784-4c58-9c3a-bd860fdf90b&title=)应用程序调用一个IO函数，导致应用程序阻塞，等待数据准备好，数据准备好，从内核拷贝到用户空间，IO函数返回成功指示，阻塞IO模型

<a name="pkGUI"></a>
## 非阻塞IO模型
通过进程反复调用IO函数，在数据拷贝过程中，进程是阻塞的<br />![](https://cdn.nlark.com/yuque/0/2022/jpeg/22646069/1665739368374-d88472f1-8788-45db-a5b7-08aa3df80a01.jpeg#averageHue=%23f1f1f1&clientId=ua7ed809b-6697-4&from=paste&id=u54e98775&originHeight=461&originWidth=828&originalType=url&ratio=1&rotation=0&showTitle=false&status=done&style=none&taskId=u1898bda7-c9e8-43b3-90ee-ab7b2bd85ba&title=)

<a name="Jiq6c"></a>
## IO复用模型
主要是select和epoll，一个线程可以对多个IO端口进行监听，当有socket有读写事件时分发到具体的线程进行处理<br />![](https://cdn.nlark.com/yuque/0/2022/jpeg/22646069/1665739557511-63f2cc92-d726-4057-a50a-b340d33ac7a7.jpeg#averageHue=%23f4f4f4&clientId=ua7ed809b-6697-4&from=paste&id=udff21ae7&originHeight=527&originWidth=887&originalType=url&ratio=1&rotation=0&showTitle=false&status=done&style=none&taskId=ufc7a18af-24b6-4c1c-8125-617154d0ba4&title=)

<a name="vzpia"></a>
## 信号驱动IO模型
信号驱动IO是首先我们允许socket进行信号驱动IO，并安装一个信号处理函数，进程继续运行并不阻塞，当数据准备之后，进程会受到一个SIGIO信号，可以在信号处理函数中调用IO操作函数处理数据<br />![](https://cdn.nlark.com/yuque/0/2022/png/22646069/1665739753370-bf26e555-edae-4776-99d0-b1880aebfd7c.png#averageHue=%23f3f3f3&clientId=ua7ed809b-6697-4&from=paste&id=u560fd5e8&originHeight=758&originWidth=1192&originalType=url&ratio=1&rotation=0&showTitle=false&status=done&style=none&taskId=uf56f317a-a2e3-437e-b61f-299a2b92281&title=)

<a name="TXVCZ"></a>
## 异步IO模型
相对同步IO，异步IO不是顺序执行的，用户进程执行完系统调用之后，无论内核数据是否主备好都会直接返回给用户进程，然后用户态进程可以去做别的事情，等到socket数据准备好之后内核直接复制数据给进程，然后再向进程发送通知，IO两个阶段进程都是非阻塞的<br />![](https://cdn.nlark.com/yuque/0/2022/png/22646069/1665739858412-54d8b3c9-d23a-437b-97f7-b8ef4069a447.png#averageHue=%23f7f7f7&clientId=ua7ed809b-6697-4&from=paste&id=u5841b86a&originHeight=740&originWidth=1176&originalType=url&ratio=1&rotation=0&showTitle=false&status=done&style=none&taskId=u159c7286-b175-4732-b98a-c03f094b0cf&title=)
