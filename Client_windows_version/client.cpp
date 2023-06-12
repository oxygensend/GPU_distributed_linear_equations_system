#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <curl/curl.h>
#include <thread>
#include <fstream>
#include "web_socket_client.h"
#include "singleton_vector.h"
#include <filesystem>

#pragma warning(disable:4996)


using namespace std;

 constexpr int STATUS_PORT = 65432;


 struct taskStruct {
     string downloaded_file;
     int task_number;
 };

int sendRegistrationRequest(string server_uri) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Error initializing curl" << endl;
        return 1;
    }

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    string body = "{\"status_port\": " + to_string(STATUS_PORT) + "}";

    string url = server_uri + "/register";

    cout << url << endl;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "Registration failed: " << curl_easy_strerror(res) << std::endl;
        return 1;
    }
    else {
        cout << "Machine logged" << endl;
    }

    curl_easy_cleanup(curl);

    return 0;
}

void sendRemoveRequest(string server_uri) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Error initializing curl" << endl;
        return;
    }

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    string body = "{\"status_port\": " + to_string(STATUS_PORT) + "}";
    string url = server_uri + "/remove";

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

static size_t curlWriteToBufferCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static size_t curlWriteToFileCallback(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}


void downloadFiles(SingletonVector* v, vector<taskStruct>& downloaded_files, string http_uri) {
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Error: could not initialize cURL" << endl;
        return;
    }
    for (auto task : v->getVector()) {

        string url = http_uri + "/data/data" + to_string(task) + ".h5";
        string file_path = "..\\data\\data" + to_string(task) + ".h5";
        // string readBuffer;
        FILE* fp = fopen(file_path.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteToFileCallback);
        // curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        CURLcode result = curl_easy_perform(curl);
        fclose(fp);
        cout << task << endl;
   
        if (result == CURLE_OK) {
            v->removeElement(task);
            taskStruct t = { file_path, task };
            downloaded_files.push_back(t);

        }
        else {
            cerr << "Error: could not download file " << url << " (cURL error code " << result << ")" << endl;
        }
    }
    curl_easy_cleanup(curl);
}

int main(int argc, char* argv[]) {

    string hostname = (argc != 2) ? "localhost" : argv[1];
    string ws_uri = "ws://" + hostname + ":1111";
    string http_uri = "http://" + hostname + ":3000";

    WebSocketClient c;
    SingletonVector* v = SingletonVector::getInstance();
    vector<taskStruct> downloaded_files;

    // REGISTER MACHINE
    if (sendRegistrationRequest(http_uri)) {
        return 1;
    };

    // Collect data from server
    thread t_client([&c, &ws_uri]() {
        c.run(ws_uri);
        });
    t_client.detach();

    // Download data from server         
    thread t_downloader([&v, &downloaded_files, http_uri]() {
        while (true) {

            if (!v->isEmpty()) {
                downloadFiles(v, downloaded_files, http_uri);
            }
            else {
                this_thread::sleep_for(chrono::milliseconds(100));
            }
        }
        });
     t_downloader.detach();


     while(true){
        if (!downloaded_files.empty()) {

            taskStruct t = downloaded_files[0];

            string program = "..\\Cuda\\Determinant_calculation_with_CUDA.exe " + t.downloaded_file + " dataset";
            int programStatus = system(program.c_str());

            double result;
            std::ifstream output_file;
            output_file.open("..\\outputFile.txt", std::fstream::in);
            output_file >> result;
            output_file.close();

          
            Json::Value msg;
            msg["type"] = 4;
            msg["task"] = t.task_number;
            msg["solution"] = result;
            cout << result << endl;
            c.send_message(msg.toStyledString());
            downloaded_files.erase(downloaded_files.begin());

            filesystem::remove(t.downloaded_file);

            // let the server know that machine finished all tasks
            if (downloaded_files.empty() && v->isEmpty()) {
                Json::Value msg;
                msg["type"] = 5;
                c.send_message(msg.toStyledString());
            }

        }
        else {
            this_thread::sleep_for(chrono::milliseconds(100));
        }


    }
    sendRemoveRequest(http_uri);

    return 0;

}