#include "../include/StompProtocol.h"
#include "../include/FrameCodec.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <bits/stdc++.h>


using namespace std;

StompProtocol::StompProtocol(ConnectionHandler& handler)
    :connectionHandler(handler),
    connected(false), 
    shouldTerminateBool(false), 
    subscriptionIdCounter(0), 
    receiptIdCounter(0), 
    subscriptions(), 
    dataReceivedLock(), 
    dataReceived()  {}

StompProtocol::~StompProtocol() {
    delete dataReceived;
}

void StompProtocol::connect(const std::string& username, const std::string& password) {
    if (connected) {
        throw std::runtime_error("Already connected.");
    }

    Frame connectFrame("CONNECT");
    connectFrame.addHeader("login", username);
    connectFrame.addHeader("passcode", password);
    connectFrame.addHeader("accept-version", "1.2");
    connectFrame.addHeader("host", "stomp.cs.bgu.ac.il");

    std::string encodedFrame = FrameCodec::encode(connectFrame);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');

    std::string response;
    connectionHandler.getFrameAscii(response, '\0');
    Frame responseFrame = FrameCodec::decode(response);

    if (responseFrame.getCommand() == "CONNECTED") {
        connected = true;
        std::cout << "Connected to STOMP server." << std::endl;
    } else {
        throw std::runtime_error("Failed to connect: " + responseFrame.getBody());
    }
}

void StompProtocol::subscribe(const std::string& topic) {
    if (!connected) {
        throw std::runtime_error("Not connected.");
    }

    int subscriptionId = subscriptionIdCounter++;
    subscriptions[subscriptionId] = topic;

    Frame subscribeFrame("SUBSCRIBE");
    subscribeFrame.addHeader("destination", topic);
    subscribeFrame.addHeader("id", std::to_string(subscriptionId));

    std::string encodedFrame = FrameCodec::encode(subscribeFrame);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');

    std::cout << "Subscribed to topic: " << topic << std::endl;
}

void StompProtocol::unsubscribe(const std::string& topic) {
    if (!connected) {
        throw std::runtime_error("Not connected.");
    }

    for (const auto& [id, subscribedTopic] : subscriptions) {
        if (subscribedTopic == topic) {
            Frame unsubscribeFrame("UNSUBSCRIBE");
            unsubscribeFrame.addHeader("id", std::to_string(id));
            std::string encodedFrame = FrameCodec::encode(unsubscribeFrame);
            connectionHandler.sendFrameAscii(encodedFrame, '\0');
            subscriptions.erase(id);

            std::cout << "Unsubscribed from topic: " << topic << std::endl;
            return;
        }
    }

    throw std::runtime_error("Topic not subscribed: " + topic);
}

void StompProtocol::send(const std::string& topic, const std::string& message) {
    if (!connected) {
        throw std::runtime_error("Not connected.");
    }

    Frame sendFrame("SEND");
    sendFrame.addHeader("destination", topic);
    sendFrame.setBody(message);
    sendFrame.addHeader("reciept", std::to_string(receiptIdCounter));
    sentMessages[receiptIdCounter] = sendFrame;
    receiptIdCounter++;

    std::string encodedFrame = FrameCodec::encode(sendFrame);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');

    std::cout << "Message sent to topic: " << topic << std::endl;
}

void StompProtocol::disconnect() {
    if (!connected) {
        throw std::runtime_error("Not connected.");
    }

    Frame disconnectFrame("DISCONNECT");
    std::string encodedFrame = FrameCodec::encode(disconnectFrame);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');
    connected = false;

    std::cout << "Disconnected from server." << std::endl;
}

std::string StompProtocol::processFromKeyboard(std::string userInput) {
    vector<string> line;
    string argument;
    while (stringstream(userInput) >> argument) {
        line.push_back(argument);
    }
    
    if (line[0] == "login" ) {

    }
    else if (line[0] == "summery" ) {

    }
    else if (line[0] == "exit") {
        disconnect();
        return "";
    } else if (userInput.find("subscribe") == 0) {
        subscribe(line[1]);
        return "";
    } else if (userInput.find("send")  == 0) {
        send(line[1], line[2]);
        return "";
    } else if (userInput.find("unsubscribe")  == 0) {
        unsubscribe(line[1]);
        return "";
    } else {
        return "Unknown command: " + userInput ;
    }
}

void StompProtocol::processFromServer(Frame message) {
    if (message.getCommand() == "MESSAGE") {
        Event event(message.getBody());
        string channel = event.get_channel_name();
        string user = event.getEventOwnerUser();
        int time = event.get_date_time();
        if (!channel.empty() & !user.empty() & time != 0) {
            dataReceivedLock.lock();
            (*dataReceived)[channel][user].push_back(event);
            dataReceivedLock.unlock();
            // the next code sort them by time;
            // auto& userEvents = dataReceived[channel][user];
            // userEvents.push_back(event);
            // sort(userEvents.begin(), userEvents.end(), [](const Event& a, const Event& b) {
            //     return a.get_date_time() < b.get_date_time();
        }
        else {
            cout << "no topic or user or time in SEND Frame from server" << endl;
        }
    }
    else if (message.getCommand() == "CONNECTED") {
        connected = true;
        std::cout << "Connected to STOMP server." << std::endl;
    }
    else if (message.getCommand() == "RECEIPT") {
        int recepitId = stoi(message.getHeaders()["receipt-id"]);
        if (sentMessages[recepitId].getCommand() == "SUBSCRIBE") {

        }
        if (sentMessages[recepitId].getCommand() == "UNSUBSCRIBE") {
            
        }
        if (sentMessages[recepitId].getCommand() == "SEND") {
            
        }
    }
    else if (message.getCommand() == "ERROR") {

    }
}


bool StompProtocol::shouldTerminate() const {
    return shouldTerminateBool;
}
