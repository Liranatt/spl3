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
    


int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <host> <port>" << endl;
        return -1;
    }

    string host = argv[1];
    short port = stoi(argv[2]);

    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        cerr << "Cannot connect to " << host << ":" << port << endl;
        return -1;
    }
    StompProtocol stompProtocol(connectionHandler);

    // listen to Socket And Process From Server
    thread socketThread( [&stompProtocol, &connectionHandler ]( ) {
        while (!stompProtocol.isConnected()) {
            sleep(97);
        }
        while (!stompProtocol.shouldTerminate() & stompProtocol.isConnected()) { 
            std::string response;
            connectionHandler.getFrameAscii(response, '\0');
            stompProtocol.processFromServer(FrameCodec::decode(response));
        }
    });

    while (!stompProtocol.shouldTerminate()) {
        string userInput = KeyboardInput::getInput("Enter command: ");
        if (!stompProtocol.processFromKeyboard(userInput)) 
            cerr << "Unknown command: " << userInput << endl;
    }

    socketThread.detach();
    socketThread.join();

    return 0;
}


/////////////////////////////////////////////////////////// old version

// using namespace std;

// int main(int argc, char *argv[]) {
//     if (argc < 3) {
//         cerr << "Usage: " << argv[0] << " <host> <port>" << endl;
//         return -1;
//     }

//     string host = argv[1];
//     short port = stoi(argv[2]);

//     ConnectionHandler connectionHandler(host, port);
//     if (!connectionHandler.connect()) {
//         cerr << "Cannot connect to " << host << ":" << port << endl;
//         return -1;
//     }

//     StompProtocol stompProtocol(connectionHandler);

//     try { 
//         string username = KeyboardInput::getInput("Enter username: ");
//         string password = KeyboardInput::getInput("Enter password: ");
//         stompProtocol.connect(username, password); 
//         string userInput;
//         while (true) {
//             userInput = KeyboardInput::getInput("Enter command: ");

//             if (userInput == "exit") {
//                 stompProtocol.disconnect();
//                 break;
//             } else if (userInput.find("subscribe") == 0) {
//                 string topic = userInput.substr(10); 
//                 stompProtocol.subscribe(topic);
//             } else if (userInput.find("send")  == 0) {
//                 size_t spacePos = userInput.find(" ");
//                 string topic = userInput.substr(5, spacePos - 5); 
//                 string message = userInput.substr(spacePos + 1); 
//                 stompProtocol.send(topic, message);
//             } else if (userInput.find("unsubscribe")  == 0) {
//                 string topic = userInput.substr(12); 
//                 stompProtocol.unsubscribe(topic);
//             } else {
//                 cerr << "Unknown command: " << userInput << endl;
//             }
//         }
//     } catch (const exception& e) {
//         cerr << "Error: " << e.what() << endl;
//     }

//     return 0;
// }
