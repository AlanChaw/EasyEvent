
#include "Poller.h"
#include "Channel.h"
#include "EventLoop.h"

#include <assert.h>
#include <algorithm>

using namespace EasyEvent;

Poller::Poller(EventLoop* loop)
    : _ownerLoop(loop)
{}

Poller::~Poller() {}

muduo::Timestamp Poller::poll(int timeoutMs, ChannelList* activeChannels){
    int numEvents = ::poll(&(*_pollfds.begin()), _pollfds.size(), timeoutMs);   // EventLoop 会阻塞在这里
    muduo::Timestamp now(muduo::Timestamp::now());

    assert(numEvents >= 0);

    if(numEvents > 0){
        fillActiveChannels(numEvents, activeChannels);
    }

    return now;     // 返回 poll(2) return 的时刻
}

void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const{
    for(PollFdList::const_iterator pfd = _pollfds.begin(); 
        pfd != _pollfds.end() && numEvents > 0; 
        ++pfd){
        
        if(pfd->revents > 0){                       // 若当前 fd 有活跃事件
            --numEvents;                            // 为了提前跳出，不必遍历所有 Channel
            auto ch = _channelMap.find(pfd->fd);
            assert(ch != _channelMap.end());
            Channel* channel = ch->second;          // 从 map 中找到该 fd 对应的 Channel
            assert(channel->getFd() == pfd->fd);    // 找到的 Channel 的 fd，要跟当前活跃 fd 一致
            channel->set_revents(pfd->revents);
            activeChannels->push_back(channel);
        }
    }
}

void Poller::updateChannel(Channel* channel){
    assert(_ownerLoop->isInLoopThread());
    if(channel->getIndex() < 0){                       // 是一个新 Channel，做添加
        assert(_channelMap.find(channel->getFd()) == _channelMap.end());
        // 由 Channel 产生出对应的 pfd
        pollfd pfd;
        pfd.fd = channel->getFd();
        pfd.events = static_cast<short>(channel->getEvents());
        pfd.revents = 0;

        _pollfds.push_back(pfd);
        int idx = static_cast<int>(_pollfds.size() - 1);
        channel->setIndex(idx);
        _channelMap[pfd.fd] = channel;
    }else{                                          // 是一个旧 Channel，做更新
        assert(_channelMap.find(channel->getFd()) != _channelMap.end());
        assert(_channelMap[channel->getFd()] == channel);
        int idx = channel->getIndex();
        assert(0 <= idx && idx < static_cast<int>(_pollfds.size()));

        pollfd pfd = _pollfds[idx];
        assert(pfd.fd == channel->getFd() || pfd.fd == -1);
        pfd.events = static_cast<short>(channel->getEvents());
        pfd.revents = 0;
        if(channel->isNoneEvent()){     // 如果这个 Channel 没有事件，则 fd = -1
            pfd.fd = -1;
        }

    }
}

void Poller::removeChannel(Channel* channel){
    assert(_ownerLoop->isInLoopThread());
    assert(_channelMap.count(channel->getFd()));
    assert(_channelMap[channel->getFd()] == channel);
    assert(channel->isNoneEvent());         // 检查确认该 Channel 不再关注任何事件
    
    int index = channel->getIndex();
    assert(index >= 0 && index < static_cast<int>(_pollfds.size()));
    const pollfd pfd = _pollfds[index]; (void)pfd;
    assert(pfd.fd == -channel->getFd() - 1 && pfd.events == channel->getEvents());
    size_t n = _channelMap.erase(channel->getFd());
    assert(n == 1); (void)n;

    // 常用的 vector 删除元素方法，先看是否是末尾，如果不是末尾则换到末尾去，O(1)
    if(index == static_cast<int>(_pollfds.size()) - 1){
        _pollfds.pop_back();
    }else{
        int endFd = _pollfds.back().fd;
        std::iter_swap(_pollfds.begin()+index, _pollfds.end()-1);
        if(endFd < 0){
            endFd = -endFd-1;
        }
        _channelMap[endFd]->setIndex(index);    // 为换进来的重设 index
        _pollfds.pop_back();                    // 末尾删除
    }

}

