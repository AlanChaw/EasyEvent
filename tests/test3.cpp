#include "net/Channel.h"
#include "net/EventLoop.h"

#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>
#include <unistd.h>

EasyEvent::EventLoop* g_loop;

void timeout()
{
  printf("Timeout!\n");
  g_loop->quitLoop();
}

int main()
{
  EasyEvent::EventLoop loop;
  g_loop = &loop;

  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  EasyEvent::Channel channel(&loop, timerfd);
  channel.setReadCallback(timeout);
  channel.enableReading();

  struct itimerspec howlong;
  bzero(&howlong, sizeof howlong);
  howlong.it_value.tv_sec = 5;
  ::timerfd_settime(timerfd, 0, &howlong, NULL);

  loop.startLoop();

  ::close(timerfd);
}
