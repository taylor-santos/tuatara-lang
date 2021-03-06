//
// Created by taylor-santos on 2/13/2022 at 12:25.
//

#pragma once

#include <istream>

#include "type/type_checker.hpp"
#include "line_stream.hpp"

namespace AST {
class Expression;
}

class Driver {
public:
    explicit Driver(std::istream &input);
    Driver(const Driver &) = delete;
    ~Driver();

    bool
    parse(const std::string *filename);

    TypeChecker::Context
    type_check();

    [[nodiscard]] const std::vector<std::string> &
    lines() const;

    [[nodiscard]] const std::vector<print::Message> &
    errors() const;

private:
    LineStream                                    line_stream_;
    std::vector<print::Message>                   errors_;
    std::vector<std::unique_ptr<AST::Expression>> ast_;
};
