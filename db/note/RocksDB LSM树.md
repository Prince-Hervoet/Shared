<a name="PBLDF"></a>
# 什么是LSM存储
![](https://cdn.nlark.com/yuque/0/2022/webp/22646069/1665235479257-5e94286c-89ea-4a74-b0a0-1cc370988b5c.webp#averageHue=%23f0f5f0&clientId=ub33604ca-f890-4&from=paste&id=ue16e6a7b&originHeight=472&originWidth=751&originalType=url&ratio=1&rotation=0&showTitle=false&status=done&style=none&taskId=u45e80daf-a319-49ec-b31b-5d744dd8341&title=)<br />RocksDB三种基本文件格式：memtable、WAL、SSTable：

<a name="Mg7Yc"></a>
## memtable

- memtable：这个是RocksDB内存中维护的一个数据结构，同时服务于读和写，数据在写入时总会被写到active memtable中，执行查询时先查询memtable（内存中的数据是比较新的）。RocksDB中的memtable采用了跳表数据结构，支持范围查询和天然有序
- 当一个active memtable被写满时，会被置为只读状态，变成immutable memtable，然后创建一个新的active memtable来提供写入。immutable memtable会在内存中保留，等待后台线程对其进行flush操作，flush的触发条件是immutable memtable数量超过阈值后，flush会一次性将immutable memtable合并压缩到磁盘的L0中，合并压缩完后内存中的memtable会被删除

<a name="tSOVR"></a>
## WAL机制
WAL是一种有利于顺序写的持久化日志文件，很多存储系统中都有类似的设计，RocksDB每次写数据都会先写入WAL再写入memtable，在发生故障时，通过重放WAL恢复内存中的数据，保证数据一致性

至于WAL的创建删除时机，每次flush一个CF后，都会新建一个WAL，但是这并不以为着旧的WAL文件会被删除，因为别的CF数据可能还没有落盘，只有所有数据都被flush到磁盘上才会删除相关的WAL
> CF：Column Family，RocksDB3.0后增加的一个东西。RocksDB允许用户创建多个CF，每一个CF各自拥有独立的memtable以及sst文件，但是共享一个WAL文件，这样是为了可以根据应用特点为不同的CF选择不同的配置，但是又不增加对WAL的写次数


<a name="xHjuN"></a>
## SSTable
SSTable是存在于磁盘上，是一个持久化的、有序的、不可更改的Map结构，Key和Value都是任意的Byte串<br />![](https://cdn.nlark.com/yuque/0/2022/webp/22646069/1665236368012-2eabfb24-3571-45bb-91e1-12e0cf143392.webp#averageHue=%23ead474&clientId=ub33604ca-f890-4&from=paste&id=ua8824ce0&originHeight=590&originWidth=768&originalType=url&ratio=1&rotation=0&showTitle=false&status=done&style=none&taskId=u25ea0575-aba8-4eb4-9699-8426bac8532&title=)

- 数据块：存储有序键值对，是SSTable中的数据实体，为了节省存储空间，并不会为每一对k-v都存储完整的key值，而是存储与上一个key非共享的部分，避免key重复内容的存储
- Meta Block：存储Filter相关信息，用于加快sst中的查询数据的效率，Filter通过布隆过滤器来快速判断指定的block中是否存在key
- Meta Index Block：对Meta Block的索引，它只有一条记录，key是meta index的名字，value为指向meta index的位置
- Index Block：index block用来存储所有的data block的相关索引信息，index block包含若干条记录，每一条记录代表一个data block的索引信息
- Footer，指向各个分区的位置和大小，Footer是定长的，读取sst文件的时候，就是从文件末尾，固定读取字节数，进而得到了Footer信息，Footer中的信息指明了MetaIndexBlock和IndexBlock的位置

<a name="YvD8k"></a>
# 读和写操作
<a name="Dm0gG"></a>
## 读操作
![](https://cdn.nlark.com/yuque/0/2022/webp/22646069/1665236924740-a9415652-ddbc-4f5b-91b6-ddef9747ce9f.webp#averageHue=%23f2f1e7&clientId=ub33604ca-f890-4&from=paste&id=ufcd58377&originHeight=583&originWidth=738&originalType=url&ratio=1&rotation=0&showTitle=false&status=done&style=none&taskId=u8ad39f4e-6b91-4582-b967-fa41f7c74d4&title=)

- 优先在active memtable中查找
- 如果active memtable没有，则在immutable memtable中查找
- 如果immutable memtable没有，则在L0sst中查找
- 如果找不到，则在剩余的层中查找

RocksDB 为 SST 中访问频繁的 data blocks 设置了一个读缓存结构 [Block cache](https://link.juejin.cn/?target=https%3A%2F%2Fgithub.com%2Ffacebook%2Frocksdb%2Fwiki%2FBlock-Cache)，并提供了两种开箱即用的实现 LRUCache 和 ClockCache 
<a name="lfmOm"></a>
## 写操作
![](https://cdn.nlark.com/yuque/0/2022/webp/22646069/1665237607942-60ce26da-6135-4996-bc83-312b9b791a42.webp#averageHue=%23f2f0e2&clientId=ub33604ca-f890-4&from=paste&id=u73647925&originHeight=391&originWidth=742&originalType=url&ratio=1&rotation=0&showTitle=false&status=done&style=none&taskId=u124a8470-3599-462f-8b5b-7953bc2e6ab&title=)

- 写操作会先写到WAL文件
- 完成WAL写入后，将数据写入到内存中的数据结构
- 然后等memtable数据量达到阈值后，转变成不可变结构，同时生成新的memtable提供服务
- 在满足条件之后将不可变结构刷入磁盘
> 默认情况下RocksDB中的写磁盘行为都是异步写，仅仅将数据写进了缓存区，而这些数据写进磁盘是一个异步的过程。异步写的话吞吐量是同步写的一千多倍。异步写的缺点是机器或者操作系统崩溃可能会丢失最近一批写请求的缓存，但是由于在程序代码执行正常和操作系统正常运行的情况下崩溃的概率比较低，所以大部分情况下是安全的


<a name="cFXsK"></a>
# 合并操作
LSM树将离散的随机写请求都转化成批量的顺序写，以此提高写性能，但是这样也会出现问题：

- 读放大：读操作有可能会访问大量文件
- 空间放大：所有写入都是顺序写的，不是在对数据进行直接更新，所以过期数据不会马上被删除

所以维护和减少sst文件的数目是很必要的，Rocks会根据不同的配置进行Compaction操作，Compaction操作会将过期或者已经删除的key进行清理合并来提高查询效率

![](https://cdn.nlark.com/yuque/0/2022/webp/22646069/1665238027749-6f2dcf29-0c62-4893-b8a7-1385d72693a5.webp#averageHue=%23171717&clientId=ub33604ca-f890-4&from=paste&id=u5a273272&originHeight=514&originWidth=1024&originalType=url&ratio=1&rotation=0&showTitle=false&status=done&style=none&taskId=u924f8fdc-dfc9-494a-a33d-1b18f2296cf&title=)

> Level Style Compaction

默认情况下RocksDB采用这种策略进行合并。这种策略保证L0存储的是最新的数据，越往后面的层级存储的数据越老。每个Compaction任务会选择一个相邻的文件进行合并，然后添加到下一层中，并删除当前已经合并的文件

> Universal Compaction

只压缩L0的文件，合并后再放入L0层

> FIFO Compaction

这种模式周期性地删除旧数据，在FIFO模式下所有文件都在L0，当sst文件总大小超过compaction_options_fifo.max_table_files_size，则删除最老的sst文件，对于FIFO来说，它的策略非常简单，所有sst都在L0，如果超过了阈值就从最老的开始删除

<a name="Jh1fq"></a>
# LSM树设计思想
LSM树将对磁盘的随机写入转化为了磁盘友好型的顺序写，从而大大提高了写性能。核心就是内存中维护一个有序的内存表，当内存表大于阈值的时候批量刷入磁盘，生成最新的sstable文件，因为本身memtable已经维护了按键排序的键值对

为了优化读性能，同时优化存储空间，LSM树通过在运行合并和压缩过程减少SSTable文件数量，删除无效的旧值，这一过程被称为Compaction。但是合并操作也会有一些影响，就是在压缩过程中可能会干扰到正在进行的读写请求

> LSM树 vs B+树
> 虽然像LSM树一样，B+树保持按键排序的键值对，但是这两种设计理念完全不同：
> - LSM树将数据库分解可变大小的段，并且总是按照顺序编写段
> - 相比之下，B+树数据库分解成固定大小的块或者页面，传统大小为4KB

> LSM树 vs B+树的操作
> - B+树可以原地更新和删除数据，这种方式对于数据库事务支持更加友好，因为一个key只会出现一个page页里面
> - 但是由于LSM树只能追加写，并且在L0层的sst中会重叠，所以对事务支持较弱，只能在compaction的时候进行真正地更新和删除

> LSM树 vs B+树的操作
> - LSM树的优点是支持高吞吐量的写，读取是O(N)复杂度
> - B+树的特点是支持高效读，但是在大规模的下请求下，效率会明显降低这是由于写操作会使得节点不断的分裂和合并

