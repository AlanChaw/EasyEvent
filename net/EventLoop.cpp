
#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"

#include <assert.h>
#include <poll.h>
#include <memory>

using namespace EasyEvent;

__thread EventLoop* t_loopInThisThread = NULL;
const int kPollTimeMs = 5000;

EventLoop::EventLoop()
    : _looping(false),
      _quit(false),
      _threadId(Thread::getCurrentThreadId()),
      _poller(new Poller(this))
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
    _quit = false;

    while(!_quit){
        _activeChannels.clear();
        _poller->poll(kPollTimeMs, &_activeChannels);
        for(auto iter = _activeChannels.begin(); iter != _activeChannels.end(); ++iter){
            (*iter)->handleEvent();
        }
    }

    _looping = false;
}

void EventLoop::quitLoop(){
    _quit = true;
}

void EventLoop::updateChannel(Channel* channel){
    assert(channel->getOwnerLoop() == this);
    assert(isInLoopThread());

    _poller->updateChannel(channel);
}
