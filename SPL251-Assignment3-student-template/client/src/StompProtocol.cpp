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
    sentMessages()  {}
   //might cause problems because of map <string, int> and not the other way around when we try to delete

void StompProtocol::connect(const std::string& username, const std::string& password){
    if (connected){
        std::cout<<"The client is already logged in, log out before trying again";
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

void StompProtocol::subscribe(const std::string& topic){
    if (!connected){
        std::cout<<"The client is not connected"<<std::endl;
    }

    int subscriptionid = ++subscriptionIdCounter;
    subscriptions[topic] = subscriptionid;
    sentIdCounter++;
    sentMessages[sentIdCounter] = "joined channel " + topic + "\n";

    Frame subframe("SUBSCRIBE");
    subframe.addHeader("destination", topic);
    subframe.addHeader("id", std::to_string(subscriptionid));
    subframe.addHeader("receipt", std::to_string(sentIdCounter));
    std::string encodedFrame = FrameCodec::encode(subframe);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');
}

void StompProtocol::unsubscribe(const std::string& topic){
    if (!connected){
         std::cout<<"The client is not connected"<<std::endl;
    }
    Frame unsubframe("UNSUBSCRIBED");
    int itopic = subscriptions[topic];
    unsubframe.addHeader("id", std::to_string(itopic));
    sentIdCounter++;
    sentMessages[sentIdCounter] = "exited channel " + topic + "\n";
    unsubframe.addHeader("receipt", std::to_string(sentIdCounter));
    std::string encodedFrame = FrameCodec::encode(unsubframe);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');
    subscriptions.erase(topic);

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

    std::string encodedFrame = FrameCodec::encode(sendFrame);
    connectionHandler.sendFrameAscii(encodedFrame, '\0');

    std::cout << "Message sent to topic: " << topic << std::endl;
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
}

bool StompProtocol::processFromKeyboard(std::string userInput){
    vector<string> line;
    string argument;
    stringstream ss(userInput);
    while (getline(ss, argument, ' ')) {
        line.push_back(argument);
    }
    if (line[0] == "login" ) {
        connect(line[1], line[2]);
        return true;
    }
    else if (line[0] == "join"){
        subscribe(line[1]);
        return true;
    }
    else if (line[0] == "exit"){
        unsubscribe(line[1]);
        return true;
    }
    else if (line[0] == "logout"){
        disconnect();
        return true;
    }
    else if (line[0] == "summary" ) { 
        ofstream outFile(line[3]);
        if (outFile.is_open()) {
            outFile << makeReportForSummary(line[1], line[2] );
            outFile.close();
            cout << "summary exported \n" ;
            return true;
        }
        else {
            
            return false;
        }
    }
    else if (line[0] == "report" ) { 
        if (line.size() < 2) {
            std::cerr << "Usage: report <file_path>" << std::endl;
            return false;
        }

        try {
            names_and_events parsedData = parseEventsFile(line[1]);
            std::string channelName = parsedData.channel_name;
            const std::vector<Event>& events = parsedData.events;

               for (const Event& event : events) {
                Frame sendFrame("SEND");
                sendFrame.addHeader("destination", "/" + channelName);
                sendFrame.addHeader("user", username); // Assuming `username` is a class member or passed in.

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
            return true;

        } catch (const std::exception& e) {
            std::cerr << "Error processing report command: " << e.what() << std::endl;
            return false;
        }
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
        std::cout << "Login successful" << std::endl;
    }
    else if (message.getCommand() == "RECEIPT") {
        int recepitId = stoi(message.getHeaders()["receipt - id"]);
        cout << sentMessages[recepitId];
        if (sentMessages[recepitId] == "disconnected from server" ) {
            shouldTerminateBool = true;
            connected = false;
        }
        // cout << "received receipt: " << to_string(recepitId) << endl;
        // if (sentMessages[recepitId].getCommand() == "SUBSCRIBE") {
        //     std::cout<<"joined channel " <<  sentMessages[recepitId].getHeaders().at("destination") << std::endl;
        // }
        // else if (sentMessages[recepitId].getCommand() == "UNSUBSCRIBE") {
        //     std::cout<<"Exited channel " <<  sentMessages[recepitId].getHeaders().at("destination") << std::endl;
        // }
        // else if (sentMessages[recepitId].getCommand() == "SEND") {
            
        // }
        // else if (sentMessages[recepitId].getCommand() == "DISCONNECT") {
        //     subscriptions.clear(); //clean all the subscriptions
            
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







