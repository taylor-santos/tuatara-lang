//
// Created by taylor-santos on 2/7/2022 at 19:06.
//

#pragma once

#include <ostream>

namespace TypeChecker {

class Type {
public:
    Type();
    Type(const Type &) = delete;
    virtual ~Type();

    virtual void
    print(std::ostream &os) const = 0;
};

std::ostream &
operator<<(std::ostream &os, const Type &type);

} // namespace TypeChecker
