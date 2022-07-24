//
// Created by taylor-santos on 7/15/2022 at 18:47.
//

#pragma once

#include "type/type.hpp"
#include "type/tuple.hpp"
#include "type/type_checker.hpp"

#include <vector>
#include <memory>

namespace TypeChecker {

class Func final : public Type {
public:
    Func(std::vector<const Type *> arg_types, const Type &ret_type, yy::location loc);

    ~Func() override;

    void
    print(std::ostream &os, bool paren) const override;

    [[nodiscard]] const std::vector<const Type *> &
    arg_types() const;

    [[nodiscard]] const Type &
    ret_type() const;

protected:
    [[nodiscard]] Relation
    get_relation(const Type &other) const override;

private:
    std::vector<const Type *> arg_types_;
    const Type               &ret_type_;
};

} // namespace TypeChecker
