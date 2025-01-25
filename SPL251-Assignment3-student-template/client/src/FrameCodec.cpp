#include "../include/FrameCodec.h"
#include <sstream>
#include <stdexcept>

std::string FrameCodec::encode(const Frame& frame) {
    std::ostringstream output;
    output << frame.getCommand() << "\n";
    for (const auto& header : frame.getHeaders()) {
        output << header.first << ":" << header.second << "\n";
    }
    output << "\n";
     if (!frame.getBody().empty()) {
        output << frame.getBody();
    }

    output << '\0'; 
    return output.str();
}

Frame FrameCodec::decode(const std::string& frameString) {
    std::istringstream input(frameString);
    std::string line;
    Frame frame;
    bool foundCommand = false;
    while (std::getline(input, line) && !line.empty()) {
        if (!foundCommand) {
            frame.setCommand(line);
            foundCommand = true;
        }
        size_t separatorPos = line.find(':');
        std::string key = line.substr(0, separatorPos);
        std::string value = line.substr(separatorPos + 1);
        frame.addHeader(key, value);
    }

    std::string body;
    while (std::getline(input, line)) {
        if (!body.empty()) body += "\n";
        body += line;
    }
    frame.setBody(body);
    return frame;
}