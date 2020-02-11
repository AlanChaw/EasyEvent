#pragma once

#include <vector>
#include <atomic>
// #include <memory>

#include "base/Thread.h"
#include "base/noncopyable.h"

namespace EasyEvent{

class Channel;
class Poller;

class EventLoop : private noncopyable{

public:
    typedef std::vector<Channel*> ChannelList;

    EventLoop();
    ~EventLoop();

    void startLoop();
    void quitLoop();

    void updateChannel(Channel* channel);

    bool isInLoopThread() const { return _threadId == Thread::getCurrentThreadId();}

private:
    std::atomic<bool> _looping;
    std::atomic<bool> _quit;
    Thread::ThreadId _threadId;
    std::unique_ptr<Poller> _poller;
    ChannelList _activeChannels;
};

}
