//
// Created by taylor-santos on 7/15/2022 at 16:04.
//

#pragma once

#include "ast/expression.hpp"

#include <vector>

namespace AST {

class Tuple final : public Expression {
public:
    Tuple(std::vector<std::unique_ptr<AST::Expression>> &&values, const yy::location &loc);
    explicit Tuple(const yy::location &loc);

    ~Tuple() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::vector<std::unique_ptr<AST::Expression>> values_;
};

} // namespace AST
