#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <curl/curl.h>

#define STATUS_PORT 65432
#define APP_PORT 5468

using namespace std;



#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>
#include <json/json.h>
#include <thread>
#include <chrono>
#include <vector>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

typedef websocketpp::client<websocketpp::config::asio> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::thread> thread_ptr;

vector<int> to_solve;

void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
    try {
        Json::Value data;
        Json::Reader reader;
        if (reader.parse(msg->get_payload(), data)) {
            cout << data << endl;
            if (data["type"] == 3) {
                to_solve = data["to_solve"].asInt();
            }
        }
    } catch (exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void on_error(client* c, websocketpp::connection_hdl hdl) {
    cerr << "Error: " << c->get_ec().message() << endl;
}

void on_close(client* c, websocketpp::connection_hdl hdl) {
    cout << "### closed ###" << endl;
}

void on_open(client* c, websocketpp::connection_hdl hdl) {
    Json::Value msg;
    msg["type"] = 0;
    msg["status_port"] = 65432;
    c->send(hdl, msg.toStyledString(), websocketpp::frame::opcode::text);
    cout << "Connected" << endl;
}


d





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

    string body = "{\"status_port\": "+ to_string(STATUS_PORT) + ", \"app_port\": "+ to_string(APP_PORT) + "}";
    string url = "http://127.0.0.1:3000/register";

    cout << url << endl;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    CURLcode res = curl_easy_perform(curl);
    cout << res << endl;
    if (res != CURLE_OK) {
        std::cerr << "Registration failed: " << curl_easy_strerror(res) << std::endl;
        return;
    }

    curl_easy_cleanup(curl);
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
    string url = "http://192.168.0.234:3000/remove";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Remove failed: " << curl_easy_strerror(res) << std::endl;
        return;
    }

    curl_easy_cleanup(curl);
}

int main() {


    sendRegistrationRequest();
    runSocketListener();
    return 0;

}