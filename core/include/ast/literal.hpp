//
// Created by taylor-santos on 1/2/2022 at 20:29.
//

#pragma once

#include "ast/expression.hpp"

namespace AST {

class Literal : public Expression {
public:
    Literal();
    ~Literal() override;
};

} // namespace AST
