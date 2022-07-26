//
// Created by taylor-santos on 7/25/2022 at 14:11.
//

#pragma once

#include "type/type.hpp"

#include <vector>
#include <memory>

namespace TypeChecker {

class Array final : public Type {
public:
    Array(const Type &type, yy::location loc);

    ~Array() override;

    void
    print(std::ostream &os, bool paren) const override;

    [[nodiscard]] const Type &
    element_type() const;

protected:
    [[nodiscard]] Relation
    get_relation(const Type &other) const override;

private:
    const Type &type_;
};

} // namespace TypeChecker
