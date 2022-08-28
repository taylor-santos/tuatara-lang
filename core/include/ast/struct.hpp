//
// Created by taylor-santos on 8/23/2022 at 18:36.
//

#pragma once

#include "ast/literal.hpp"
#include "ast/definition.hpp"

#include <vector>

namespace AST {

class Struct final : public Literal {
public:
    Struct(std::vector<std::unique_ptr<AST::Definition>> fields, const yy::location &loc);
    ~Struct() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::vector<std::unique_ptr<AST::Definition>> fields_;
};

} // namespace AST
