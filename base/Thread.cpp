
#include "Thread.h"

#include <assert.h>

using namespace EasyEvent;

std::atomic<int> Thread::_numCreated;

Thread::Thread(ThreadFunc func, const std::string& name)
    : _started(false),
      _joined(false),
      _thread(),
      _threadId(0),
      _func(std::move(func)),
      _name(name)
{
    ++_numCreated;
}

Thread::~Thread(){
    if(_started && !_joined){
        _thread.detach();        
    }
}

void Thread::start(){
    assert(!_started);
    _started = true;
    _thread = std::thread(_func);
}

void Thread::join(){
    assert(_started);
    assert(_thread.joinable());
    _joined = true;
    _thread.join();
}
