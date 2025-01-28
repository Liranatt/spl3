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
 * 
 * @param connectionHandler 
 */
void handleLoginCommandUntilConnectedToSerer(ConnectionHandler& connectionHandler) {
    while (true) {
        // store the command in a vector
        string userInput = KeyboardInput::getInput("Enter login command: ");
        vector<string> line;
        string argument;
        stringstream ss(userInput);
        while (getline(ss, argument, ' ')) {
            line.push_back(argument);
        }
        // Check for correct login command structure
        if ((line.empty()) || (line[0] != "login" | line.size() != 4)) {
            cerr << "Invalid login command: " << userInput << endl;
            continue;
        }
        else if (line[0] == "login" & line.size() == 4) {
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
                cerr << "Could not connect to server1" << endl;
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
                cerr<< "Could not connect to server2" << endl;
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
                break;
            }
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <host> <port>" << endl;
        return -1;
    }

    /**
     * the client can't be turned off, so its on never ending while true loop.
     *  each iterition in the loop is a new client,
     *  starting with login, then executing commands,
     *  until requesting to logout.
     */
    while (true) {
        ConnectionHandler connectionHandler;

        handleLoginCommandUntilConnectedToSerer(connectionHandler);
        // initiate a thread to listen to the server
        StompProtocol stompProtocol(connectionHandler);
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
            if (!stompProtocol.processFromKeyboard(userInput))
                cerr << "Unknown command: " << userInput << endl;
        }
        socketThread.join();
    } // enb of never ending while true;
    return 0;
}



                // stompProtocol.processFromServer(FrameCodec::decode(response));
                // else {
                    // StompProtocol stompProtocol(connectionHandler);
                    // stompProtocol.connect(line[2], line[3]);
                    // connected = true;
                    // thread socketThread( [&stompProtocol, &connectionHandler ]( ) {
                    //     while (!stompProtocol.isConnected()) {
                    //         sleep(97);
                    //     }
                    //     while (!stompProtocol.shouldTerminate() & stompProtocol.isConnected()) {
                    //         std::string response;
                    //         connectionHandler.getFrameAscii(response, '\0');
                    //         stompProtocol.processFromServer(FrameCodec::decode(response));
                    //     }
                    // });
                    // while (!stompProtocol.shouldTerminate()) {
                    //     string userInput = KeyboardInput::getInput("Enter command: ");
                    //     if (!stompProtocol.processFromKeyboard(userInput))
                    //         cerr << "Unknown command: " << userInput << endl;
                    // }
                // }

// void listenToKeyboard()

    // ConnectionHandler connectionHandler(host, port);
    // if (!connectionHandler.connect()) {
    //     cerr << "Cannot connect to " << host << ":" << port << endl;
    //     return -1;
    // }
    // StompProtocol stompProtocol(connectionHandler);

    // listen to Socket And Process From Server
    // thread socketThread( [&stompProtocol, &connectionHandler ]( ) {
    //     while (!stompProtocol.isConnected()) {
    //         sleep(97);
    //     }
    //     while (!stompProtocol.shouldTerminate() & stompProtocol.isConnected()) {
    //         std::string response;
    //         connectionHandler.getFrameAscii(response, '\0');
    //         stompProtocol.processFromServer(FrameCodec::decode(response));
    //     }
    // });

    // while (!stompProtocol.shouldTerminate()) {
    //     string userInput = KeyboardInput::getInput("Enter command: ");
    //     if (!stompProtocol.processFromKeyboard(userInput))
    //         cerr << "Unknown command: " << userInput << endl;
    // }

    // socketThread.detach();
    // socketThread.join();




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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////old version
//#include <iostream>
//#include <string>
//#include <thread>
//#include <mutex>
//#include "ConnectionHandler.h"
//#include "StompProtocol.h"
//#include "FrameCodec.h"
//#include "KeyboardInput.h"
//#include "event.h"
//
//using namespace std;
//
//
//
//int main(int argc, char *argv[]) {
//    if (argc < 3) {
//        cerr << "Usage: " << argv[0] << " <host> <port>" << endl;
//        return -1;
//    }
//
//    string host = argv[1];
//    short port = stoi(argv[2]);
//
//    ConnectionHandler connectionHandler(host, port);
//    if (!connectionHandler.connect()) {
//        cerr << "Cannot connect to " << host << ":" << port << endl;
//        return -1;
//    }
//    StompProtocol stompProtocol(connectionHandler);
//
//    // listen to Socket And Process From Server
//    thread socketThread( [&stompProtocol, &connectionHandler ]( ) {
//        while (!stompProtocol.isConnected()) {
//            sleep(97);
//        }
//        while (!stompProtocol.shouldTerminate() & stompProtocol.isConnected()) {
//            std::string response;
//            connectionHandler.getFrameAscii(response, '\0');
//            stompProtocol.processFromServer(FrameCodec::decode(response));
//        }
//    });
//
//    while (!stompProtocol.shouldTerminate()) {
//        string userInput = KeyboardInput::getInput("Enter command: ");
//        if (!stompProtocol.processFromKeyboard(userInput))
//            cerr << "Unknown command: " << userInput << endl;
//    }
//
//    socketThread.detach();
//    socketThread.join();
//
//    return 0;
//}
//
//
///////////////////////////////////////////////////////////// old version
//
//// using namespace std;
//
//// int main(int argc, char *argv[]) {
////     if (argc < 3) {
////         cerr << "Usage: " << argv[0] << " <host> <port>" << endl;
////         return -1;
////     }
//
////     string host = argv[1];
////     short port = stoi(argv[2]);
//
////     ConnectionHandler connectionHandler(host, port);
////     if (!connectionHandler.connect()) {
////         cerr << "Cannot connect to " << host << ":" << port << endl;
////         return -1;
////     }
//
////     StompProtocol stompProtocol(connectionHandler);
//
////     try {
////         string username = KeyboardInput::getInput("Enter username: ");
////         string password = KeyboardInput::getInput("Enter password: ");
////         stompProtocol.connect(username, password);
////         string userInput;
////         while (true) {
////             userInput = KeyboardInput::getInput("Enter command: ");
//
////             if (userInput == "exit") {
////                 stompProtocol.disconnect();
////                 break;
////             } else if (userInput.find("subscribe") == 0) {
////                 string topic = userInput.substr(10);
////                 stompProtocol.subscribe(topic);
////             } else if (userInput.find("send")  == 0) {
////                 size_t spacePos = userInput.find(" ");
////                 string topic = userInput.substr(5, spacePos - 5);
////                 string message = userInput.substr(spacePos + 1);
////                 stompProtocol.send(topic, message);
////             } else if (userInput.find("unsubscribe")  == 0) {
////                 string topic = userInput.substr(12);
////                 stompProtocol.unsubscribe(topic);
////             } else {
////                 cerr << "Unknown command: " << userInput << endl;
////             }
////         }
////     } catch (const exception& e) {
////         cerr << "Error: " << e.what() << endl;
////     }
//
////     return 0;
//// }
