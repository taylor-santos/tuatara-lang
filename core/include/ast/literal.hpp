//
// Created by taylor-santos on 1/2/2022 at 20:29.
//

#pragma once

#include "ast/simple_expression.hpp"

namespace AST {

class Literal : public SimpleExpression {
public:
    Literal();
    ~Literal() override;
};

} // namespace AST
