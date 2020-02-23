# add_executable(test0 test0.cpp)
# target_link_libraries(test0 EasyEvent_net)

# add_executable(test1 test1.cpp)
# target_link_libraries(test1 EasyEvent_net)

# add_executable(test2 test2.cpp)
# target_link_libraries(test2 EasyEvent_net)

# add_executable(test3 test3.cpp)
# target_link_libraries(test3 EasyEvent_net)


# add_executable(test5 test5.cpp)
# target_link_libraries(test5 EasyEvent_net)

# add_executable(test6 test6.cpp)
# target_link_libraries(test6 EasyEvent_net)

# add_executable(testTCPSocket testTCPSocket.cpp)
# target_link_libraries(testTCPSocket EasyEvent_net)

# add_executable(testTCPSocketClient testTCPSocketClient.cpp)
# target_link_libraries(testTCPSocketClient EasyEvent_net)

# add_executable(testAddrConverter testAddrConverter.cpp)
# target_link_libraries(testAddrConverter EasyEvent_net)

# add_executable(testSocketWrapper testSocketWrapper.cpp)
# target_link_libraries(testSocketWrapper EasyEvent_net)

# add_executable(testAcceptor testAcceptor.cpp)
# target_link_libraries(testAcceptor EasyEvent_net)

# add_executable(testTcpServer testTcpServer.cpp)
# target_link_libraries(testTcpServer EasyEvent_net)

# add_executable(testConnClose testConnClose.cpp)
# target_link_libraries(testConnClose EasyEvent_net)

# add_executable(Buffertest Buffertest.cpp)
# target_link_libraries(Buffertest EasyEvent_net)

# add_executable(testEcho testEcho.cpp)
# target_link_libraries(testEcho EasyEvent_net)

# add_executable(testWrite testWrite.cpp)
# target_link_libraries(testWrite EasyEvent_net)

add_executable(udpClient udpClient.cpp)
target_link_libraries(udpClient EasyEvent_net)

add_executable(udpServer udpServer.cpp)
target_link_libraries(udpServer EasyEvent_net)