# GPU_linear_equations_system
### Project carried out as part of the course credit Design Studio at AGH UST
## PL
Obliczenia rozproszone na platformach graficznych (GPU). Celem projektu jest stworzenie systemu służącego do rozwiązania układu liniowego równań z wykorzystaniem GPU w systemie rozproszonym. Na system muszą składać się minimum trzy niezależne maszyny komunikujące się poprzez sieć lokalną. W przypadku gdy jedno z urządzeń przestanie działać, to pozostałe powinny móc kontynuować jego pracę.

## EN
Distributed computing on graphics processing unit (GPU) platforms. The goal of the project is to develop a system for solving a system of linear equations using GPUs in a distributed system. The system must consist of a minimum of three independent machines communicating over a local network. In case one of the machines stops working, the others should be able to continue its work.	

# Server

# Client GPU
# Client CPU
## PL
Wymagana wersja `C++17`

Aby odpalić program kliencki CPU  nalezy doinstalowac zewnętrzne biblioteki w zaleznosci od wersji bitowej windowsa. 

Przykład zainstalowania wymaganych bibliotek z wykorzystaniem narzędzia vcpkg dla wersji windowsa 64 bitowego:

    2. vcpkg install curl:x64-windows jsoncpp:x64-windows websocketpp:x64-windows boost-random:x64-windows
    3. vcpkg integrate install - polaczenie bibliotek z visual studio 

Program przyjmuje opcjonalny argument jaki jest address ip/nazwa domeny serwera. Defaultową wartością jest `localhost`.

Przykład wywołania programu: ` ./client.exe 127.0.0.1 `. Zalecana jest kompilacja poprzez Visual Studio.

Skompilowana wersja klienta GPU obsługującego obliczenia wraz z wszystkimi zaleznosciami znajduje się w katalogu `Client_windows_version`, dzieki temu doinstalowywanie dodatkowych bibliotek nie jest potrzebne.


## EN

Required version `C++17`.

To run the CPU client program, external libraries need to be installed depending on the bit version of windows. 

An example of installing the required libraries using the vcpkg utility for 64 bit version of windows:

    2. vcpkg install curl:x64-windows jsoncpp:x64-windows websocketpp:x64-windows boost-random:x64-windows
    3. vcpkg integrate install - link libraries with visual studio 

The program takes an optional argument which is the ip address/domain name of the server. The default value is `localhost`.

An example of calling the program: ` ./client.exe 127.0.0.1 `. Compilation through Visual Studio is recommended.

The compiled version of the GPU computing client with all the dependencies is located in the `Client_windows_version` directory, so installing additional libraries is not necessary.

## Stack
    - C++17
    - jsonCpp
    - websocketpp
    - curl
    - VS SLN



	
 
