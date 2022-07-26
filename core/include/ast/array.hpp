//
// Created by taylor-santos on 7/25/2022 at 14:17.
//

#pragma once

#include "ast/expression.hpp"

#include <vector>

namespace AST {

class Array final : public Expression {
public:
    Array(std::vector<std::unique_ptr<AST::Expression>> &&values, const yy::location &loc);
    explicit Array(const yy::location &loc);

    ~Array() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::vector<std::unique_ptr<AST::Expression>> values_;
};

} // namespace AST
