//
// Created by taylor-santos on 2/8/2022 at 19:14.
//

#pragma once

#include "type/type.hpp"

namespace TypeChecker {

class Class;

class Object final : public Type {
public:
    Object(const Class &cl, yy::location loc);
    ~Object() override;

    [[nodiscard]] const Class &
    get_class() const;

    void
    print(std::ostream &os, bool paren) const override;

protected:
    [[nodiscard]] Relation
    get_relation(const Type &other) const override;

private:
    const Class &class_;
};

} // namespace TypeChecker
