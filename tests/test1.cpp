// EventLoop 功能测试

#include "net/EventLoop.h"
#include "base/Thread.h"
#include <stdio.h>

using namespace EasyEvent;

void threadFunc(){
    printf("threadFunc(): pid = %d\n", int(std::hash<std::thread::id>{}(std::this_thread::get_id())));

    EventLoop loop;
    loop.startLoop();
}

int main(){
    printf("main(): pid = %d\n", int(std::hash<std::thread::id>{}(std::this_thread::get_id())));

    EventLoop loop;

    Thread thread(threadFunc);
    thread.start();

    loop.startLoop();

    return 0;
}