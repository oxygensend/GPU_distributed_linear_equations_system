#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <json/json.h>


class WebSocketClient {
public:
    typedef websocketpp::client <websocketpp::config::asio_client> client;
    typedef websocketpp::lib::lock_guard <websocketpp::lib::mutex> scoped_lock;

    WebSocketClient();

    void run(const std::string &uri);

    void on_open(websocketpp::connection_hdl hdl);

    void on_close(websocketpp::connection_hdl);

    void on_message(client *c, websocketpp::connection_hdl hdl, client::message_ptr msg);

    void on_fail(websocketpp::connection_hdl);

    void send_message(const std::string message);

    void loop();

private:
    client m_client;
    websocketpp::connection_hdl m_hdl;
    websocketpp::lib::mutex m_lock;
    bool m_open;
    bool m_done;
    std::vector<int> to_solve;

};

#endif // WEBSOCKET_CLIENT_H
