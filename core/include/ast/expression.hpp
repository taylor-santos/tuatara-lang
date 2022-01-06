//
// Created by taylor-santos on 1/2/2022 at 20:11.
//

#pragma once

#include "ast/ast.hpp"

namespace AST {

class Expression : public virtual Node {
public:
    Expression();
    ~Expression() override;
};

} // namespace AST
