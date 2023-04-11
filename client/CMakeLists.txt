//gowno dziala, moze kiedys
cmake_minimum_required(VERSION 3.24)
project(mywebsocket)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)


target_link_libraries(mywebsocket PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(mywebsocket PRIVATE Boost::system Boost::thread)


add_executable(GPU_linear_equations_system
        ./client.cpp
        ./singleton_vector.h
        ./web_socket_client.cpp
        ./web_socket_client.h)
