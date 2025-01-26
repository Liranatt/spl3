#pragma once
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include "Frame.h"
#include "StompProtocol.h"



class Client {
    
    public:
        Client(std::string host, short port);
        void run();


    private:
        void manageKeyboard();
        void manageConnections();
        void processFrame(Frame frame);
        ConnectionHandler connectionHandler;
        StompProtocol protocol;
        std::vector<std::string> toPrintOnScreen;
        std::vector<std::string> fromKeyboard;
        std::map<std::string, std::vector<Frame>> messagesReceived;
        std::mutex toPrintLock;
        std::mutex toProcessLock;
        int messagesCounter;

};