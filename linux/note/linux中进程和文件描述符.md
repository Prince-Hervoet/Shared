<a name="TKe3w"></a>

# 进程

进程在 linux 中就是一个数据结构。用户空间中装载着用户进程需要使用的资源，比如在程序中开辟了一块数组，这个数组就是存在于用户空间；而内核空间存放内核进程需要的资源，这些资源用户进程一般是访问不了的，但是有的用户进程会共享一些内核空间的资源，比如动态链接库<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/22646069/1675924541303-cfb62d46-4c97-482c-8ab2-e64f103dc916.png#averageHue=%23ece3d2&clientId=u79acb8cf-37b0-4&from=paste&id=ud7e86d13&originHeight=720&originWidth=1280&originalType=url&ratio=1&rotation=0&showTitle=false&size=616070&status=done&style=none&taskId=ucded9156-209e-4760-bb3c-f8869d00b5c&title=)

> 编译好的可执行程序是一个文件，可执行文件要执行就必须要载入内存，变成一个进程数据结构才能被调度。进程是要依靠操作系统创建，每个进程都有它的固有属性

```c
struct task_struct {
	// 进程状态
	long			  state;
	// 虚拟内存结构体
	struct mm_struct  *mm;
	// 进程号
	pid_t			  pid;
	// 指向父进程的指针
	struct task_struct __rcu  *parent;
	// 子进程列表
	struct list_head		children;
	// 存放文件系统信息的指针
	struct fs_struct		*fs;
	// 一个数组，包含该进程打开的文件指针
	struct files_struct		*files;
};
```

> task_struct 就是 linux 对一个进程的描述，也称为进程描述符。其中 mm 指针是进程的虚拟内存，files 指针指向一个数组，这个数组中存着所有该进程打开的文件指针

<a name="Q5piz"></a>

# 文件描述符

files 是一个文件指针数组，一般来说一个进程会从 files[0]输入，将输出写入 files[1]，将错误信息写入 files[2]。每个进程被创建时，files 的前三位被填入默认值，分别指向标准输入流、标准输出流、标准错误流，所谓的文件描述符就是指这个文件指针数组的索引<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/22646069/1675925228742-e35f7bb3-c7fe-48a1-b62b-93d68773df56.png#averageHue=%23ece3d2&clientId=u79acb8cf-37b0-4&from=paste&id=u62bafbf9&originHeight=720&originWidth=1280&originalType=url&ratio=1&rotation=0&showTitle=false&size=707594&status=done&style=none&taskId=u80346347-0648-4fea-99f2-ca49eb5a49f&title=)

对于一般的计算机，输入流是键盘，输出流是下显示器，错误流也是显示器，因为硬件都是由内核管理的，用户进程需要通过系统调用让内核进程访问硬件资源

> linux 中一切皆文件，就是将所有的设备和资源抽象成可操作文件

如果我们的程序还需要继续打开其他资源，这个文件就会被放到 files 的第 4 个位置：<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/22646069/1675925450665-dc7f86b7-bf15-41f0-b33d-a1f1965738f9.png#averageHue=%23ede4d3&clientId=u79acb8cf-37b0-4&from=paste&id=u9ec2096c&originHeight=720&originWidth=1280&originalType=url&ratio=1&rotation=0&showTitle=false&size=735913&status=done&style=none&taskId=u6fd78239-2fc1-4dbe-8e70-8d050e71668&title=)

所以输入重定向就是当程序想要读取数据的时候就会去 files[0]读取，所以我们只要把 files[0]指向一个文件，那么程序就会从这个文件中读取数据，而不是键盘；输出重定向就是把 files[1]指向一个文件，那么程序的输出就不会写入到显示器，而是写入到这个文件中

管道符就是将一个进程的输出流和另一个进程的输入流连接起来，数据就在其中传递<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/22646069/1675925754193-c83b6edc-22c6-4343-a351-630d01dbe621.png#averageHue=%23ece3d3&clientId=u79acb8cf-37b0-4&from=paste&id=u0c0adbb4&originHeight=720&originWidth=1280&originalType=url&ratio=1&rotation=0&showTitle=false&size=728805&status=done&style=none&taskId=u620692d3-0c83-4b1f-899d-28304825ca8&title=)

<a name="vbmWI"></a>

# 线程

在 linux 中对于线程和进程基本没有区别，因为从 linux 内核来看并没有把进程和线程区别对待。系统调用 fork()可以创建一个子进程，函数 pthread()可以创建一个线程，但是无论线程还是进程，都是使用 task_struct 结构表示，唯一的区别就是共享的数据区域不同

> 线程数据结构看起来跟进程没有区别，只是线程的某些资源是和进程共享的，而子进程是拷贝副本，不是共享。也就是 mm 结构和 files 结构在线程中都是共享的

![image.png](https://cdn.nlark.com/yuque/0/2023/png/22646069/1675926042439-204a7f19-7f93-4364-979a-628fa7f3b77e.png#averageHue=%23eff1e9&clientId=u79acb8cf-37b0-4&from=paste&id=u3a6d0b20&originHeight=720&originWidth=1280&originalType=url&ratio=1&rotation=0&showTitle=false&size=911915&status=done&style=none&taskId=uca55d6a4-87f1-4f40-b2af-dd354c46a7e&title=)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/22646069/1675926048465-f328cb68-79ef-4065-a18b-f3e3b8ffdc68.png#averageHue=%23f6f3ec&clientId=u79acb8cf-37b0-4&from=paste&id=ua90877ab&originHeight=720&originWidth=1280&originalType=url&ratio=1&rotation=0&showTitle=false&size=876702&status=done&style=none&taskId=u03ed677b-aa17-4909-a2fb-a569c4ab6a5&title=)
