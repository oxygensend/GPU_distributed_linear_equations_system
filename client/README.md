*Kompilowanie*

Instalacja potrzebnych bibliotek z wykorzystaniem vcpkg https://github.com/microsoft/vcpkg 

vcpkg install jsoncpp
vcpkg install websocketpp

`
g++ -lcurl -I /path/to/vcpkg/installed/include client.cpp -L ~/path to/vcpkg/installed/x64-linux/lib/  -std=c++17 -ljsoncpp  -lboost_system  -c
`

`
g++ -lcurl -I /path/to/vcpkg/installed/include web_socket_client.cpp -L ~/path to/vcpkg/installed/x64-linux/lib/  -std=c++17 -ljsoncpp  -lboost_system  -c
`

`
g++  -I /path/to/vcpkg/installed/include web_socket_client.o client.o -L ~/path to/vcpkg/installed/x64-linux/lib/  -std=c++17 -ljsoncpp  -lboost_system -lcurl  -o main
`
