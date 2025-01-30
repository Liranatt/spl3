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
        bool terminateAllClients;
        int subscriptionIdCounter;
        int sentIdCounter;
        std::map<std::string, int> subscriptions;
        std::mutex dataReceivedLock;
        std::map<std::string, std::map<std::string, std::vector<Event>>> dataReceived;
        // std::map<int, std::string> sentMessages;
        std::map<int, std::function<void()>> receiptActions;
        std::string username;
        std::string makeReportForSummary(std::string channel, std::string user);
        std::string processReport(std::vector<std::string> line);
        std::string subscribe(const std::string& topic);
        std::string unsubscribe(const std::string& topic);
        void send(const std::string& topic, const std::string& message);
        void disconnect();


    public:
        StompProtocol(ConnectionHandler& handler);
        ~StompProtocol() = default;
        void setUsername(std::string username);
        void connect(const std::string& username, const std::string& password);
        std::string processFromKeyboard(std::string userInput);
        void processFromServer(Frame message);
        bool shouldTerminate() const;
        bool isConnected() const;
        bool shouldTerminateAllClients() const;
};
