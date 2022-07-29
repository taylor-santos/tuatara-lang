//
// Created by taylor-santos on 7/15/2022 at 16:45.
//

#pragma once

#include "ast/expression.hpp"
#include "ast/type.hpp"
#include "type/type_checker.hpp"

#include <vector>
#include <optional>

namespace AST {

class Function final : public Expression {
public:
    using arg_t = std::tuple<std::string, yy::location, std::unique_ptr<AST::Type>>;
    Function(
        std::vector<arg_t>             &&args,
        const yy::location              &sig_loc,
        std::unique_ptr<AST::Expression> body,
        const yy::location              &loc);

    Function(
        std::vector<arg_t>             &&args,
        std::unique_ptr<AST::Type>       ret_type,
        const yy::location              &args_loc,
        std::unique_ptr<AST::Expression> body,
        const yy::location              &loc);

    ~Function() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::vector<arg_t>                          args_;
    yy::location                                sig_loc_;
    std::optional<std::unique_ptr<AST::Type>>   ret_type_;
    std::unique_ptr<AST::Expression>            body_;
    mutable std::optional<TypeChecker::Context> ctx_;
};

} // namespace AST
