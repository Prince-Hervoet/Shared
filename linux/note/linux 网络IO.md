# linux 网络IO

# 网络请求接收流程

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled.png)

当网络数据帧通过网络达到网卡时，网卡会将网络数据帧通过DMA的方式存储到环形缓冲区RingBuffer中

- RingBuffer是网卡启动的时候自动分配和初始化的，当缓冲区满载时，新到达的数据包会被丢弃
    
    

当DMA操作完成时，网卡会向CPU发送一个硬件中断，通过CPU有数据到达，CPU调用网卡驱动注册的硬中断响应程序。网卡硬中断响应程序会为网络数据帧创建数据结构sk_buffer，然后将数据拷贝到sk_buffer中，再发送一个软中断，通过内核有数据到达

- sk_buffer是一个维护网络数据帧结构的双向链表，链表的每一个元素是一个网络帧，虽然TCP/IP协议栈分层，但是上下层之间并不需要拷贝数据，而是通过指针移动来读取数据
    
    

内核线程ksoftirqd发现有软中断到达之后调用网卡驱动注册的poll函数，将sk_buffer中的数据发送到内核协议栈中注册的ip_rcv函数中

- 每一个CPU会绑定一个ksoftirqd内核线程用来处理软中断响应
- 网卡向CPU发送硬中断时是现需要均衡到多个CPU核心上的，否则会影响CPU亲和性

性能开销：

- 应用程序通过系统调用从用户态转变为内核态的开销（后面还需要再转回来）
- 网络数据从内核空间拷贝到用户空间
- 内核线程响应软中断
- CPU响应硬中断
- DMA拷贝网络数据包到内存中

# 网络发送流程

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%201.png)

当我们在应用程序中调用send系统调用发送数据时，内核会先根据fd找到socket结构，这个socket结构中记录着各种协议栈的函数地址，然后构造struct msghdr对象，将用户需要发送的数据全部封装在这个struct msghdr结构体中

调用内核协议栈函数inet_sendmsg，发送流程进入内核协议栈处理，在进入到内核协议栈中后，内核会找到socket上的具体协议的发送函数

- 使用TCP就会使用tcp_sendmsg
    
    

在TCP协议的发送函数中，创建内核数据结构sk_buffer，将struct msghdr结构体中的数据拷贝到sk_buffer中，调用tcp_write_queue_tail函数获取socket发送队列中的队尾元素，将新创建的sk_buffer添加到socket发送队列的尾部

发送流程走到这里，用户要发送的数据已经从用户空间拷贝到了内核空间中，这时候虽然发送数据已经拷贝到了内核socket的发送队列中，但是内核还不会开始发送，因为TCP协议的流量控制和拥塞控制还会进行控制，需要符合TCP协议（传输层的要求）才能发送

如果符合发送条件，则开始调用tcp_write_xmit内核函数，在这个函数中会循环获取socket发送队列中等待发送的sk_buffer，然后进行拥塞控制和窗口管理等

将socket发送队列中获取到的sk_buffer重新拷贝一份，设置sk_buffer副本中的TCP头部

- sk_buffer中已经包含了网络协议的所有header，在设置TCP头部的时候只是将指针指向sk_buffer的合适位置，后面设置IP头部的时候继续动一下指针就行，不需要申请内存
    
    ![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%202.png)
    
- 为什么不直接使用socket发送队列中的sk_buffer而是重新拷贝一份？因为TCP协议是支持重传机制的，在没有收到对端的ACK之前，这个sk_buffer是不能删除的，内核每次调用网卡发送数据时，实际上传递的是sk_buffer的副本，当网卡将数据发送出去后，这个副本会被释放。在收到对端ACK之后，socket发送队列中sk_buffer才会被删除
    
    
- 设置完TCP头部后，内核协议栈传输层的业务就完成了，下面就会将数据传递给IP层，来到网络层处理
    - 将sk_buffer中的指针移动到IP头位置上，设置IP头
    - 执行netfilers过滤，过滤通过之后，如果数据大于MTU的话，则执行分片
    - 检查socket中是否有缓存路由表，如果没有的话，则查找路由项，并缓存到socket中，接着在把路由表设置到sk_buffer中

内核协议栈网络层的事情处理完后，现在发送流程进入到邻居子系统，邻居子系统位于内核协议栈中的网络层和网络接口层之间，用于发送ARP请求获取MAC地址，然后将sk_buffer中的指针移动到MAC头位置，填充MAC头部

经过邻居子系统的处理，现在sk_buffer中已经封装了一个完整的数据帧，随后内核将sk_buffer交给网络设备子系统进行处理，网络设备子系统主要做：

- 选择发送队列，因为网卡拥有多个发送队列，所以需要确定一个发送队列
- 将sk_buffer添加到发送队列
- 循环从发送队列中取出sk_buffer，调用内核函数sch_direct_xmit发送数据，其中会调用网卡驱动程序来发送数据
- 以上过程全部是用户线程的内核态执行，占用CPU时间是系统态时间，当分配给用户线程的CPU quota用完的时候，会触发NET_TX_SOFTIRQ类型的软中断，内核线程ksoftirqd会响应这个软中断，并执行NEW_TX_SOFTIRQ类型的软中断注册的回调函数
    
    

# IO多路复用

- 多路：我们的核心需求是用尽可能少的线程来处理尽可能多的连接，这里的多路就是我们需要处理众多的连接
- 复用：我们要使用尽可能少的线程，尽可能少地使用系统开销去处理尽可能多的连接

非阻塞IO模型就是在解决这个事情，但是由于传统非阻塞IO模型需要不断发起系统调用去轮询各个socket是否有数据可读可写，所以会导致很多无用的计算资源。随着并发量的增大，整体性能消耗会很大

## select

select是操作系统内核提供给我们的一个系统调用，它解决了在非阻塞IO中需要不断发起IO调用轮询而导致用户空间频繁切换到内核空间的问题，select系统调用将轮询操作交给了内核来帮我们完成，从而避免在用户空间不断发起轮询所带来的系统性能开销

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%203.png)

- 首先用户线程发起一次select系统调用的时候会阻塞在select系统调用上，用户线程从用户态切换到了内核态完成了一次上下文切换
- 用户线程将需要监听的socket对应的文件描述符fd数组通过select系统调用传递给内核。此时用户线程将用户空间中的文件描述符fd数组拷贝到内核空间

这里文件描述符数组其实是一个BitMap，BitMap下标为文件描述符fd，下标对应的值为：1表示该fd上有读写事件，0表示该fd上没有读写事件

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%204.png)

- 文件描述符fd就是一个整数值，在linux中一切都是文件，socket也抽象成了文件，描述进程所有信息的数据结构task_struct中有一个属性struct files_struct *files，它最终指向了一个数组，数组中存放了进程打开的所有文件列表，文件信息封装在struct file结构体中，这个数组存放的类型就是struct file结构体，数组的下标则是我们常说的文件描述符fd（内核会修改原始fd数组）
    
    

select不会告诉用户线程具体哪些fd上有IO数据到来，只是在IO活跃的fd上面打标记，将打好的标记的完整fd数组返回给用户线程，所以用户线程还需要遍历fd数组找出具体哪些fd上有IO数据到来

由于内核在遍历的过程中已经修改了fd数组，所以在用户线程遍历完fd数组后获取到IO就绪的socket后，就需要重置fd数组，并重新调用select传入重置后的fd数组让内核发起新的一轮遍历轮询

虽然select解决了非阻塞IO模型中频繁发起系统调用的问题，但是select本身还存在许多不足：

- 在发起select系统调用以及返回时，用户线程各发生了一次状态切换，发生两次上下文切换
- 在发起select系统调用以及返回时，用户线程在内核态需要将文件描述符集合从用户空间拷贝到内核空间。以及在内核修改完文件描述符集合后又要将它从内核空间拷贝到用户空间
- 操作的时间复杂度是O(N)

## Poll

poll相当于改进版的select，但是工作原理基本和select没有本质区别

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%205.png)

select中使用的文件描述符集合是采用的固定长度是1024的BitMap结构的fd_set，而poll换成了一个pollfd结构没有固定长度的数组，这样就没有了最大fd数量的限制

## socket创建

服务端线程调用accept系统调用后开始阻塞，当有客户端连接上来并完成TCP三次握手之后，内核会创建一个对应的socket作为服务端与客户端通信的内核接口

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%206.png)

struct task_struct是内核中用来表示进程的一个数据结构，它包含了进程的所有信息，其中进程内打开的所有文件是通过一个数组fd_array来进行组织管理，数组的下标即为我们常提到的文件描述符，数组的下标即为我们常常提到的文件描述符，数组中存放的是对应的文件数据结构struct file与之对应，并在fd_array中找到一个空闲位置分配给它，数组中对应的下标就是我们在用户空间用到的fd

用于封装文件元信息的内核数据结构struct file中的private_data指针指向具体的Socket结构。struct file中的file_operations属性定义了文件的操作系统，不同文件类型，对应的file_operations是不同的，针对socket文件类型，这里的file_operations指向socket_file_ops

socket的内核结构

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%207.png)

- 当我们调用accept之后，内核会基于监听socket创建出来一个新的socket用于与客户端之间的网络通信，并将监听socket中的socket操作函数集合赋值到新的socket的ops中

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%208.png)

- 监听socket和真正用来网络通信的socket是不同的
    
    

- 接着内核会为已连接的socket创建struct file并初始化，并把socket文件操作函数集合socket_file_ops赋值给struct file中的f_ops指针，然后将struct socket中的file指针指向这个新分配申请的struct file结构体
- 内核会维护两个队列：
    - 一个是TCP三次握手，连接状态处于established的连接队列，内核中为icsk_accept_queue
    - 一个是没有完成TCP三次握手，连接状态处于syn_rcvd的半连接队列

- 然后调用socket →ops →accept，从socket内核结构图中我们可以看到其实调用的是inet_accept，该函数会在icsk_accept_queue中查找是否有已经建立好的连接，如果有的话，直接从icsk_accept_queue中获取已经创建好的struct sock，并将这个struct sock对象赋值给struct socket中的sock指针
- 根据创建socket时发起的系统调用sock_create中的protocol参数，找到对于tcp定义的操作方法实现集合inet_stream_ops和tcp_prot，并将它们分别设置到socket→ops和sock→sk_prot上

## 阻塞IO中用户进程阻塞以及唤醒原理

当用户进程发起系统IO调用时，例如read，用户进程会在内核态查看对应的socket接收缓冲区是否有数据到来：

- 如果有数据，则拷贝数据到用户空间，系统调用返回
- socket接收缓冲区没有数据，则用户进程让出CPU进入阻塞状态，当数据到达接收缓冲区时，用户进程会被唤醒，从阻塞状态进入就绪状态，等待CPU调度

关于用户进程是如何阻塞在socket上：

- 首先我们在用户进程中对socket进行read系统调用时，用户进程会从用户态转变为内核态
- 在进程的struct task_struct结构找到fd_array，并根据socket的文件描述符fd找到对应的struct file，调用struct file中的文件操作函数结合file_operations，read系统调用对应的是sock_read_iter
- 在sock_read_iter函数中找到struct file指向的struct socket，并调用socket→ops→recvmsg，这里我们知道调用的是inet_stream_ops集合中定义的inet_recvmsg
- 在inet_recvmsg中找到struct sock，并调用sock→skprot→recvmsg，这里调用的是tcp_prot集合中定义的tcp_recvmsg函数

```c
int tcp_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
  size_t len, int nonblock, int flags, int *addr_len)
{
    .................省略非核心代码...............
   //访问sock对象中定义的接收队列
  skb_queue_walk(&sk->sk_receive_queue, skb) {

    .................省略非核心代码...............

  //没有收到足够数据，调用sk_wait_data 阻塞当前进程
  sk_wait_data(sk, &timeo);
}
```

```c
int sk_wait_data(struct sock *sk, long *timeo)
{
 //创建struct sock中等待队列上的元素wait_queue_t
 //将进程描述符和回调函数autoremove_wake_function关联到wait_queue_t中
 DEFINE_WAIT(wait);

 // 调用 sk_sleep 获取 sock 对象下的等待队列的头指针wait_queue_head_t
 // 调用prepare_to_wait将新创建的等待项wait_queue_t插入到等待队列中，并将进程状态设置为可打断 INTERRUPTIBLE
 prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
 set_bit(SOCK_ASYNC_WAITDATA, &sk->sk_socket->flags);

 // 通过调用schedule_timeout让出CPU，然后进行睡眠，导致一次上下文切换
 rc = sk_wait_event(sk, timeo, !skb_queue_empty(&sk->sk_receive_queue));
 ...
```

## epoll

### epoll_create创建epoll

epoll_create是内核提供的创建epoll对象的一个系统调用，当我们在用户进程中调用epoll_create时，内核会为我们创建一个struct eventpoll对象，并且也有相应的struct file与之关联，同样需要把这个struct eventpoll对象所关联的struct file放入进程打开的文件列表fd_array中管理

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%209.png)

- wait_queue_head_t wq：epoll中的等待队列，队列中存放的是阻塞在epoll上的用户进程，在IO就绪的时候epoll可以通过这个队列找到这些阻塞的进程并唤醒它们，从而执行IO调用读写socket上的数据
    
    

### epoll_ctl向epoll对象中添加socket

当调用epoll_create之后就可以使用epoll_ctl向epoll中添加socket连接：

- 首先在内核中创建一个表示socket连接的数据结构struct epitem，在epoll中采用了红黑树来存储这些结构（struct epitem就是一个红黑树节点）

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%2010.png)

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%2011.png)

- 在内核中创建完表示socket连接的数据结构struct epitem之后我们就需要在socket中的等待队列上创建等待项wait_queue_t并且注册epoll的回调函数ep_poll_callback

### epoll_wait同步阻塞获取IO就绪事件

- 用户程序调用epoll_wait之后，内核会查找epoll中的就绪队列eventpoll→rdllist是否有IO就绪的epitem，epitem中封装了socket的信息，如果就绪队列中有就绪的epitem，就将就绪的socket信息封装到epoll_event返回
- 如果eventpoll→rdllist就绪队列中没有IO就绪的epitem，则会创建等待项wait_queue_t，将用户进程的fd关联到wait_queue_t→private上，并在等待项wait_queue_t→func上注册回调函数default_wake_function，最后将等待项添加到epoll的等待队列中，用户进程让出CPU，进入阻塞状态

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%2012.png)

### 水平触发和边缘触发

在epoll工作流程中，当监听的socket上有数据到达时，软中断会执行epoll的回调函数ep_epoll_callback，在回调函数中会将epoll中描述socket信息的数据结构epitem插入到epoll中的就绪队列rdllist中，随后用户进程从epoll的等待队列中被唤醒，epoll_wait将IO就绪的socket返回给用户进程，随即epoll_wait会清空rdllist

所以水平触发和边缘触发最关键的点就在于是否清空rdllist：

- 水平触发：在这种模式下，用户线程调用epoll_wait获取IO就绪事件之后，对socket进行系统IO调用读取完数据，假设有socket中的数据没有一次性读取完，这时候再调用epoll_wait，会再次检查这些接收缓冲区中是否有数据可读，如果还有数据可读就会放回rdllist中
- 边缘触发：在这种模式下，epoll_wait会直接清空rdllist，不管socket上是否有数据可读，所以在边缘触发模式下，当你没有来得及处理socket接收缓冲区剩下的可读数据时，再次调用epoll_wait，因为这时候rdllist已经被清空了，所以socket不会再次从epoll_wait中返回

# Netty的IO模型

Reactor IO线程模型有三种：

- 单Reactor单线程
- 单Reactor多线程
- 主从Reactor多线程（Netty选用的）

![Untitled](linux%20%E7%BD%91%E7%BB%9CIO%208a8ee93197684244905668789f8c2e89/Untitled%2013.png)

- Reactor在Netty中是以group的形式出现的，Netty中将Reactor分成两组，一组是MainReactorGroup，一组是SubReactorGroup
- MainReactorGroup中通常只有一个Reactor，专门负责监听连接accept事件，当有连接事件产生时，在对应的处理handler中创建初始化相应的NioSocketChannel，然后以负载均衡的方式在SubReactorGroup中选取一个Reactor，注册上去监听Read事件