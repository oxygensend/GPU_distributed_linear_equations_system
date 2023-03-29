#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <curl/curl.h>

#define PORT 65432

using namespace std;

// ten kod musi zostac odpalony na odzielnym watku w cudzie

void runSocketListener() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        cerr << "Socket failed";
        return;
    }


    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
        cerr << "Set socketopt failed";
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);


    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        cerr << "Bind failed";
        return;
    }

    if (listen(server_fd, 2) < 0) {
        cerr << "Listen";
        return;
    }

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
            cerr << "Error";
            return;
        }

        cout << "Hello" << endl;
        close(new_socket);
    }
}

void sendRegistrationRequest() {
    CURL *curl = curl_easy_init();
    if (!curl) {
        cerr << "Error initializing curl" << endl;
        return;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    string body = "{\"port\": "+ to_string(PORT) + "}";
    string url = "http://192.168.0.234:3000/register";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Registration failed: " << curl_easy_strerror(res) << std::endl;
        return;
    }

    curl_easy_cleanup(curl);
}

int main() {


    sendRegistrationRequest();
    runSocketListener();
    return 0;

}