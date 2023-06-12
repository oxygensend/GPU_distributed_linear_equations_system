# GPU_linear_equations_system
### Project carried out as part of the course credit Design Studio at AGH UST
## PL
Obliczenia rozproszone na platformach graficznych (GPU). Celem projektu jest stworzenie systemu służącego do rozwiązania układu liniowego równań z wykorzystaniem GPU w systemie rozproszonym. Na system muszą składać się minimum trzy niezależne maszyny komunikujące się poprzez sieć lokalną. W przypadku gdy jedno z urządzeń przestanie działać, to pozostałe powinny móc kontynuować jego pracę.

## EN
Distributed computing on graphics processing unit (GPU) platforms. The goal of the project is to develop a system for solving a system of linear equations using GPUs in a distributed system. The system must consist of a minimum of three independent machines communicating over a local network. In case one of the machines stops working, the others should be able to continue its work.	

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


# Server

## PL
Serwer obsługujący zdalne połączenie pomiędzy jednostkami obliczeniowymi przygotowany jest w Pythonie. Aby go uruchomić należy wykonać poniższe polecenia a także otworzyć porty :3000 oraz :1111 na zewnętrzne połączenia TCP.

## EN
The server that supports remote connection between computing units is prepared in Python. To run it, you need to run the following commands and also open ports :300 and :1111 for external TCP connections.

```
git clone https://github.com/oxygensend/GPU_distributed_linear_equations_system.git
cd GPU_distributed_linear_equations_system/
cd server
pip install flask simple_websocket_server numpy
python index.py
```

# GPU Client
```
Step 1: Clone the vcpkg repo
	git clone https://github.com/Microsoft/vcpkg.git
Step 2: Run the bootstrap script to build vcpkg
	.\vcpkg\bootstrap-vcpkg.bat
Step 3: Install necessary libraries
	vcpkg install highfive
Step 4:
	 vcpkg integrate install 
Step 5: Install CUDA and CUDA dependencies, everything under this link:
https://docs.nvidia.com/cuda/cuda-installation-guide-microsoft-windows/index.html
	
```