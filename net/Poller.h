#pragma once

#include <unordered_map>
#include <vector>

#include "base/Timestamp.h"
#include "base/noncopyable.h"
#include <poll.h>

namespace EasyEvent{

class Channel;
class EventLoop;

class Poller : private noncopyable{
public:
    typedef std::vector<Channel*> ChannelList;

    explicit Poller(EventLoop* loop);
    ~Poller();

    // Poll 所有的 IO 事件，需要在 loop 所在的线程调用
    muduo::Timestamp poll(int timeoutMs, ChannelList* activeChannels);   

    // 改变该 Poller 所关注的 Channel，需要在 loop 所在线程调用
    void updateChannel(Channel* channel);
    // 当连接关闭时，不再关注此 Channel，将其删除
    void removeChannel(Channel* channel);

private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    typedef std::vector<pollfd> PollFdList;
    typedef std::unordered_map<int, Channel*> ChannelMap;     // 从 fd 到 Channel 的映射

    EventLoop* _ownerLoop;
    PollFdList _pollfds;
    ChannelMap _channelMap;
};

}