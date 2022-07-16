//
// Created by taylor-santos on 7/15/2022 at 18:17.
//

#pragma once

#include "type/type.hpp"

#include <vector>
#include <memory>

namespace TypeChecker {

class Tuple final : public Type {
public:
    Tuple(std::vector<std::unique_ptr<Type>> &&types, yy::location loc);
    explicit Tuple(yy::location loc);

    ~Tuple() override;

    void
    print(std::ostream &os) const override;

    [[nodiscard]] Relation
    compare(const Type &other) const override;

private:
    std::vector<std::unique_ptr<Type>> types_;
};

} // namespace TypeChecker
