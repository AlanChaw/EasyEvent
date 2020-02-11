#pragma once

#include <functional>
#include "base/noncopyable.h"


namespace EasyEvent{

class EventLoop;

class Channel : private noncopyable{

public:
    typedef std::function<void()> EventCallback;    // 用户传入的回调函数

    Channel(EventLoop* loop, int fd);               // 每个 Channel 只属于一个 EventLoop
    ~Channel();

    void handleEvent();
    void setReadCallback(const EventCallback& cb){
        _readCallback = cb;
    }
    void setWriteCallback(const EventCallback& cb){
        _writeCallback = cb;
    }
    void setErrorCallback(const EventCallback& cb){
        _errorCallback = cb;
    }

    int getFd() const { return _fd; }
    int getEvents() const { return _events;}
    void set_revents(int revt) { _revents == revt; }
    bool isNoneEvent() const { return _events == kNoneEvent; }

    void enableReading() { _events = (_events | kReadEvent); update(); }

    // 给 Poller 使用
    int index() const { return _index; }
    void set_index(int idx) { _index = idx; }

    EventLoop* getOwnerLoop() { return _loopPtr; }

private:
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* _loopPtr;
    const int _fd;
    int _events;        // 一共有多少种事件
    int _revents;       // 当前有多少种活跃事件
    int _index;         // 由 Poller 使用，该 Channel 在 Poller 数组中的下标，便于快速定位

    // 实际上，Channel 并不直接保存系统库<poll.h>中的 struct ::pollfd
    // 因为它并不关心 Poller 用的是哪种 poll 方法，所以以后换用 epoll 之类的方法会更灵活

    EventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _errorCallback;
};

}
