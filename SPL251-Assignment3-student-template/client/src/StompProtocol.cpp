#include "../include/StompProtocol.h"
#include "../include/FrameCodec.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <iostream>
#include <fstream>


using namespace std;

StompProtocol::StompProtocol(ConnectionHandler& handler)
    :connectionHandler(handler),
    connected(false), 
    shouldTerminateBool(false), 
    subscriptionIdCounter(0), 
    receiptIdCounter(0), 
    subscriptions(), 
    dataReceivedLock(), 
    dataReceived(),
    sentMessages()  {}

StompProtocol::~StompProtocol() {
    // delete dataReceived;
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
    // subscriptions[subscriptionId] = topic;
    subscriptions[topic] = subscriptionId;

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

    for (const auto& [subscribedTopic, id] : subscriptions) {
        if (subscribedTopic == topic) {
            Frame unsubscribeFrame("UNSUBSCRIBE");
            unsubscribeFrame.addHeader("id", to_string(id));
            std::string encodedFrame = FrameCodec::encode(unsubscribeFrame);
            connectionHandler.sendFrameAscii(encodedFrame, '\0');
            subscriptions.erase(subscribedTopic);

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

bool StompProtocol::processFromKeyboard(std::string userInput) {
    vector<string> line;
    string argument;
    while (stringstream(userInput) >> argument) {
        line.push_back(argument);
    }
    
    if (line[0] == "login" ) {
        connect(line[1], line[2]);
    }
    else if (line[0] == "summery" ) { // nir
        ofstream outFile(line[3]);
        if (outFile.is_open()) {
            outFile << makeReportForSummary(line[1], line[2] );
            outFile.close();
            cout << "summary exported \n" ;
        }
        else {
        cerr << "Failed to open the file!" << endl;
        }
    }
    else if (line[0] == "report" ) { // nir
        
    }
    else if (line[0] == "exit") {
        disconnect();
        return true;
    } else if (userInput.find("subscribe") == 0) {
        subscribe(line[1]);
        return true;
    } else if (userInput.find("send")  == 0) {
        send(line[1], line[2]);
        return true;
    } else if (userInput.find("unsubscribe")  == 0) {
        unsubscribe(line[1]);
        return true;
    } else {
        return false;
    }
    return false;
}

void StompProtocol::processFromServer(Frame message) {
    if (message.getCommand() == "MESSAGE") { // nir
        Event event(message.getBody());
        string channel = event.get_channel_name();
        string user = event.getEventOwnerUser();
        int time = event.get_date_time();
        if ((!channel.empty() & !user.empty()) & (time != 0)) {
            dataReceivedLock.lock();
            (dataReceived)[channel][user].push_back(event);
            dataReceivedLock.unlock();
        }
        else {
            cout << "no topic or no user or no time in SEND Frame from server" << endl;
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

bool StompProtocol::isConnected() const {
    return connected;
}

string StompProtocol::makeReportForSummary(string channel, string user) {
    int active = 0;
    int arrived = 0;
    dataReceivedLock.lock();
    int total = (dataReceived)[channel][user].size();
    for ( int i = 0; i < total; i++)  {
        if ((dataReceived)[channel][user][i].get_general_information().count("active") > 0 &&
            (dataReceived)[channel][user][i].get_general_information().at("active") == "true")
            active++;
        if ((dataReceived)[channel][user][i].get_general_information().count("forces arrival at scene") > 0 &&
            (dataReceived)[channel][user][i].get_general_information().at("forces arrival at scene") == "true")
            arrived++;
    }

    string toPrint = "Channel " + channel + "\n";
    toPrint.append("Stats:\n");
    toPrint.append("Total: ").append(to_string(total)).append("\n");
    toPrint.append("active: ").append(to_string(active)).append("\n");
    toPrint.append("forces arrival at scene : ").append(to_string(arrived)).append("\n");
    toPrint.append("\nEvent Reports:\n");

    auto& events = (dataReceived)[channel][user];
    sort(events.begin(), events.end(), [](const Event &a, const Event &b) {
        if (a.get_date_time() != b.get_date_time()) {
            return a.get_date_time() < b.get_date_time(); // sort by time
        }
        return a.get_name() < b.get_name(); // sort by name
    });

    for (int i = 0; i < total; i++) {
        toPrint.append("\nReport_").append(to_string((i+1))).append("\n");
        toPrint.append("\tciry: ").append(events[i].get_city()).append("\n");
        toPrint.append("\tdate time: ").append(to_string(events[i].get_date_time())).append("\n");
        toPrint.append("\tevent name: ").append(events[i].get_name()).append("\n");
        if (events[i].get_description().length() <= 30)
            toPrint.append("\tevent name: ").append(events[i].get_name()).append("\n");
        else 
            toPrint.append("\tevent name: ").append(events[i].get_name().substr(0, 27)).append("...\n");
    }
    dataReceivedLock.unlock();
    return toPrint;
}







