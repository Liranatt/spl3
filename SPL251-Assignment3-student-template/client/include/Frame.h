#pragma once
#include <string>
#include <unordered_map>


class Frame {
    protected:
        std::string command;
        std::underordered_map<std::string, std::string> headers;
        std::string body;

    public:
        frame(const std::string& cmd) : command(cmd) {}

    void addHeader(const std::string& key, const std::string& value){
        headers[key] = value;
        
            }
    void addHeader(const std::string& key, const std::string& value) {
        headers[key] = value;
    }
    void setBody(const std::string& b){
        body = b;
    }
    virtual ~frame():
}
