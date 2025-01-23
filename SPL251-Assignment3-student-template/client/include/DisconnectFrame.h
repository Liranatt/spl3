#pragma once
#include "Frame.h"

class DisconnectFrame : public Frame {
    public:
    DisconnectFrame(): Frame("DISCONNECT"){}

};
