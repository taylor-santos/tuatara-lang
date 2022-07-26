//
// Created by taylor-santos on 2/13/2022 at 12:25.
//

#include "driver.hpp"
#include "scanner.hpp"

#include <algorithm>

Driver::Driver(std::istream &input)
    : line_stream_(input) {}

Driver::~Driver() = default;

bool
Driver::parse(const std::string *filename) {
    auto failed  = false;
    auto scanner = yy::Scanner(filename, line_stream_);
    auto parser  = yy::Parser(scanner, errors_, ast_, failed);
    scanner.set_debug_level(0);
    parser.set_debug_level(0);
    parser.parse();
    return failed;
}

TypeChecker::Context
Driver::type_check() {
    auto ctx = TypeChecker::Context(errors_);
    std::for_each(ast_.begin(), ast_.end(), [&ctx](const auto &node) {
        (void)node->get_type(ctx);
    });
    return ctx;
}

const std::vector<std::string> &
Driver::lines() const {
    return line_stream_.lines();
}

const std::vector<std::unique_ptr<AST::Expression>> &
Driver::ast() const {
    return ast_;
}

const std::vector<print::Message> &
Driver::errors() const {
    return errors_;
}
