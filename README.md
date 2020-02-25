# EasyEvent

### 介绍

- EasyEvent 是一个基于事件驱动的 C++ 多线程网络库。 根据陈硕《muduo》一书提供的设计思路实现，并加以一定的修改和完善。

- 同时支持 TCP 和 UDP。 

- 使用了可自扩容的环形队列做应用层缓冲，减少了写入时可能的内存拷贝操作（内部腾挪）。

- 仅基于 C++11 实现，未使用 boost 库，便于安装使用。

- 尽量使用了 C++ STL 提供的原生的线程库和线程同步原语（如 thread、lock_guard、condition_variable）等，未进行过多封装，未使用 Linux POSIX thread 线程库，便于移植。


### Todo list

- 完善定时器功能 （使用最小堆实现）。

- 设计单独的日志线程和 I/O 缓冲，完善日志功能。

- UDP 模式目前能够支持多线程并发，但由于应用层缓冲的设计以及 UDP Socket 的一些限制，使其暂不能支持非阻塞 I/O。将来可以加以改进。

- 添加更多 Examples。

- 目前仅支持 Linux 操作系统，可以尝试移植。
