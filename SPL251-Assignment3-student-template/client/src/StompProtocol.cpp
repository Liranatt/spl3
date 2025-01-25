#include "../include/StompProtocol.h"
#include "../include/FrameCodec.h"
#include <stdexcept>
#include <iostream>

using namespace std;

StompProtocol::StompProtocol(ConnectionHandler& handler)
    : connectionHandler(handler), connected(false), subscriptionIdCounter(0), receiptIdCounter(0) {}

StompProtocol::~StompProtocol() {}

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
    cout << "response:" << endl << response << endl;
    Frame responseFrame = FrameCodec::decode(response);
    cout << "recieved command:" << endl << responseFrame.getCommand() << endl;
    cout << "recieved body:" << endl << responseFrame.getBody() << endl;

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
