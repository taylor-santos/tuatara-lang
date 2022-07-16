//
// Created by taylor-santos on 12/30/2021 at 18:38.
//

#include <iostream>
#include <fstream>
#include <cstring>

#include "driver.hpp"
#include "ast/expression.hpp"
#include "rang.hpp"
#include "printer.hpp"

#define POS(FILE, LINE, COL) \
    yy::position { (FILE), (LINE), (COL) }
#define LOC(FILE, BEGIN_LINE, BEGIN_COL, END_LINE, END_COL) \
    yy::location { POS((FILE), (BEGIN_LINE), (BEGIN_COL)), POS((FILE), (END_LINE), (END_COL)) }

int
main(int argc, char **argv) {
    rang::setControlMode(rang::control::Auto);
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
    auto driver = Driver(input);

    auto path = std::filesystem::relative(argv[1]).string();
    driver.parse(&path);

    for (auto &node : driver.ast()) {
        node->to_json(std::cout);
        std::cout << "\n";
    }

    auto ctx = driver.type_check();

    for (auto &message : driver.errors()) {
        message.print(driver.lines(), std::cout);
    }

    return 0;
}
