#include <iostream>
#include <string>
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"
#include "../include/keyboardInput.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return -1;
    }

    std::string host = argv[1];
    short port = std::stoi(argv[2]);

    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return -1;
    }

    StompProtocol stompProtocol(connectionHandler);

    try {
        std::string username = keyboardInput::getInput("Enter username: ");
        std::string password = keyboardInput::getInput("Enter password: ");
        stompProtocol.connect(username, password); 
        std::string userInput;
        while (true) {
            userInput = keyboardInput::getInput("Enter command: ");

            if (userInput == "exit") {
                stompProtocol.disconnect();
                break;
            } else if (userInput.find("subscribe") == 0) {
                std::string topic = userInput.substr(10); 
                stompProtocol.subscribe(topic);
            } else if (userInput.find("send")  == 0) {
                size_t spacePos = userInput.find(" ");
                std::string topic = userInput.substr(5, spacePos - 5); 
                std::string message = userInput.substr(spacePos + 1); 
                stompProtocol.send(topic, message);
            } else if (userInput.find("unsubscribe")  == 0) {
                std::string topic = userInput.substr(12); 
                stompProtocol.unsubscribe(topic);
            } else {
                std::cerr << "Unknown command: " << userInput << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
