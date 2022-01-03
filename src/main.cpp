//
// Created by taylor-santos on 12/30/2021 at 18:38.
//

#include <iostream>
#include <fstream>

#include "scanner.hpp"

int
main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    std::ifstream input;
    input.exceptions(std::ios::badbit | std::ios::failbit);
    try {
        input.open(argv[1]);
    } catch (std::ios_base::failure &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    auto path    = std::filesystem::relative(argv[1]);
    auto scanner = yy::Scanner(path.string(), input, std::cerr);
    auto parser  = yy::Parser(scanner);
    scanner.set_debug_level(0);
    parser.set_debug_level(0);
    parser.parse();
    return scanner.had_error();
}
