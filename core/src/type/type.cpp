//
// Created by taylor-santos on 2/7/2022 at 19:22.
//

#include "type/type.hpp"

namespace TypeChecker {

Type::Type() = default;

Type::~Type() = default;

std::ostream &
operator<<(std::ostream &os, const Type &type) {
    type.print(os);
    return os;
}

} // namespace TypeChecker
