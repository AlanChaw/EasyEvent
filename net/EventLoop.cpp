
#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"

#include <assert.h>
#include <memory>

#include <sys/eventfd.h>
#include<unistd.h>
#include <poll.h>

using namespace EasyEvent;

__thread EventLoop* t_loopInThisThread = NULL;
const int kPollTimeMs = 5000;

static int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  assert(evtfd >= 0);
  return evtfd;
}

/************************* 构造/析构 ************************/
EventLoop::EventLoop()
    : _looping(false),
      _quit(false),
      _callingPendingFunctors(false),
      _threadId(Thread::getCurrentThreadId()),
      _poller(new Poller(this)),
      _wakeupFd(createEventfd()),
      _wakeupChannel(new Channel(this, _wakeupFd))
{
    assert(!t_loopInThisThread);            // 每个线程只能有一个 EventLoop
    t_loopInThisThread = this;
    _wakeupChannel->setReadCallback(
        std::bind(&EventLoop::handleRead, this)
    );
    _wakeupChannel->enableReading();
}

EventLoop::~EventLoop(){
    assert(!_looping);
    ::close(_wakeupFd);
    t_loopInThisThread = NULL;
}

/************************* 开始/终止 ************************/
void EventLoop::startLoop(){
    assert(!_looping);
    assert(isInLoopThread());
    
    _looping = true;
    _quit = false;

    while(!_quit){
        _activeChannels.clear();
        _poller->poll(kPollTimeMs, &_activeChannels);
        for(auto iter = _activeChannels.begin(); iter != _activeChannels.end(); ++iter){
            (*iter)->handleEvent();
        }
        doPendingFunctors();
    }

    _looping = false;
}

void EventLoop::quitLoop(){
    _quit = true;
    if(!isInLoopThread()){
        wakeup();       // 起来！该干活了！
    }
}

/************************* 跨线程调用 ************************/
void EventLoop::runInLoop(const Functor& cb){
    if(isInLoopThread()){
        cb();
    }else{
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor& cb){
    {
        LockGuard lk(_mtx);
        _pendingFunctors.push_back(cb);
    }

    if(!isInLoopThread() || _callingPendingFunctors){
        wakeup();
    }
}


void EventLoop::updateChannel(Channel* channel){
    assert(channel->getOwnerLoop() == this);
    assert(isInLoopThread());

    _poller->updateChannel(channel);
}

// 向 _wakeupFd 写一下以唤醒_wakeupChannel，从而唤醒 EventLoop
void EventLoop::wakeup(){
    uint64_t one = 1;
    ssize_t n = ::write(_wakeupFd, &one, sizeof one);

    printf("wake up fd: %d \n", _wakeupFd);
    printf("size of one: %d\n", int(sizeof one));
    printf("n: %d \n", int(n));

    assert(n == sizeof one);
}


/************************* private  ************************/
void EventLoop::handleRead(){
    uint64_t one = 1;
    ssize_t n = ::read(_wakeupFd, &one, sizeof one);
    assert(n == sizeof one);
}

void EventLoop::doPendingFunctors(){
    std::vector<Functor> functors;
    _callingPendingFunctors = true;
    
    // 换出之后慢慢执行，不长时间持有锁。 相当于move-on-write
    {
        LockGuard lk(_mtx);
        functors.swap(_pendingFunctors);
    }

    // 按顺序执行所有排队的函数
    for(size_t i = 0; i < functors.size(); ++i){
        functors[i]();
    }

    _callingPendingFunctors = false;
}
