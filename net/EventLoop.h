#include <iostream>

class EventLoop{
public:
    EventLoop(){}
    ~EventLoop(){}

    void printInfo(){
        std::cout << "event loop print info" << std::endl;
    }
};
