
#include <assert.h>

#include "EventLoopThread.h"
#include "EventLoop.h"

using namespace EasyEvent;

EventLoopThread::EventLoopThread()
    : _loop(nullptr),
      _exiting(false),
      _thread(std::bind(&EventLoopThread::threadFunc, this)),
      _mtx(),
      _cond()
{}

EventLoopThread::~EventLoopThread(){
    _exiting = true;
    _loop->quitLoop();
    _thread.join();
}

// 使用条件变量的原因是：需要确保在线程和其中的 EventLoop 初始化成功后，才能返回其地址
EventLoop* EventLoopThread::startLoop(){
    assert(!_thread.started());
    _thread.start();

    std::unique_lock<std::mutex> lk(_mtx);      // 必须用互斥锁
    _cond.wait(lk, 
        [this]{ return (_loop != NULL); }
    );
    lk.unlock();

    return _loop;
}

void EventLoopThread::threadFunc(){
    EventLoop loop;

    {
        LockGuard lk(_mtx);
        _loop = &loop;
        _cond.notify_one();
    }

    loop.startLoop();
}

