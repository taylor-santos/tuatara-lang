//
// Created by taylor-santos on 1/2/2022 at 21:58.
//

#pragma once

#include "ast/expression.hpp"
#include "ast/type.hpp"

namespace AST {

class Error final
    : public Expression
    , public Type {
public:
    explicit Error(const yy::location &loc);
    ~Error() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;
};

} // namespace AST
