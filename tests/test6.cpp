#include "net/EventLoop.h"
#include "net/EventLoopThread.h"
#include <stdio.h>

#include <unistd.h>

#define GET_ID int(std::hash<std::thread::id>{}(std::this_thread::get_id()))

void runInThread()
{
  printf("runInThread(): tid = %d \n", GET_ID);
}

int main()
{
  printf("main(): tid = %d\n", GET_ID);

  EasyEvent::EventLoopThread loopThread;
  EasyEvent::EventLoop* loop = loopThread.startLoop();
  loop->runInLoop(runInThread);
  sleep(1);
//   loop->runInLoop(runInThread);
//   sleep(1);
  loop->quitLoop();     // 这里会调用 wakeup，因为 loop 在  EventLoopThread中，不在当前主线程中
//   sleep(1);
  printf("exit main().\n");

//   return 0;
}
