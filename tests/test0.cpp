#include <iostream>
#include "net/EventLoop.h"

int main(){
    EventLoop loop;
    loop.printInfo();
    std::cout << "this is test 0" << std::endl;

    return 0;
}