//
// Created by taylor-santos on 1/2/2022 at 20:56.
//

#pragma once

#include "ast/ast.hpp"

namespace AST {

class Type : public virtual Node {
public:
    Type();
    ~Type() override;
};

} // namespace AST
