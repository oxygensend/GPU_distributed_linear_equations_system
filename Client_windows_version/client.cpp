#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <curl/curl.h>
#include <thread>
#include <fstream>
#include "web_socket_client.h"
#include "singleton_vector.h"
//#include <hdf5.h>
//#include <hdf5_hl.h>
//
#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib


using namespace std;

// ten kod musi zostac odpalony na odzielnym watku w cudzie
 constexpr int STATUS_PORT = 65432;

void runStatusListener() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
  //  socklen_t addrlen = static_cast<socklen_t>(sizeof(address));
    int addrlen = sizeof(address);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cerr << "Socket failed" << std::endl;
        return;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*) & opt, sizeof(opt)) == -1) {
        cerr << "Set socketopt failed" << std::endl;
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(STATUS_PORT);
   
    if (::bind(server_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR) {
        cerr << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(server_fd);
        WSACleanup();
        return;
    }  
    
    if (listen(server_fd, 2) == -1) {
        cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        return;
    }

    while (true) {
     
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address,(int*)(&addrlen))) < -1) {
            cerr << "Accept failed: " << WSAGetLastError() <<  std::endl;
            return;
        }

        cout << "Connected with server!" << std::endl;
        closesocket(new_socket);
    }
}

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

int main(int argc, char* argv[]) {

    string hostname = (argc != 2) ? "localhost" : argv[1];
    string ws_uri = "ws://" + hostname + ":1111";
    string http_uri = "http://" + hostname + ":3000";

    WebSocketClient c;
    SingletonVector* v = SingletonVector::getInstance();


    // REGISTER MACHINE
    if (sendRegistrationRequest(http_uri)) {
        return 1;
    };

    // CHECKING MACHINE STATUS
    thread t_status_check([]() {
        runStatusListener();
        });
    t_status_check.detach();

    // Collect data from server
    thread t_client([&c, &ws_uri]() {
        c.run(ws_uri);
        });
    t_client.detach();

    // GPU logic
    while (true) {

        if (!v->isEmpty()) {
            CURL* curl = curl_easy_init();
            if (!curl) {
                cerr << "Error: could not initialize cURL" << endl;
                continue;
            }
            for (auto task : v->getVector()) {

                // Download file from server into buffer
                string url = http_uri + "/data/data" + to_string(task) + ".h5";
                string readBuffer;

                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteToBufferCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
                CURLcode result = curl_easy_perform(curl);

                if (result == CURLE_OK) {

                    //                    H5File file;
                    //                    file.openFile(readBuffer, H5F_ACC_RDONLY);
                    //
                    //                    // Read the dataset into a matrix
                    //                    DataSet dataset = file.openDataSet("your dataset name");
                    //                    DataSpace dataspace = dataset.getSpace();
                    //                    int ndims = dataspace.getSimpleExtentNdims();
                    //                    hsize_t dims[ndims];
                    //                    dataspace.getSimpleExtentDims(dims, NULL);
                    //                    double data[dims[0]][dims[1]];
                    //                    dataset.read(data, PredType::NATIVE_DOUBLE);
                    //
                    //
                    //                    // Print the matrix
                    //                    for (int i = 0; i < dims[0]; i++) {
                    //                        for (int j = 0; j < dims[1]; j++) {
                    //                            cout << data[i][j] << " ";
                    //                        }
                    //                        cout << endl;
                    //                    }

                                        // Close the file
                    //                    file.close();
                    //                    cout << url << endl;
                    ////
                    //                    size_t size = readBuffer.size();
                    //
                    //                    H5File filebuf
                    //
                    //                    // create an in-memory file
                    //                    hid_t file_id = H5Fcreate("file.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
                    //
                    //                    // create a dataset in the file
                    //                    hsize_t dims[2] = {10, 10};
                    //                    hid_t dataspace_id = H5Screate_simple(2, dims, nullptr);
                    //                    hid_t dataset_id = H5Dcreate(file_id, "dataset", H5T_NATIVE_INT, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
                    //
                    //                    // write the data to the dataset
                    //                    H5Dwrite(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, readBuffer.c_str());
                    //
                    //                    // read the data back from the dataset
                    //                    int data[100];
                    //                    cout << data[0] << endl;
                    //                    H5LTread_dataset_int(file_id, "dataset", data);
                    //
                    //                    // cleanup
                    //                    H5Dclose(dataset_id);
                    //                    H5Sclose(dataspace_id);
                    //                    H5Fclose(file_id);
                    //

                                            // Send solution
                    Json::Value msg;
                    msg["type"] = 4;
                    msg["task"] = task;
                    msg["solution"] = 4;
                    c.send_message(msg.toStyledString());

                    // Remove from queue
                    v->removeElement(task);
                }
                else {
                    cerr << "Error: could not download file " << url << " (cURL error code " << result << ")"
                        << endl;
                }
            }
            curl_easy_cleanup(curl);
        }
        else {
            this_thread::sleep_for(chrono::milliseconds(100));
        }


    }
    sendRemoveRequest(http_uri);

    return 0;

}