#include "Client.h"
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include "Frame.h"
#include "StompProtocol.h"
#include "KeyboardInput.h"
#include "FrameCodec.h"

using namespace std;

Client::Client(std::string host, short port): 
    connectionHandler(host, port), 
    protocol(connectionHandler),
    toPrintOnScreen(),
    fromKeyboard(),
    messagesReceived(),
    toPrintLock(),
    toProcessLock(),
    messagesCounter(0) {}

void Client::run() {
    thread keyboardThread(manageKeyboard);
    thread connectionsThread(manageConnections);

}

void Client::manageKeyboard() {
    try {
        while (protocol.isConnected()) {
            // write to the screen all that needs to be printed
            if (!toPrintOnScreen.empty()) {
                toPrintLock.lock();
                while (!toPrintOnScreen.empty()) {
                    cout << toPrintOnScreen[0] << endl;
                    toPrintOnScreen.erase(toPrintOnScreen.begin());
                }
                toPrintLock.unlock();
            }
            // read from the screen and pass it to the connections thread
            string userInput = KeyboardInput::getInput("Enter command: ");
            toProcessLock.lock();
            fromKeyboard.push_back(userInput);
            toProcessLock.unlock();
        }
    } catch (const exception& e) {
        cerr << "Error in manageKeyboard: " << e.what() << endl;
    }
}

void Client::manageConnections() {
    try {
        while (protocol.isConnected()) {
            // process from keyboard
            if (!fromKeyboard.empty()) {
                vector<string> toAddToPrintVector;
                toProcessLock.lock();
                for (string toProcess : fromKeyboard) {
                    string response = protocol.process(toProcess);
                    if (!response.empty()) {
                        toAddToPrintVector.push_back(response);
                    }
                }
                toProcessLock.unlock();
                for (string line : toPrintOnScreen) {
                    toPrintLock.lock();
                    toPrintOnScreen.push_back(line);
                    toPrintLock.unlock();
                }
            }
            // listen and process from socket
            std::string response;
            connectionHandler.getFrameAscii(response, '\0');
            Frame responseFrame = FrameCodec::decode(response);
            processFrame(responseFrame);
        }
    } catch (const exception& e) {
        cerr << "Error in manageConnections: " << e.what() << endl;
    }
}

void Client::processFrame(Frame frame) {
    // process the probebly message Frame sent from the server and add its information to messagesReceived
}