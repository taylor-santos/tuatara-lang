//
// Created by taylor-santos on 7/15/2022 at 16:04.
//

#pragma once

#include "ast/simple_expression.hpp"

#include <vector>

namespace AST {

class Tuple final : public SimpleExpression {
public:
    Tuple(std::vector<std::unique_ptr<AST::SimpleExpression>> &&values, const yy::location &loc);
    explicit Tuple(const yy::location &loc);

    ~Tuple() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::vector<std::unique_ptr<AST::SimpleExpression>> values_;
};

} // namespace AST
