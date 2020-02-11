#pragma once

#include <condition_variable>
#include <mutex>

#include "base/Thread.h"
#include "base/noncopyable.h"

namespace EasyEvent{

class EventLoop;

class EventLoopThread : private noncopyable{

public:
    typedef std::lock_guard<std::mutex> LockGuard;

    EventLoopThread();
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();

    EventLoop* _loop;
    bool _exiting;
    Thread _thread;
    std::mutex _mtx;
    std::condition_variable _cond;
};

}