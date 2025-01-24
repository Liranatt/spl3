#pragma once
#include "Frame.h"

class UnsubscribedFrame : public Frame {
    public:
        UnsubscribedFrame(int id):
        Frame("UNSUBSCRIBE"){
            addHeader("id", std::to_string(id));
        }
};
