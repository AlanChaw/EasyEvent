set(base_SRCS
  # temp.cpp
  Thread.cpp
  Timestamp.cc
  CapsuledAddr.cpp 
  Socket.cpp 
#   AsyncLogging.cc
#   Condition.cc
#   CountDownLatch.cc
#   CurrentThread.cc
#   Date.cc
#   Exception.cc
#   FileUtil.cc
#   LogFile.cc
#   Logging.cc
#   LogStream.cc
#   ProcessInfo.cc
#   Timestamp.cc
#   Thread.cc
#   ThreadPool.cc
#   TimeZone.cc
  )

add_library(EasyEvent_base ${base_SRCS})
# target_link_libraries(muduo_base pthread rt)

# install(TARGETS muduo_base DESTINATION lib)

# file(GLOB HEADERS "*.h")
# install(FILES ${HEADERS} DESTINATION include/muduo/base)

# if(MUDUO_BUILD_EXAMPLES)
#   add_subdirectory(tests)
# endif()
