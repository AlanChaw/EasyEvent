// EventLoop 负面测试

#include "net/EventLoop.h"
#include "base/Thread.h"

using namespace EasyEvent;

EventLoop* g_loop;

void threadFunc(){
    g_loop->startLoop();
}

int main(){
    EventLoop loop;
    g_loop = &loop;

    Thread t(threadFunc);   // 在主线程创建 loop，但是尝试在子线程执行
    t.start();
    t.join();
}

