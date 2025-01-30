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
#include <thread>
#include <chrono>



using namespace std;

StompProtocol::StompProtocol(ConnectionHandler& handler)
    :connectionHandler(handler),
    connected(true), 
    shouldTerminateBool(false), 
    terminateAllClients(false), 
    subscriptionIdCounter(0), 
    sentIdCounter(0), 
    subscriptions(),
    dataReceivedLock(), 
    dataReceived(),
    receiptActions(),
    username()  {}

    void StompProtocol::setUsername(string username) {
        this->username = username;
    }

void StompProtocol::connect(const std::string& username, const std::string& password){
    if (connected){
        std::cout<<"The client is already logged in, log out before trying again" << endl;
    }
   
    Frame connectFrame("CONNECT");
    connectFrame.addHeader("accept-version", "1.2");
    connectFrame.addHeader("host", "stomp.cs.bgu.ac.il");
    connectFrame.addHeader("login", username);
    connectFrame.addHeader("passcode", password);

    std::string encodedFrame = FrameCodec::encode(connectFrame);
    if(!connectionHandler.sendFrameAscii(encodedFrame, '\0')){
        cerr<< "failed to send connect frame" << endl;
    }
    sentIdCounter++;
    connected = true;
}

string StompProtocol::subscribe(const std::string& topic){
    if (!connected){
        std::cout<<"The client is not connected"<<std::endl;
    }

    if (subscriptions.count(topic) == 1) {
        return "you are already subscribed to " + topic;
    }
    subscriptionIdCounter++;
    int subscriptionId = subscriptionIdCounter;
    sentIdCounter++;
    receiptActions[sentIdCounter] = [&topic, subscriptionId, this]() {
        cout << "joined channel " << topic << endl;
        subscriptions[topic] = subscriptionId;
    };

    Frame subframe("SUBSCRIBE");
    subframe.addHeader("destination", topic);
    subframe.addHeader("id", std::to_string(subscriptionId));
    subframe.addHeader("receipt", std::to_string(sentIdCounter));
    std::string encodedFrame = FrameCodec::encode(subframe);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');
    return "";
}

string StompProtocol::unsubscribe(const std::string& topic) {
    if (!connected){
         std::cout<<"The client is not connected"<<std::endl;
    }
    if (subscriptions.count(topic) == 0) {
        return "you are not subscribed to " + topic;
    }
    Frame unsubframe("UNSUBSCRIBE");
    unsubframe.addHeader("id", to_string(subscriptions[topic]));
    sentIdCounter++;
    receiptActions[sentIdCounter] = [&topic, this]() {
        cout << "exited channel " << topic << endl;
        subscriptions.erase(topic);
    };
    unsubframe.addHeader("receipt", std::to_string(sentIdCounter));
    std::string encodedFrame = FrameCodec::encode(unsubframe);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');
    return "";
}


void StompProtocol::disconnect(){
    if (!isConnected()){
        std::cout<<"The client is not connected";
    }
    sentIdCounter++;
    Frame disconnectframe("DISCONNECT");
    disconnectframe.addHeader("receipt", std::to_string(sentIdCounter));
    std::string encodedFrame = FrameCodec::encode(disconnectframe);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');
    receiptActions[sentIdCounter] = [this]() {
        cout << "disconnected from server " << endl;
        shouldTerminateBool = true;
        connected = false;
    };
}

string StompProtocol::processFromKeyboard(std::string userInput) {
    vector<string> line;
    string argument;
    stringstream ss(userInput);
    
    while (getline(ss, argument, ' ')) {
        line.push_back(argument);
    }

    if (line[0] == "login") {
        if (line.size() != 3)
            return "login command needs 3 args: {host:port}, {username}, {passcode}";
        connect(line[1], line[2]);
        return "";
    }
    else if (line[0] == "join") {
        if (line.size() != 2)
            return "join command needs 1 args: {channel_name}";
        return subscribe(line[1]);
    }
    else if (line[0] == "exit") {
        if (line.size() != 2)
            return "exit command needs 1 args: {channel_name}";
        return unsubscribe(line[1]);
    }
    else if (line[0] == "logout") {
        if (line.size() != 1)
            return "logout command needs 0 args";
        disconnect();
        return "";
    }
    else if (line[0] == "summary") {  
        if (line.size() != 4) 
            return "summary command needs 3 args: {channel_name}, {user}, {file}";
        std::ofstream outFile(line[3]);  
        if (outFile.is_open()) {
            outFile << makeReportForSummary(line[1], line[2]);
            outFile.close();
            return "";
        } else {
            return "Error: Could not write summary file!";
        }
    }
    else if (line[0] == "report") {
        if (line.size() != 2)
            return "summary command needs 1 args: {file}";
        return processReport(line);
    }
    
    return "Invalid command";
}



void StompProtocol::processFromServer(Frame message) {
    if (message.getCommand() == "MESSAGE" || message.getCommand() == "SEND") {  // Handle incoming reports
        if (message.getHeaders().find("destination") == message.getHeaders().end() ||
            message.getHeaders().find("user") == message.getHeaders().end()) {
            std::cerr << " ERROR: Missing 'destination' or 'user' header in received message!\n";
            return;
        }

        std::string channel = message.getHeaders().at("destination");
        std::string user = message.getHeaders().at("user");


        try {
            Event event(message.getBody());

            // Store event in received data
            dataReceivedLock.lock();
            dataReceived[channel][user].push_back(event);
            dataReceivedLock.unlock();
        } catch (const std::exception &e) {
            std::cerr << " ERROR: Failed to parse event: " << e.what() << "\n";
        }
    } 
    else if (message.getCommand() == "CONNECTED") {
        connected = true;
        std::cout << "Login successful" << std::endl;
    } 
    else if (message.getCommand() == "RECEIPT") {
        if (message.getHeaders().find("receipt - id") != message.getHeaders().end()) {
            int receiptId = stoi(message.getHeaders().at("receipt - id"));
            if (receiptActions.count(receiptId)) {
                receiptActions[receiptId]();
            }
        }
    } 
    else if (message.getCommand() == "ERROR") {
        std::cerr << " ERROR Received: " << FrameCodec::encode(message) << std::endl;
        terminateAllClients = true;
        connected = false;
        shouldTerminateBool = true;
    }
}


bool StompProtocol::shouldTerminate() const {
    return shouldTerminateBool;
}

bool StompProtocol::isConnected() const {
    return connected;
}

bool StompProtocol::shouldTerminateAllClients() const {
    return terminateAllClients;
}

string StompProtocol::processReport(vector<string> line) {
    try {
        // Parse JSON file into structured data
        names_and_events parsedData = parseEventsFile(line[1]);
        std::string channelName = parsedData.channel_name;
        const std::vector<Event>& events = parsedData.events;

        if (subscriptions.count(channelName) == 0) {
            return "you are not subscribed to " + channelName;
        }
        
        for (const Event& event : events) {
            Frame sendFrame("SEND");
            sendFrame.addHeader("destination", channelName);
            sendFrame.addHeader("user", username);
            std::ostringstream body;
            // Format the body correctly
            body << "city:" << event.get_city() << "\n"
                 << "event name:" << event.get_name() << "\n"
                 << "date time:" << event.get_date_time() << "\n"
                 << "general information:\n"
                 << "active:" << event.get_general_information().at("active") << "\n"
                 << "forces_arrival_at_scene:" << event.get_general_information().at("forces_arrival_at_scene") << "\n"
                 << "description:\n" << event.get_description() <<"\n\n";
            sendFrame.setBody(body.str());
            // Encode and send the STOMP frame
            std::string encodedFrame = FrameCodec::encode(sendFrame);

            if (!connectionHandler.sendFrameAscii(encodedFrame, '\0')) {
                std::cerr << "Failed to send report " << std::endl;
            }
        }
        
        return "reported";

    } catch (const std::exception& e) {
        return "Error processing report command: " + std::string(e.what());
    }
}



string StompProtocol::makeReportForSummary(string channel, string user) {
    dataReceivedLock.lock();
    // Check if the channel exists
    if (dataReceived.find(channel) == dataReceived.end()) {
        return "ERROR: No data available for channel: " + channel;
    }

    // Check if the user exists
    if (dataReceived[channel].find(user) == dataReceived[channel].end()) {
        return "ERROR: No data available for user: " + user + " in channel: " + channel;
    }

    int activeCount = 0, arrivedCount = 0;
    int totalEvents = dataReceived[channel][user].size();

    for (const Event &event : dataReceived[channel][user]) {
        if (event.get_general_information().count("active") && event.get_general_information().at("active") == "true")
            activeCount++;
        if (event.get_general_information().count("forces_arrival_at_scene") &&
            event.get_general_information().at("forces_arrival_at_scene") == "true")
            arrivedCount++;
    }

    std::ostringstream report;
    report << "Channel: " << channel << "\n";
    report << "Stats:\n";
    report << "Total Events: " << totalEvents << "\n";
    report << "Active: " << activeCount << "\n";
    report << "Forces Arrival: " << arrivedCount << "\n\n";
    report << "Event Reports:\n";

    auto &events = dataReceived[channel][user];
    std::sort(events.begin(), events.end(), [](const Event &a, const Event &b) {
        return a.get_date_time() < b.get_date_time();
    });

    int reportNum = 1;
    for (const Event &event : events) {
        report << "\n Report_" << reportNum++ << "\n";
        report << "city: " << event.get_city() << "\n";
        report << "date time: " << event.get_date_time() << "\n";
        report << "event name: " << event.get_name() << "\n";
        if (event.get_description().size() <= 30) 
            report << "summary: " << event.get_description() << "\n";
        else 
            report << "summary: " << event.get_description().substr(0,27) << "...\n";
    }
    dataReceivedLock.unlock();

    return report.str();
}

