#pragma once

#include <vector>
#include <atomic>
// #include <memory>

#include "base/Thread.h"
#include "base/noncopyable.h"
#include "Channel.h"

namespace EasyEvent{

class EventLoop : private noncopyable{

public:
    typedef std::vector<Channel*> ChannelList;

    EventLoop();
    ~EventLoop();

    void startLoop();
    void quitLoop();

    bool isInLoopThread() const { return _threadId == Thread::getCurrentThreadId();}

private:
    bool _looping;
    bool _quit;
    Thread::ThreadId _threadId;
    std::unique_ptr<Poller> _poller;
    ChannelList _activeChannels;    
};

}
