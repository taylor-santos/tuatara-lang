//
// Created by taylor-santos on 2/7/2022 at 19:06.
//

#pragma once

#include <ostream>
#include <optional>

#include "location.hh"

namespace TypeChecker {

class Type {
public:
    explicit Type(std::optional<yy::location> loc);
    Type(const Type &) = delete;
    virtual ~Type();

    [[nodiscard]] std::optional<yy::location>
    get_loc() const;

    virtual void
    print(std::ostream &os) const = 0;

private:
    std::optional<yy::location> loc_;
};

std::ostream &
operator<<(std::ostream &os, const Type &type);

} // namespace TypeChecker
