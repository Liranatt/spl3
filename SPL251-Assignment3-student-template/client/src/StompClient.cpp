#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "ConnectionHandler.h"
#include "StompProtocol.h"
#include "FrameCodec.h"
#include "KeyboardInput.h"
#include "event.h"

using namespace std;

/**
 * receive a command from the keyboard
 * turn it to vector of strings {line}
 * extract the host and port and assign it to {connectionHandler}
 * connect the {connectionHandler} to the server
 * send a CONNECT frame to the server
 * if ERROR is received, try all over again
 * else, we have a connected {connectionHandler}, and we can go on with the client program
 * return username used to login to the server
 * @param connectionHandler 
 */
string handleLoginCommandUntilConnectedToSerer(ConnectionHandler& connectionHandler, string lastLine) {
    while (true) {
        // store the command in a vector
        string userInput;
        if (lastLine != "") {
            userInput = lastLine;
            lastLine = "";
        }
        else
            userInput = KeyboardInput::getInput("");
        vector<string> line;
        string argument;
        stringstream ss(userInput);
        while (getline(ss, argument, ' ')) {
            line.push_back(argument);
        }
        // Check for correct login command structure
        if (line.empty()) 
            continue;
        else if (line[0] != "login") {
            cerr << "Invalid command" << endl;
            continue;
        }
        else if (line.size() != 4) {
            cerr << "login command needs 3 args: {host:port}, {username}, {passcode}" << endl;
            continue;
        }
        else if ((line[0] == "login") & (line.size() == 4)) {
            // extracting the host and port from the command
            size_t colonPos = line[1].find(':');
            if(colonPos == string::npos) {
                cerr << "Invalid host:port format" <<endl;
                continue;
            }
            string enteredHost = line[1].substr(0, colonPos);
            short enteredPort = stoi(line[1].substr(colonPos + 1));
            connectionHandler.setHost(enteredHost);
            connectionHandler.setPort(enteredPort);
            // trying to connect to these host and port
            try {
                connectionHandler.close();
            } catch (const exception& ignored) {}
            if(!connectionHandler.connect()) {
                cerr << "Could not connect to server" << endl;
                continue;
            }
            // sending CONNECT frame to the server
            Frame connectFrame("CONNECT");
            connectFrame.addHeader("accept-version", "1.2");
            connectFrame.addHeader("host", "stomp.cs.bgu.ac.il");
            connectFrame.addHeader("login", line[2]);
            connectFrame.addHeader("passcode", line[3]);
            std::string encodedFrame = FrameCodec::encode(connectFrame);
            if(!connectionHandler.sendFrameAscii(encodedFrame, '\0')) {
                cerr<< "could not connect to server" << endl;
            }
            // waiting and checking the respones from the server
            std::string response;
            connectionHandler.getFrameAscii(response, '\0');
            Frame responseFrame = FrameCodec::decode(response);
            if (responseFrame.getCommand() == "ERROR") {
                cout << responseFrame.getHeaders().at("message") << endl;
                continue;
            }
            else if (responseFrame.getCommand() == "CONNECTED") {
                cout << "login successful" << endl;
                return line[2];
            }
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <host> <port>" << endl;
        return -1;
    }
    cout << "started" << endl;

    /**
     * the client can't be turned off, so its on never ending while true loop.
     *  each iterition in the loop is a new client,
     *  starting with login, then executing commands,
     *  until requesting to logout.
     */
    string lastLine = "";
    while (true) {
        ConnectionHandler connectionHandler;

        string username = handleLoginCommandUntilConnectedToSerer(connectionHandler, lastLine);
        lastLine = "";
        // initiate a thread to listen to the server
        StompProtocol stompProtocol(connectionHandler);
        stompProtocol.setUsername(username);
        thread socketThread( [&stompProtocol, &connectionHandler ]( ) {
            while (!stompProtocol.shouldTerminate() & stompProtocol.isConnected()) {
                std::string response;
                connectionHandler.getFrameAscii(response, '\0');
                stompProtocol.processFromServer(FrameCodec::decode(response));
            }
        });
        // listen to the keyboard and execute its commands
        while (!stompProtocol.shouldTerminate()) {
            string userInput = KeyboardInput::getInput("");
            if (userInput.length() > 0) {
                if (stompProtocol.shouldTerminate()) {
                    lastLine = userInput;
                }
                else {
                    string response = stompProtocol.processFromKeyboard(userInput);
                    if (response.length()  > 0)
                        cerr << response << endl;
                }
            }
        }
        socketThread.join();
        if (stompProtocol.shouldTerminateAllClients())
            break;
    } // enb of never ending while true;
    return 0;
}
