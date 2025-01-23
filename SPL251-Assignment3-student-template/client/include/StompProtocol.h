#pragma once
#include "ConnectionHandler.h"
#include "Frame.h"
#include <string>
#include <unorderedmap>

// TODO: implement the STOMP protocol
class StompProtocol {
    private:
    ConnectionHandler& ConnectionHandler;
    bool connected;
    int subcriptionIdCounter;
    int receiptIdCounter;
    std::unordered_map<int, std::string> subscriptions;

public:
    StompProtocol(ConnectionHandler& handler);
    ~StompProtocol();

    void connect(const std::string& username, cnost std::string& password);
    void subscribe(cnost std::string& topic);
    void unsubscribe(const std::string& topic);
    void send(const std::string& topic, const std::string& message)
    void discoonect();
    void handleFrame(const std::string& frame);
};
