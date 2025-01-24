#pragma once
#include "Frame.h"

class SendFrame : public Frame {
    public:
        SendFrame(const std::string& destination, cnost std::string& body):
        Frame("SEND") {
            addHeader("destination", destination);
            setBody(body);
        }

};
