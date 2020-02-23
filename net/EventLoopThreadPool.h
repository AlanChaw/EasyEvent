#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "base/noncopyable.h"

namespace EasyEvent{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : private noncopyable{
public:
    typedef std::vector<std::shared_ptr<EventLoopThread>> ThreadVector;

    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();
    void setThreadNum(int numThreads) { _numThreads = numThreads; }
    void start();
    EventLoop* getNextLoop();

private:
    EventLoop* _baseLoop;
    bool _started;
    int _numThreads;
    int _next;
    ThreadVector _threads;
    std::vector<EventLoop*> _loops;
    // EventLoopThread 是堆上的对象，但 EventLoop 是其线程栈上的对象（函数内变量）
};


}