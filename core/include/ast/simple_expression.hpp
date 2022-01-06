//
// Created by taylor-santos on 1/2/2022 at 20:30.
//

#pragma once

#include "ast/expression.hpp"

namespace AST {

class SimpleExpression : public Expression {
public:
    SimpleExpression();
    ~SimpleExpression() override;
};

} // namespace AST
