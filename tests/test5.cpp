#include "net/EventLoop.h"
#include <stdio.h>

EasyEvent::EventLoop* g_loop;
int g_flag = 0;

void run4()
{
  printf("run4(): pid = %d, flag = %d\n", int(std::hash<std::thread::id>{}(std::this_thread::get_id())), g_flag);
  g_loop->quitLoop();
}

// void run3()
// {
//   printf("run3(): pid = %d, flag = %d\n", EasyEvent::Thread::getCurrentThreadId(), g_flag);
//   g_loop->runAfter(3, run4);
//   g_flag = 3;
// }

void run2()
{
  printf("run2(): pid = %d, flag = %d\n", int(std::hash<std::thread::id>{}(std::this_thread::get_id())), g_flag);
    g_loop->queueInLoop(run4);
}

void run1()
{
  g_flag = 1;
  printf("run1(): pid = %d, flag = %d\n", int(std::hash<std::thread::id>{}(std::this_thread::get_id())), g_flag);
  g_loop->runInLoop(run2);
  g_flag = 2;
}

int main()
{
  printf("main(): pid = %d, flag = %d\n", int(std::hash<std::thread::id>{}(std::this_thread::get_id())), g_flag);

  EasyEvent::EventLoop loop;
  g_loop = &loop;

//   loop.runAfter(2, run1);
  loop.runInLoop(run1);
  loop.startLoop();

  printf("main(): pid = %d, flag = %d\n", int(std::hash<std::thread::id>{}(std::this_thread::get_id())), g_flag);
}
