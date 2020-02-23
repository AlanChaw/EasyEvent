#include "EventLoopThreadPool.h"

#include <assert.h>

#include "EventLoop.h"
#include "EventLoopThread.h"

using namespace EasyEvent;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
    : _baseLoop(baseLoop), 
      _started(false),
      _numThreads(0),       // 0个线程数表示用 TcpServer 自身线程来执行 IO
      _next(0) {}

EventLoopThreadPool::~EventLoopThreadPool()
{
    // EventLoop* 指向的是栈对象，不要delete
}

void EventLoopThreadPool::start(){
    assert(!_started);
    assert(_baseLoop->isInLoopThread());

    _started = true;
    printf("start begin\n");
    for(int i = 0; i < _numThreads; ++i){
        // std::shared_ptr<EventLoopThread> t = std::make_shared<EventLoopThread>();
        EventLoopThread* t = new EventLoopThread;
        _threads.push_back(std::shared_ptr<EventLoopThread>(t));
        _loops.push_back(t->startLoop());
    }
    printf("start finished\n");
}

EventLoop* EventLoopThreadPool::getNextLoop(){
    assert(_baseLoop->isInLoopThread());
    EventLoop* loop = _baseLoop;

    if(!_loops.empty()){
        loop = _loops[_next++];
        if(static_cast<size_t>(_next) >= _loops.size()){
            _next = 0;
        }
    }
    return loop;
}
