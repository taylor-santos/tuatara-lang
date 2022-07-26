//
// Created by taylor-santos on 7/25/2022 at 14:11.
//

#pragma once

#include "ast/type.hpp"

#include <vector>

namespace AST {

class ArrayType final : public Type {
public:
    ArrayType(std::unique_ptr<Type> type, const yy::location &loc);

    ~ArrayType() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::unique_ptr<Type> type_;
};

} // namespace AST
