//
// Created by taylor-santos on 6/29/2022 at 11:50.
//

#pragma once

#include "ast/definition.hpp"

#include <memory>

namespace AST {
class Type;
} // namespace AST

namespace AST {

class TypeValueDefinition final : public Definition {
public:
    TypeValueDefinition(
        std::string               &&name,
        const yy::location         &name_loc,
        std::unique_ptr<AST::Type>  type,
        std::unique_ptr<Expression> value,
        const yy::location         &loc);
    ~TypeValueDefinition() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::unique_ptr<AST::Type>       type_;
    std::unique_ptr<AST::Expression> value_;
};

} // namespace AST
