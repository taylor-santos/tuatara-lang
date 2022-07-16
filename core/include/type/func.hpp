//
// Created by taylor-santos on 7/15/2022 at 18:47.
//

#pragma once

#include "type/type.hpp"
#include "type/tuple.hpp"

#include <vector>
#include <memory>

namespace TypeChecker {

class Func final : public Type {
public:
    Func(std::unique_ptr<Tuple> arg_type, std::unique_ptr<Type> ret_type, yy::location loc);

    ~Func() override;

    void
    print(std::ostream &os) const override;

    [[nodiscard]] Relation
    compare(const Type &other) const override;

private:
    std::unique_ptr<Tuple> arg_type_;
    std::unique_ptr<Type>  ret_type_;
};

} // namespace TypeChecker
