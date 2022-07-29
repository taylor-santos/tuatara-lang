//
// Created by taylor-santos on 7/26/2022 at 12:53.
//

#pragma once

#include "ast/definition.hpp"
#include "ast/type.hpp"

#include <optional>

namespace AST {

class Operator final : public Definition {
public:
    using arg_t = std::tuple<std::string, yy::location, std::unique_ptr<AST::Type>>;
    Operator(
        uint64_t                                  precedence,
        arg_t                                   &&lhs,
        std::string                             &&op,
        const yy::location                       &op_loc,
        arg_t                                   &&rhs,
        std::optional<std::unique_ptr<AST::Type>> ret_type,
        std::unique_ptr<Expression>               body,
        const yy::location                       &loc);

    Operator(
        uint64_t                                  precedence,
        std::string                             &&op,
        const yy::location                       &op_loc,
        arg_t                                   &&rhs,
        std::optional<std::unique_ptr<AST::Type>> ret_type,
        std::unique_ptr<Expression>               body,
        const yy::location                       &loc);

    Operator(
        uint64_t                                  precedence,
        arg_t                                   &&lhs,
        std::string                             &&op,
        const yy::location                       &op_loc,
        std::optional<std::unique_ptr<AST::Type>> ret_type,
        std::unique_ptr<Expression>               body,
        const yy::location                       &loc);

    ~Operator() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    uint64_t                                  precedence_;
    std::optional<arg_t>                      lhs_;
    std::optional<arg_t>                      rhs_;
    std::optional<std::unique_ptr<AST::Type>> ret_type_;
    std::unique_ptr<Expression>               body_;
};

} // namespace AST