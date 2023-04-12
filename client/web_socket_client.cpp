#include "web_socket_client.h"
#include "singleton_vector.h"

using namespace std;

WebSocketClient::WebSocketClient() : m_open(false), m_done(false) {


    // Set up logs
    m_client.clear_access_channels(websocketpp::log::alevel::all);
    m_client.set_access_channels(websocketpp::log::alevel::connect);
    m_client.set_access_channels(websocketpp::log::alevel::disconnect);
    m_client.set_access_channels(websocketpp::log::alevel::app);

    m_client.init_asio();

    // Bind the handlers we are using
    using websocketpp::lib::bind;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    m_client.set_open_handler(bind(&WebSocketClient::on_open, this, _1));
    m_client.set_close_handler(bind(&WebSocketClient::on_close, this, _1));
    m_client.set_fail_handler(bind(&WebSocketClient::on_fail, this, _1));
    m_client.set_message_handler(bind(&WebSocketClient::on_message, this, &m_client, _1, _2));
}

void WebSocketClient::run(const std::string &uri) {
    // Create connection
    websocketpp::lib::error_code ec;
    client::connection_ptr con = m_client.get_connection(uri, ec);
    if (ec) {
        m_client.get_alog().write(websocketpp::log::alevel::app,
                                  "Get Connection Error: " + ec.message());
        return;
    }

    m_hdl = con->get_handle();
    m_client.connect(con);

    // Create a thread to run the ASIO io_service event loop
    websocketpp::lib::thread asio_thread(&client::run, &m_client);

    // Create a thread to run the telemetry loop
    websocketpp::lib::thread client_thread(&WebSocketClient::loop, this);

    asio_thread.join();
    client_thread.join();
}

void WebSocketClient::on_open(websocketpp::connection_hdl hdl) {
    // Open connection
    m_client.get_alog().write(websocketpp::log::alevel::app,
                              "Connection opened, starting GPU!");

    scoped_lock guard(m_lock);
    m_open = true;

    // Send data
    Json::Value msg;
    msg["type"] = 0;
    msg["status_port"] = 65432;
    m_client.send(hdl, msg.toStyledString(), websocketpp::frame::opcode::text);
}

void WebSocketClient::on_close(websocketpp::connection_hdl) {
    m_client.get_alog().write(websocketpp::log::alevel::app,
                              "Connection closed!");

    scoped_lock guard(m_lock);
    m_done = true;
}

void WebSocketClient::on_message(client *c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
    try {
        Json::Value data;
        Json::Reader reader;
        if (reader.parse(msg->get_payload(), data)) {
            cout << data << endl;
            if (data["type"] == 3) {
                SingletonVector *singleton_vector = SingletonVector::getInstance();
                const Json::Value in = data["to_solve"];
                vector<int> temp_vector;
                transform(in.begin(), in.end(), back_inserter(temp_vector), [](const auto &e) { return e.asInt(); });

                singleton_vector->setVector(temp_vector);
            }
        }
    }
    catch (exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
}

// The fail handler will signal that we should stop sending telemetry
void WebSocketClient::on_fail(websocketpp::connection_hdl) {
    m_client.get_alog().write(websocketpp::log::alevel::app,
                              "Connection failed!");

    scoped_lock guard(m_lock);
    m_done = true;
}

void WebSocketClient::loop() {
    uint64_t count = 0;
    std::stringstream val;
    websocketpp::lib::error_code ec;

    while (1) {
        bool wait = false;

        {
            scoped_lock guard(m_lock);
            // If the connection has been closed, stop
            if (m_done) {
                break;
            }

            // If the connection hasn't been opened yet wait a bit and retry
            if (!m_open) {
                wait = true;
            }
        }

        if (wait) {
            this_thread::sleep_for(chrono::milliseconds(1000));
            continue;
        }

        if (ec) {
            m_client.get_alog().write(websocketpp::log::alevel::app,
                                      "Send Error: " + ec.message());
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}


void WebSocketClient::send_message(string message) {
    m_client.get_alog().write(websocketpp::log::alevel::app,
                              "Sending message to server " + message);


    client::connection_ptr con = m_client.get_con_from_hdl(m_hdl);
    m_client.send(con, message, websocketpp::frame::opcode::text);
}
