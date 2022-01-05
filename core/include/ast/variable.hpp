//
// Created by taylor-santos on 1/2/2022 at 21:27.
//

#pragma once

#include <string>

#include "ast/simple_expression.hpp"

namespace AST {

class Variable final : public SimpleExpression {
public:
    Variable(std::string name, const yy::location &loc);
    ~Variable() override;

    void
    to_json(std::ostream &os) const override;

private:
    std::string name_;
};

} // namespace AST
