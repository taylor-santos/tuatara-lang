//
// Created by taylor-santos on 7/26/2022 at 11:49.
//

#pragma once

#include "ast/expression.hpp"

#include <optional>

namespace AST {

class OperatorCall final : public Expression {
public:
    OperatorCall(
        std::unique_ptr<Expression> lhs,
        std::string               &&op,
        const yy::location         &op_loc,
        const yy::location         &loc);

    OperatorCall(
        std::string               &&op,
        const yy::location         &op_loc,
        std::unique_ptr<Expression> rhs,
        const yy::location         &loc);

    OperatorCall(
        std::unique_ptr<Expression> lhs,
        std::string               &&op,
        const yy::location         &op_loc,
        std::unique_ptr<Expression> rhs,
        const yy::location         &loc);

    ~OperatorCall() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::optional<std::unique_ptr<Expression>> lhs_;
    std::string                                operator_;
    yy::location                               op_loc_;
    std::optional<std::unique_ptr<Expression>> rhs_;
};

} // namespace AST