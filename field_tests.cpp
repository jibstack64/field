#include <iostream>

#include "field.hpp"

void say(field::context& ctx, std::vector<const char *> values) {
    std::cout << "passed values:" << std::endl;
    for (const auto& v : values) {
       std::cout << v << std::endl;
    }
    std::cout << "passed flags:" << std::endl;
    for (auto& fl : ctx.passedFlags) {
        std::cout << fl->name << std::endl;
    }
    std::cout << "overflowing values:" << std::endl;
    for (auto& of : ctx.overflowValues) {
        std::cout << of << std::endl;
    }
}

void kill(field::context& ctx) {
    std::cout << "kill all humanz!" << std::endl;
}

int main(int argc, char ** argv) {
    field::parser parser;
    parser.disableLock = true;

    parser.add("say", say, 3);
    parser.add("mute", say, 1);
    parser.addFlag("-kill", kill);
    parser.add("no-func", 1);

    field::arg* ar = parser.parse(argc, argv);

    if (ar->name == "no-func") {
        std::cout << "passed values:" << std::endl;
        for (const auto& v : ar->values()) {
        std::cout << v << std::endl;
        }
        std::cout << "passed flags:" << std::endl;
        for (auto& fl : parser.ctx.passedFlags) {
            std::cout << fl->name << std::endl;
        }
        std::cout << "overflowing values:" << std::endl;
        for (auto& of : parser.ctx.overflowValues) {
            std::cout << of << std::endl;
        }
    }
}
