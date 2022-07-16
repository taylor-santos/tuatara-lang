//
// Created by taylor-santos on 7/15/2022 at 16:45.
//

#pragma once

#include "ast/simple_expression.hpp"
#include "ast/type.hpp"

#include <vector>
#include <optional>

namespace AST {

class Function final : public SimpleExpression {
public:
    using arg_t = std::tuple<std::string, std::unique_ptr<AST::Type>, yy::location>;
    Function(
        std::vector<arg_t>                   &&args,
        std::unique_ptr<AST::SimpleExpression> body,
        const yy::location                    &loc);

    Function(
        std::vector<arg_t>                   &&args,
        std::unique_ptr<AST::Type>             ret_type,
        std::unique_ptr<AST::SimpleExpression> body,
        const yy::location                    &loc);

    ~Function() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::vector<arg_t>                        args_;
    std::optional<std::unique_ptr<AST::Type>> ret_type_;
    std::unique_ptr<AST::SimpleExpression>    body_;
};

} // namespace AST
