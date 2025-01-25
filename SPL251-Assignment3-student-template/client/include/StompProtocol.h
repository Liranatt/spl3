#pragma once
#include "ConnectionHandler.h"
#include "Frame.h"
#include <string>
#include <map>

// TODO: implement the STOMP protocol
class StompProtocol {
    private:
    ConnectionHandler& connectionHandler;
    bool connected;
    int subcriptionIdCounter;
    int receiptIdCounter;
    std::map<int, std::string> subscriptions;

public:
    StompProtocol(ConnectionHandler& handler);
    ~StompProtocol();

    void connect(const std::string& username, const std::string& password);
    void subscribe(const std::string& topic);
    void unsubscribe(const std::string& topic);
    void send(const std::string& topic, const std::string& message);
    void disconnect();
    void handleFrame(const std::string& frame);
};
