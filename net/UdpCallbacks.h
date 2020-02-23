#pragma once

#include <memory>

namespace EasyEvent{

class CapsuledAddr;
class UdpServer;

typedef std::shared_ptr<UdpServer> UdpServerPtr;

typedef std::function<void ()> ConnectionCallback;
typedef std::function<void (UdpServer* server, char* msg, int BUFFLEN, CapsuledAddr& peerAddr)> MessageReceivedCallback;
typedef std::function<void ()> MessageSentCallback;

// typedef std::function<void (UdpServerPtr& server, char* msg, int BUFFLEN, CapsuledAddr& peerAddr)> testCallback;

}