#pragma once
#include "ConnectionHandler.h"
#include "event.h"
#include "Frame.h"
#include <string>
#include <mutex>
#include <map>

// TODO: implement the STOMP protocol
class StompProtocol {
    private:
        ConnectionHandler& connectionHandler;
        bool connected;
        bool shouldTerminateBool;
        int subscriptionIdCounter;
        int receiptIdCounter;
        std::map<std::string, int> subscriptions;
        std::mutex dataReceivedLock;
        std::map<std::string, std::map<std::string, std::vector<Event>>> dataReceived;
        std::map<int, Frame> sentMessages;
        std::string makeReportForSummary(std::string channel, std::string user);
        void connect(const std::string& username, const std::string& password);
        void subscribe(const std::string& topic);
        void unsubscribe(const std::string& topic);
        void send(const std::string& topic, const std::string& message);
        void disconnect();
        

    public:
        StompProtocol(ConnectionHandler& handler);
        ~StompProtocol();

        bool processFromKeyboard(std::string userInput);
        void processFromServer(Frame message);
        bool shouldTerminate() const;
        bool isConnected() const;
};
