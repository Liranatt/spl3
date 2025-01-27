#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "../include/ConnectionHandler.h"
#include "StompProtocol.h"
#include "FrameCodec.h"
#include "KeyboardInput.h"
#include "event.h"

using namespace std;

void split_str2(const std::string &str, char delimiter, std::vector<std::string> &tokens) {
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
}



int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <host> <port>" << endl;
        return -1;
    }

    // ConnectionHandler connectionHandler;
    // StompProtocol stompProtocol;
    bool connected = false;
    while (!connected) {
        string userInput = KeyboardInput::getInput("Enter command: ");
        vector<string> line;
        split_str2(userInput, ' ', line);
        // while (stringstream(userInput) >> argument){
        //     line.push_back(argument);
        //     cout << "argument: " << argument << endl;
        // }
         if (line.empty() || line[0] != "login") {
            cerr << "Unknown command: " << userInput << endl;
            continue;
        }

        // Check for correct login command structure
        if (line.size() < 4) {
            cerr << "Invalid login command" << endl;
            continue;
        }
            try {
                size_t colonPos = line[1].find(':');
                if(colonPos == string::npos) {
                    cerr << "Invalid host:port format" <<endl;
                    continue;
                }

                string host = line[1].substr(0, colonPos);
                short port = stoi(one[1].substr(colonPos + 1));

                ConnectionHandler connectionHandler(host, port);
                if(!connectionHandler.connect()){
                    cerr << "Failed to connect" << endl;
                    continue;
                }
                cout << "arrived here1" << endl;
                StompProtocol stompProtocol(connectionHandler);
                cout << "arrived here2" << endl;
                stompProtocol.connect(line[2], line[3]);
                cout << "arrived here3" << endl;
                connected = true;
                cout << "arrived here4" << endl;
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
                cout << "arrived here5" << endl;
                while (!stompProtocol.shouldTerminate()) {
                    string userInput = KeyboardInput::getInput("Enter command: ");
                    if (!stompProtocol.processFromKeyboard(userInput))
                        cerr << "Unknown command: " << userInput << endl;
                }
                cout << "arrived here6" << endl;

            } catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
            }
        }
        return 0;
    }


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
