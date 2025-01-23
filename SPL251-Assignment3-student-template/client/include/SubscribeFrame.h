#pragma once
#include "Frame.h"

class SubscribeFrame : public Frame {
    public:
        SubscribeFrame(const std::string& destination, int id):
        Frame("SUBSCRIBE"){
            addHeader("destination", destination);
            addHeader("id", std::to_string(id));
        }
};
