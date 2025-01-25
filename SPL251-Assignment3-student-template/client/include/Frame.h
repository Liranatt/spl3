#pragma once
#include <string>
#include <map>


class Frame {
    private:
        std::string command;
        std::map<std::string, std::string> headers;
        std::string body;

    public:
    Frame() = default;
    Frame(const std::string& cmd):command(cmd), headers(), body() {}

    std::string getCommand() const {return command;}
    void setCommand(const std::string& cmd) {command = cmd;}

    std::map<std::string, std::string> getHeaders() const{return headers;}

    void addHeader(const std::string& key, const std::string& value){
        headers[key] = value;
        
            }
    std::string getBody() const{return body;}
    void setBody(const std::string& b){
        body = b;
    }
};
