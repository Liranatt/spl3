#pragma once
#include <string>
#include <iostream>

class keyboardInput {
    public:
        static std::string getInput(const std::string&prompt) {
            std::cout << prompt;
            std::string input;
            std::getline(std::cin, input);
            return input;
        }
};
