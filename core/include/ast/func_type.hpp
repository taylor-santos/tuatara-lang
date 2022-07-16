//
// Created by taylor-santos on 7/15/2022 at 18:45.
//

#pragma once

#include "ast/type.hpp"

#include <vector>

namespace AST {

class FuncType final : public Type {
public:
    FuncType(
        std::unique_ptr<Type> arg_type,
        std::unique_ptr<Type> ret_type,
        const yy::location   &loc);

    ~FuncType() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::unique_ptr<Type> arg_type_;
    std::unique_ptr<Type> ret_type_;
};

} // namespace AST
