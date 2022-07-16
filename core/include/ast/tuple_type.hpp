//
// Created by taylor-santos on 7/15/2022 at 18:18.
//

#pragma once

#include "ast/type.hpp"

#include <vector>

namespace AST {

class TupleType final : public Type {
public:
    TupleType(std::vector<std::unique_ptr<Type>> &&types, const yy::location &loc);

    explicit TupleType(const yy::location &loc);

    ~TupleType() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::vector<std::unique_ptr<Type>> types_;
};

} // namespace AST
