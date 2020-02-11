#pragma once

#include "noncopyable.h"

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>


namespace EasyEvent{

class Thread : private noncopyable{

public:
    typedef std::function<void ()> ThreadFunc;
    typedef std::thread::id ThreadId;

    explicit Thread(ThreadFunc func, const std::string& name = std::string());
    ~Thread();
    
    void start();
    void join();

    bool started() const            { return _started; }
    ThreadId getId() const          { return _threadId; }
    const std::string& name() const { return _name; }

    static int numCreated()         { return _numCreated; }

    static inline ThreadId getCurrentThreadId() { return std::this_thread::get_id(); }

private:
    bool _started;
    bool _joined;
    std::thread _thread;
    ThreadId _threadId;
    ThreadFunc _func;
    std::string _name;

    static std::atomic<int> _numCreated;

};

}