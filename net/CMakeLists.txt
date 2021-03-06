# include(CheckFunctionExists)

# check_function_exists(accept4 HAVE_ACCEPT4)
# if(NOT HAVE_ACCEPT4)
#   set_source_files_properties(SocketsOps.cc PROPERTIES COMPILE_FLAGS "-DNO_ACCEPT4")
# endif()

set(net_SRCS
#   Acceptor.cc
#   Buffer.cc
#   Channel.cc
#   Connector.cc
  EventLoop.cpp
  Poller.cpp
  Channel.cpp
  EventLoopThread.cpp
  Acceptor.cpp 
  TcpConnection.cpp 
  TcpServer.cpp 
  EventLoopThreadPool.cpp
  UdpHandler.cpp 
  UdpServer.cpp 
#   EventLoopThread.cc
#   EventLoopThreadPool.cc
#   InetAddress.cc
#   Poller.cc
#   poller/DefaultPoller.cc
#   poller/EPollPoller.cc
#   poller/PollPoller.cc
#   Socket.cc
#   SocketsOps.cc
#   TcpClient.cc
#   TcpConnection.cc
#   TcpServer.cc
#   Timer.cc
#   TimerQueue.cc
  )

add_library(EasyEvent_net ${net_SRCS})
target_link_libraries(EasyEvent_net EasyEvent_base)

# install(TARGETS muduo_net DESTINATION lib)

# set(HEADERS
#   Buffer.h
#   Callbacks.h
#   Channel.h
#   Endian.h
  # EventLoop.h
#   EventLoopThread.h
#   EventLoopThreadPool.h
#   InetAddress.h
#   TcpClient.h
#   TcpConnection.h
#   TcpServer.h
#   TimerId.h
  # )
# install(FILES ${HEADERS} DESTINATION include/muduo/net)

# add_subdirectory(http)
# add_subdirectory(inspect)

# if(MUDUO_BUILD_EXAMPLES)
#   add_subdirectory(tests)
# endif()

# if(PROTOBUF_FOUND)
#   add_subdirectory(protobuf)
#   add_subdirectory(protorpc)
# else()
#   add_subdirectory(protobuf EXCLUDE_FROM_ALL)
#   add_subdirectory(protorpc EXCLUDE_FROM_ALL)
# endif()
