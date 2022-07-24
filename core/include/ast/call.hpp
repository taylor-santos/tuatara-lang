//
// Created by taylor-santos on 7/6/2022 at 12:27.
//

#pragma once

#include "ast/simple_expression.hpp"

#include <vector>

namespace AST {

class Call final : public SimpleExpression {
public:
    Call(
        std::unique_ptr<AST::Expression>                    func,
        std::vector<std::unique_ptr<AST::SimpleExpression>> args,
        const yy::location                                 &args_loc,
        const yy::location                                 &loc);

    ~Call() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::unique_ptr<AST::Expression>                    func_;
    std::vector<std::unique_ptr<AST::SimpleExpression>> args_;
    yy::location                                        args_loc_;
};

} // namespace AST
