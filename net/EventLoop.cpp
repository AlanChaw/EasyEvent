
#include "EventLoop.h"

#include <assert.h>
#include <poll.h>
#include <memory>

using namespace EasyEvent;

__thread EventLoop* t_loopInThisThread = NULL;

EventLoop::EventLoop()
    : _looping(false),
      _threadId(Thread::getCurrentThreadId())
{
    assert(!t_loopInThisThread);            // 每个线程只能有一个 EventLoop
    t_loopInThisThread = this;
}

EventLoop::~EventLoop(){
    assert(!_looping);
    t_loopInThisThread = NULL;
}

void EventLoop::startLoop(){
    assert(!_looping);
    assert(isInLoopThread());
    
    _looping = true;

    ::poll(NULL, 0, 5*1000);

    _looping = false;
}
