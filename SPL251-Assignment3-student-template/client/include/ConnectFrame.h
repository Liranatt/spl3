#pragma once
#include "Frame.h"

class ConnectFrame : public Frame {
public:
    ConnectFrame(const std::string& username, const std::string& password):
        Frame("CONNECT") {
        addHeader("login", username);
        addHeader("passcode", password);
        addHeader("accept-version", "1.2");
        addHeader("host", "stomp.cs.bgu.ac.il");
    }
};
