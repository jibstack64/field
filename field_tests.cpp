#include <iostream>

#include "field.hpp"

void say(field::context& ctx, std::vector<const char *> values) {
    for (const auto& v : values) {
       std::cout << v << std::endl;
    }
    for (auto& fl : ctx.passedFlags) {
        std::cout << "from 'say': " << fl->name << std::endl;
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

    parser.parse(argc, argv);
}