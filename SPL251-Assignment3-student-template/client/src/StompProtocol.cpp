#include "../include/StompProtocol.h"
#include "../include/ConnectFrame.h"
#include "SubscribeFrame.h"
#include "UnsubscribedFrame.h"
#include "SendFrame.h"
#include "DisconnectFrame.h"
#include <stdexcept>
#include <iostream>


StompProtocol::StompProtocol(ConnectionHandler& handler):
ConnectionHander(handler), connected(false), subscriptionIdCounter(0), receiptIdCounter(0){}

StompProtocol::~StompProtocol() {}

void StompProtocol::connect(const std::String& username, const std::string& password){
    if (connected){
        throw std::runtime_error("Already connected");
    }

    ConnectFrame frame(username, password);
    std::string encodedFrame = frame.encode();
    connectionHandler.sendFrameAscii(encodedFrame, '\0');
    std::string response;
    connectionHnadler.getFrameAscii(response, '\0');
    handleFrame(response);
}

void StompProtocol::subscribe(const std::string& topic) {
    if (!connected) {
        throw std::runtime_error("Not connected");
    }

    int subscripionId = subscriptionIdCounter++;
    subscriptions[subscripionId] = topic;
    SubscribeFrame frame(topic, subscriptionId);
    std::string encodedFrame = frame.encode();
    ConnectionHandler.sendFrameAscii(encodedFrame, '/0');
}

void StompProtocol::unsubscribe(const std::string& topic) {
    if (!connected){
        throw std::runtime_error("Not connected")''
    }

    for (const auto& [id, subscribedTopic]: subscriptions){
        if (subscribedTopic == topic){
            UnsubscribedFrame frame(id);
            subscriptions.erase(id);
            std::string encodedFrame = frame.encode();
            ConnectionHandler.sendFramesAscii(encodedFrame, '\0');
            return;
        }
    }
    throw std::runtime_error("Topic not subscribed");
}

void StompProtocol::send(const std::string& topic, const std::string& message){
     if (!connected){
        throw std::runtime_error("Not connected")''
    }

    SendFrame Frame(topic, message);
    std::string encodedFrame = frame.encode();
    connectionHandler.sendFrameAscii(encodedFrame, '\0');
}

void StompProtocol::disconnect() {
     if (!connected){
        throw std::runtime_error("Not connected")''
    }

    DisconnectFrame frame;
    std::string encodedFrame = frame.encode();
    ConnectionHandler.sendFrameAscii(encodedFrame, '\0');
    connected = false;
}

void StompProtocol::handleFrame(const std::string& frame) {
    std::istringstream stream(frame);
    std::string command;
    std::getline(stream, command);

    if (command == "CONNECTED"){
        connected = true;
        std::cout << "Connected Succesfully" << std::endl;
    }
    else if (command == "MESSAGE") {
        std::cout << "New message received: " << frame << std::endl;
    }
    else if (command == "RECEIPT") {
    std::cout << "Receipt acknowledged: " << frame << std::endl;
    }
    else if (command == "ERROR") {
    std::cerr << "Error received: " << frame << std::endl;
    }   
}


