
#include "Channel.h"
#include "EventLoop.h"

#include <sstream>
#include <poll.h>

using namespace EasyEvent;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : _loopPtr(loop),
      _fd(fd),
      _events(0),
      _revents(0),
      _index(-1)
{}

void Channel::update(){
    _loopPtr->updateChannel(this);
}

void Channel::handleEvent(){
//   if (revents_ & POLLNVAL) {
//     LOG_WARN << "Channel::handle_event() POLLNVAL";
//   }

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

}
