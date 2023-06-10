<a name="tIfBN"></a>
# 什么是binder机制
安卓系统是基于linux内核设计的，linux中提供了管道、消息队列、共享内存和网络通信等IPC机制，但是这些机制都各有各的优缺点，在安卓这种大型开发使用场景中都不能完全满足性能、稳定性和安全性几个方面，所以引出了binder这种机制

从性能上看，linux中的socket通信在同一个主机上使用传输效率低下、开销也比较大；消息队列和管道采用存储转发方式，整个过程至少需要两次拷贝；共享内存不需要拷贝，但是使用上控制复杂
> binder机制需要一次拷贝，性能上仅次于共享内存


从稳定性上看，binder基于CS架构，client有需求就丢给服务端，架构较为清晰，共享内存虽然性能最好，但是控制比较复杂，所以binder的稳定性是优于共享内存的

从安全性上看，安卓作为一个开放性平台，市场上拥有大量的APP供用户下载使用，因此这个安全性保证非常重要，这其中就包括APP隐匿读取用户通信录、上传隐私数据、偷取流量等操作，传统的IPC并没有完善的安全机制，完全依赖上层协议来保证安全。首先传统的IPC接收方无法获得对方可靠的进程ID，无法鉴别对方的真实身份，所以安卓为每一个安装的APP分配了一个唯一UID（可靠的身份标识只能由IPC机制在内核层面添加）；其次传统的IPC访问接入点是开放的，只要知道接入点的程序都可以和对端建立连接

<a name="v4iss"></a>
# linux下进程通信的基本流程
由于在操作系统中两个进程之间的内存是隔离的，互不干扰，而如果两个进程之间需要交换某些信息就需要进行进程间通信。而在linux中进程访问的都是虚拟内存，整个虚拟内存分成了内核空间和用户空间，虽然从逻辑上进行了用户空间和内核空间的划分，但是不可避免地需要访问内核资源，所以需要通过系统调用来实现，而这里就设计了用户态和内核态的切换<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/22646069/1686391966939-b4cd2771-8d66-4c00-a123-69154bfd3b7b.png#averageHue=%23fadcd5&clientId=u524c3baa-06b4-4&from=paste&height=1242&id=uf2a017be&originHeight=1552&originWidth=2576&originalType=binary&ratio=1.25&rotation=0&showTitle=false&size=35584&status=done&style=none&taskId=ub057fa8d-49ed-45a5-afe6-d075d8f12e3&title=&width=2060.8)

传统的IPC通信模型<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/22646069/1686392222965-c5c734a1-6c0e-41e6-9d0d-3ee4457fd1d2.png#averageHue=%23b5e2d9&clientId=u524c3baa-06b4-4&from=paste&height=1023&id=u96f4d352&originHeight=1279&originWidth=3125&originalType=binary&ratio=1.25&rotation=0&showTitle=false&size=39608&status=done&style=none&taskId=u24c60c99-81f2-420d-a05d-d4146442031&title=&width=2500)
> - 这个过程中需要从内存缓冲区 --> 内核缓存区 --> 内存缓存区，需要两次数据拷贝
> - 接收数据的缓存区由数据接收进程提供，但是接收进程并不知道需要多大的空间来存放将要传递过来的数据，因此只能开辟尽可能大的内存空间或者先调用 API 接收消息头来获取消息体的大小，这两种做法不是浪费空间就是浪费时间


<a name="vWNHD"></a>
# 动态内核可加载模块
跨进程通信是需要内核空间做支持的，传统的IPC通信如管道、socket通信等都是内核支持的一部分，因此通过内核支持来实现进程间通信是没问题的，但是binder并不是linux系统内核的一部分，所以需要通过linux动态内核可加载模块机制来实现，模块是具有独立功能的程序，但是不能独立运行，需要在运行时被链接到内核作为内核的一部分参与运行，这样安卓系统就可以通过动态添加一个内核模块运行在内核空间，用户进程之间通过这个模块来实现通信<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/22646069/1686393615380-1ef503c1-a72e-4d03-8b26-fbae4e1f7423.png#averageHue=%23b0e0d5&clientId=u524c3baa-06b4-4&from=paste&height=1014&id=u81447ef0&originHeight=1268&originWidth=3154&originalType=binary&ratio=1.25&rotation=0&showTitle=false&size=66184&status=done&style=none&taskId=u924a7948-e55c-4c7a-a6d1-4365b196df3&title=&width=2523.2)
> 一次完整的 Binder IPC 通信过程：
> 1. 首先 Binder 驱动在内核空间创建一个数据接收缓存区；
> 2. 接着在内核空间开辟一块内核缓存区，建立**内核缓存区**和**内核中数据接收缓存区**之间的映射关系，以及**内核中数据接收缓存区**和**接收进程用户空间地址**的映射关系；
> 3. 发送方进程通过系统调用 copy_from_user() 将数据 copy 到内核中的**内核缓存区**，由于内核缓存区和接收进程的用户空间存在内存映射，因此也就相当于把数据发送到了接收进程的用户空间，这样便完成了一次进程间的通信。


Binder IPC机制中涉及到的内存映射通过mmap()实现，mmap()是操作系统中一种内存映射的方法，内存映射简单的讲就是讲用户空间的一块内存区域映射到内核空间。映射关系建立后用户对这块内存区域的修改可以直接映射到内核空间。内存映射减少了数据拷贝的次数，实现了用户空间和内核空间的高效互动，两个空间各自的修改能够直接同步

<a name="q3eoo"></a>
# Binder通信模型
<a name="vDF39"></a>
## Client/Server/ServiceManager驱动
Binder是基于CS架构的，由一系列的组件组成，包括Client、Server、ServiceManager、Binder驱动。其中Client、Server、ServiceManager运行在用户空间，Binder驱动运行在内核空间<br />![](https://cdn.nlark.com/yuque/0/2023/webp/22646069/1686413368942-233326a3-9e7d-4d4c-9fd3-7c4f19cdd371.webp#averageHue=%23f8e6e1&clientId=u524c3baa-06b4-4&from=paste&id=u1f44c249&originHeight=423&originWidth=720&originalType=url&ratio=1.25&rotation=0&showTitle=false&status=done&style=none&taskId=u81886622-143d-4843-ba94-6946277e1b9&title=)
> Binder 驱动就如同路由器一样，是整个通信的核心；驱动负责进程之间 Binder 通信的建立，Binder 在进程之间的传递，Binder 引用计数管理，数据包在进程之间的传递和交互等一系列底层支持


Binder通信过程：

1. 首先，一个进程使用 BINDER_SET_CONTEXT_MGR 命令通过 Binder 驱动将自己注册成为 ServiceManager；
2. Server 通过驱动向 ServiceManager 中注册 Binder（Server 中的 Binder 实体），表明可以对外提供服务。驱动为这个 Binder 创建位于内核中的实体节点以及 ServiceManager 对实体的引用，将名字以及新建的引用打包传给 ServiceManager，ServiceManger 将其填入查找表。
3. Client 通过名字，在 Binder 驱动的帮助下从 ServiceManager 中获取到对 Binder 实体的引用，通过这个引用就能实现和 Server 进程的通信。
