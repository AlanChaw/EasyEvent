#include <iostream>
#include "base/Socket.h"
#include <unistd.h>

using namespace EasyEvent;

int main() {
    std::cout << "This is server" << std::endl;
    // socket
    int socketFd = Socket::createNonblockingOrDie();
    Socket socket(socketFd);
    CapsuledAddr addr(2000);
    socket.bindAddress(addr);
    socket.listen();

    CapsuledAddr peerAddr(0);

    while(true){
        std::cout << "...listening" << std::endl;
        int connfd = socket.accept(&peerAddr);
        if(connfd < 0){
            std::cout << "no connected" << std::endl;
        }else{
            std::cout << "connected, peer address: "
                    << peerAddr.toString() << std::endl; 
        }

        sleep(1);
    }

    return 0;
}