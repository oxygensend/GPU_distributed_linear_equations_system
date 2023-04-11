#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <curl/curl.h>
#include <thread>
// #include  "web_socket_client.h"
// #include "singleton_vector.h"

using namespace std;

using namespace std;

// ten kod musi zostac odpalony na odzielnym watku w cudzie
// constexpr int STATUS_PORT = 65432;
#define STATUS_PORT 65432

void runStatusListener() {
  int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = static_cast<socklen_t>(sizeof(address));
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Socket failed" << std::endl;
        return ;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)) == -1) {
        std::cerr << "Set socketopt failed" << std::endl;
        return ;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(65432);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    if (listen(server_fd, 2) == -1) {
        std::cerr << "Listen failed"<< std::endl;
        return;
    }

    while (true) {
        std::cout << "Waiting for connections..." << std::endl;

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, reinterpret_cast<socklen_t *>(&addrlen))) == -1) {
            std::cerr << "Accept failed" << std::endl;
            return ;
        }

        std::cout << "Connected!" << std::endl;
        close(new_socket);
    }
}

int sendRegistrationRequest() {
    CURL *curl = curl_easy_init();
    if (!curl) {
        cerr << "Error initializing curl" << endl;
        return 1;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    string body = "{\"status_port\": "+ to_string(STATUS_PORT) + "}";

    string url = "http://192.168.1.12:3000/register";

    cout << url << endl;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "Registration failed: " << curl_easy_strerror(res) << std::endl;
        return 1;
    } else {
        cout << "Machine logged" << endl;
    }

    curl_easy_cleanup(curl);

    return 0;
}

void sendRemoveRequest() {
    CURL *curl = curl_easy_init();
    if (!curl) {
        cerr << "Error initializing curl" << endl;
        return;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    string body = "{\"status_port\": "+ to_string(STATUS_PORT) + "}";
    string url = "http://127.0.0.1:3000/remove";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "Remove failed: " << curl_easy_strerror(res) << std::endl;
        return;
    }

    curl_easy_cleanup(curl);
}

int main() {

    // // // WebSocketClient c;
    // // string uri = "ws://localhost:1111";
    // SingletonVector* v = SingletonVector::getInstance();


    // REGISTER MACHINE
    if(sendRegistrationRequest()){
        return 1;
    };

    // CHECKING MACHINE STATUS
    // thread  t_status_check([]() {
        runStatusListener();
    // });
    // t_status_check.detach();

    // // Colect data from server
    // thread t_client([&c, &uri]() {
    //     c.run(uri);
    // });
    // t_client.detach();

    // // GPU logic
    // while(true){
    //  vector<int>  to_solve = v->getVector();
    
    //     if(to_solve.size() > 0){
    //         // for (auto task : to_solve) {
    //         //     string filename = "data/data" + to_string(task) + ".h5";
    //         //     ifstream file(filename, ios::in | ios::binary);
    //         //     if (file.is_open()) {
    //         //         file.seekg(0, ios::end);
    //         //         size_t size = file.tellg();
    //         //         file.seekg(0, ios::beg);
    //         //         char* buffer = new char[size];
    //         //         file.read(buffer, size);
    //         //         file.close();
    //         //         h5::fd_t fd = h5::from_buffer(buffer, size, h5::fd::rdonly);
    //         //         auto dataset = h5::read_dataset<int>(fd, "dataset");
    //         //         auto matrix = h5::reshape(dataset, {10, 10});
    //         //         double solution = h5::det(matrix);
    //         //         Json::Value msg;
    //         //         msg["type"] = 4;
    //         //         msg["task"] = task;
    //         //         msg["solution"] = solution;
    //         //         c.send(hdl, msg.toStyledString(), websocketpp::frame::opcode::text);
    //         //         to_solve.erase(std::remove(to_solve.begin(), to_solve.end(), task), to_solve.end());
    //         //         delete[] buffer;
    //         //     } else {
    //         //         cerr << "Error: could not open file " << filename << endl;
    //         //     }
    //         // }
    // } else {
    //     this_thread::sleep_for(chrono::milliseconds(100));
    // }


    // }

    return 0;

}