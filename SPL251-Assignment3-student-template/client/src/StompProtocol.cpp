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
    connected(true), 
    shouldTerminateBool(false), 
    subscriptionIdCounter(0), 
    sentIdCounter(0), 
    subscriptions(),
    dataReceivedLock(), 
    dataReceived(),
    sentMessages(),
    receiptActions(),
    username()  {}
   //might cause problems because of map <string, int> and not the other way around when we try to delete

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
    // subscriptions[subscriptionId] = topic;
    sentIdCounter++;
    sentMessages[sentIdCounter] = "joined channel " + topic + "\n";
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
    // i changed subscriptions from <string, int> to <int, string> back although it doesnt really matter, we need to use find_if function to check if the
    // the department we want to delete exists and then procceed (thats why nothing happen)
    // bool exists = find(begin(subscriptions), end(subscriptions), topic) != end(subscriptions);
    // if (exists){
    //     if (subscriptions[subscriptions.size() -1] != topic)
    //     cout << "the user is not subscripted to that department" << endl;
    // }
    // int itopic = find(begin(subscriptions), end(subscriptions), topic);

    if (subscriptions.count(topic) == 0) {
        return "you are not subscribed to " + topic;
    }
    Frame unsubframe("UNSUBSCRIBED");
    unsubframe.addHeader("id", to_string(subscriptions[topic]));
    sentIdCounter++;
    sentMessages[sentIdCounter] = "exited channel " + topic + "\n";
    receiptActions[sentIdCounter] = [&topic, this]() {
        cout << "exited channel " << topic << endl;
        subscriptions.erase(topic);
    };
    unsubframe.addHeader("receipt", std::to_string(sentIdCounter));
    std::string encodedFrame = FrameCodec::encode(unsubframe);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');
    return "";
    // subscriptions.erase(topic);

    // subscriptionidCounter-- ? = put into processFromServer

}

void StompProtocol::send(const std::string& topic, const std::string& message) {
    if (!isConnected()) {
        std::cout<<"The client is not connected"<<std::endl;
    }

    Frame sendFrame("SEND");
    sendFrame.addHeader("destination", topic);
    sendFrame.setBody(message);
    sentIdCounter++;
    sendFrame.addHeader("reciept", std::to_string(sentIdCounter));
    sentMessages[sentIdCounter] = "reported an event";
    receiptActions[sentIdCounter] = []() {
        cout << "reported an event " << endl;
    };

    std::string encodedFrame = FrameCodec::encode(sendFrame);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');

    // std::cout << "Message sent to topic: " << topic << std::endl;
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
    sentMessages[sentIdCounter] = "disconnected from server";
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
    if ((line[0] == "login") & (line.size() == 3)) {
        connect(line[1], line[2]);
        return "";
    }
    else if ((line[0] == "join") & (line.size() == 2)) {
        return subscribe(line[1]);
    }
    else if ((line[0] == "exit") & (line.size() == 2)) {
        return unsubscribe(line[1]);
    }
    else if ((line[0] == "logout") & (line.size() == 1)) {
        disconnect();
        return "";
    }
    else if ((line[0] == "summary") & (line.size() == 3)) {
        ofstream outFile(line[3]);
        if (outFile.is_open()) {
            outFile << makeReportForSummary(line[1], line[2] );
            outFile.close();
            return "summary exported";
        }
        else {
            return "summary error";
        }
    }
    else if ((line[0] == "report") & (line.size() == 2)) {
        return processReport(line);
    }
    return "invalid command";
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
        std::cout << "Login successful" << std::endl;
    }
    else if (message.getCommand() == "RECEIPT") {
        int recepitId = stoi(message.getHeaders()["receipt - id"]);
        receiptActions[recepitId]();
        // cout << sentMessages[recepitId];
        // if (sentMessages[recepitId] == "disconnected from server" ) {
        //     shouldTerminateBool = true;
        //     connected = false;
        // }
    }
    else if (message.getCommand() == "ERROR") {
        cout << message.getHeaders().at("message") << endl;
    }
}

bool StompProtocol::shouldTerminate() const {
    return shouldTerminateBool;
}

bool StompProtocol::isConnected() const {
    return connected;
}

string StompProtocol::processReport(vector<string> line) {
    if (line.size() < 2) {
        return "Usage: report <file_path>";
    }

    try {
        names_and_events parsedData = parseEventsFile(line[1]);
        std::string channelName = parsedData.channel_name;
        const std::vector<Event>& events = parsedData.events;

            for (const Event& event : events) {
            Frame sendFrame("SEND");
            sendFrame.addHeader("destination", "/" + channelName);
            sendFrame.addHeader("user", username);

            std::ostringstream body;
            body << "city: " << event.get_city() << "\n"
                    << "event name: " << event.get_name() << "\n"
                    << "date time: " << event.get_date_time() << "\n"
                    << "general information:\n"
                    << "active: " << (event.get_general_information().at("active") == "true" ? "true" : "false") << "\n"
                    << "forces_arrival_at_scene: " << (event.get_general_information().at("forces_arrival_at_scene") == "true" ? "true" : "false") << "\n"
                    << "description:\n" << event.get_description();

            sendFrame.setBody(body.str());

            std::string encodedFrame = FrameCodec::encode(sendFrame);
            if (!connectionHandler.sendFrameAscii(encodedFrame, '\0')) {
                std::cerr << "Failed to send report for event: " << event.get_name() << std::endl;
            }
        }

        std::cout << "All events from " << line[1] << " have been reported successfully." << std::endl;
        return "";

    } catch (const std::exception& e) {
        string response = "Error processing report command: ";
        response += e.what();
        return response;
    }
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







