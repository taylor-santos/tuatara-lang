//
// Created by taylor-santos on 12/30/2021 at 18:38.
//

#include <iostream>
#include <fstream>
#include <cstring>

#include "scanner.hpp"
#include "ast/expression.hpp"
#include "ast/int.hpp"

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
    } catch (std::ios_base::failure &) {
        std::cerr << argv[1] << ": " << std::strerror(errno) << std::endl;
        return 1;
    }
    auto path    = std::filesystem::relative(argv[1]);
    auto scanner = yy::Scanner(path.string(), input);
    auto ast     = std::vector<std::unique_ptr<AST::Expression>>();
    auto failed  = false;
    auto parser  = yy::Parser(scanner, std::cerr, ast, failed);
    scanner.set_debug_level(0);
    parser.set_debug_level(0);
    parser.parse();
    std::cout << "[";
    std::string sep;
    for (const auto &expr : ast) {
        std::cout << sep;
        sep = ",";
        expr->to_json(std::cout);
    }
    std::cout << "]";
    return failed;
}
