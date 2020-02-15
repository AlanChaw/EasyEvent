
#include "Channel.h"
#include "EventLoop.h"

#include <sstream>
#include <poll.h>
#include <assert.h>

using namespace EasyEvent;

// 表示没有绑定的事件
const int Channel::kNoneEvent = 0;
// 表示可读事件
const int Channel::kReadEvent = POLLIN | POLLPRI;
// 表示可写事件
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : _loopPtr(loop),
      _fd(fd),
      _events(0),
      _revents(0),
      _index(-1),
      _eventHandling(false)
{}

// 要确保在 Channel 处理事件的时候，自身不会被突然析构
// 例如，用户关闭连接，Channel 要处理该事件，但同时用户可能会析构 Channel
// 这时很可能会 core dump
Channel::~Channel(){
    assert(!_eventHandling);
}

void Channel::update(){
    printf("update channel, fd: %d \n", _fd);
    _loopPtr->updateChannel(this);
}

void Channel::handleEvent(){
//   if (revents_ & POLLNVAL) {
//     LOG_WARN << "Channel::handle_event() POLLNVAL";
//   }
    _eventHandling = true;
    // 处理关闭事件
    if((_revents & POLLHUP) && !(_revents & POLLIN)){
        if(_closeCallback){
            _closeCallback();
        }
    }

    // 如果有活跃事件而且是 ERR 或 NVAL 中的一种
    if(_revents & (POLLERR | POLLNVAL)){        
        if(_errorCallback){
            _errorCallback();
        }
    }
    if(_revents & (POLLIN | POLLPRI | POLLRDHUP)){
        if(_readCallback){
            _readCallback();
        }
    }
    if(_revents & POLLOUT){
        if(_writeCallback){
            _writeCallback();
        }
    }

    _eventHandling = false;     // 最后记得要重置

}
