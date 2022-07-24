//
// Created by taylor-santos on 7/17/2022 at 15:22.
//

#pragma once

#include "type/type.hpp"

#include <memory>

namespace TypeChecker {

class Wrapper final : public Type {
public:
    Wrapper(const Type &type, yy::location loc);
    explicit Wrapper(const Type &type);
    ~Wrapper() override;

    void
    print(std::ostream &os, bool paren) const override;

protected:
    [[nodiscard]] Relation
    get_relation(const Type &other) const override;

private:
    const Type &type_;
};

} // namespace TypeChecker
