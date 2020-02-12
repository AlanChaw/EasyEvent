#pragma once

#include <vector>
#include <atomic>
#include <mutex>

#include "base/Thread.h"
#include "base/noncopyable.h"
#include "base/Timestamp.h"


namespace EasyEvent{

class Channel;
class Poller;

class EventLoop : private noncopyable{

public:
    typedef std::vector<Channel*> ChannelList;
    typedef std::function<void()> Functor;
    typedef std::lock_guard<std::mutex> LockGuard;

    EventLoop();
    ~EventLoop();

    void startLoop();
    void quitLoop();

    muduo::Timestamp pollReturnTime() const { return _pollReturnTime; }

    // 可安全地跨线程调用
    void runInLoop(const Functor& cb);
    void queueInLoop(const Functor& cb);

    // 内部使用
    void wakeup();
    void updateChannel(Channel* channel);

    bool isInLoopThread() const { return _threadId == Thread::getCurrentThreadId();}

    // TODO
    // void runAt();
    // void runAfter();
    // void runEvery();

private:
    void handleRead();
    void doPendingFunctors();

private:
    std::atomic<bool> _looping;
    std::atomic<bool> _quit;
    std::atomic<bool> _callingPendingFunctors;

    Thread::ThreadId _threadId;
    std::unique_ptr<Poller> _poller;
    muduo::Timestamp _pollReturnTime;

    int _wakeupFd;
    std::unique_ptr<Channel> _wakeupChannel;
    ChannelList _activeChannels;

    std::mutex _mtx;
    std::vector<Functor> _pendingFunctors;
};

}
