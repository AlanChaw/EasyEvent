// 测试 Thread.h 的功能

#include <iostream>
// #include <thread>
#include "net/EventLoop.h"
#include "base/Thread.h"

void testFunc(int n){
    for(int i = 0; i < n; ++i)
        printf("%d \n", i);
}

int main(){

    EasyEvent::Thread thread1(std::bind(testFunc, 300), "main");
    EasyEvent::Thread thread2(std::bind(testFunc, 500), "main");

    thread1.start();
    // thread1.join();
    thread1.join();
    thread2.start();
    // thread2.join();
    thread2.join();
    // std::thread thread(testFunc, 3);
    // thread.join();
    std::cout << "total created: " << EasyEvent::Thread::numCreated() << std::endl;

    return 0;
}