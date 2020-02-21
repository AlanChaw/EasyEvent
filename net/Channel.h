#pragma once

#include <functional>
#include "base/noncopyable.h"
#include "base/Timestamp.h"

namespace EasyEvent{

class EventLoop;

// 每个 Channel 负责处理一个 socket，也就是一个文件描述符，但不拥有该 fd
class Channel : private noncopyable{

public:
    typedef std::function<void()> EventCallback;    // 用户传入的回调函数
    typedef std::function<void(muduo::Timestamp)> ReadEventCallback;

    Channel(EventLoop* loop, int fd);               // 每个 Channel 只属于一个 EventLoop
    ~Channel();

    // Channel 负责根据 fd 的变化来处理客户传入的回调函数
    // 这里是核心部分，相当于对 IO 进行分发(dispatch)
    void handleEvent(muduo::Timestamp receiveTime);
    void setReadCallback(const ReadEventCallback& cb){
        _readCallback = cb;
    }
    void setWriteCallback(const EventCallback& cb){
        _writeCallback = cb;
    }
    void setErrorCallback(const EventCallback& cb){
        _errorCallback = cb;
    }
    void setCloseCallback(const EventCallback& cb){
        _closeCallback = cb;
    }

    int getFd() const { return _fd; }
    int getEvents() const { return _events;}
    void set_revents(int revt) { _revents = revt; }
    bool isNoneEvent() const { return _events == kNoneEvent; }

    void enableReading() { _events = (_events | kReadEvent); update(); }

    void disableAll(){ _events = kNoneEvent; update(); }

    // 给 Poller 使用
    int getIndex() const { return _index; }
    void setIndex(int idx) { _index = idx; }

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

    bool _eventHandling;    // 是否正在处理事件，如果是则该对象不可被析构

    // 实际上，Channel 并不直接保存系统库<poll.h>中的 struct ::pollfd
    // 因为它并不关心 Poller 用的是哪种 poll 方法，所以以后换用 epoll 之类的方法会更灵活
    ReadEventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _errorCallback;
    EventCallback _closeCallback;
};

}
