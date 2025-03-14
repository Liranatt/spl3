#pragma once
#include <string>
#include <map>
#include "Frame.h"

class FrameCodec {
    public:
    static std::string encode(const Frame& frame);
    static Frame decode(const std::string& frameString);


};