//
// Created by taylor-santos on 2/7/2022 at 19:24.
//

#include "type/class.hpp"

#include <utility>

namespace TypeChecker {

Class::Class(std::string name, std::optional<yy::location> loc)
    : Type(loc)
    , name_{std::move(name)} {}

Class::~Class() = default;

void
Class::print(std::ostream &os) const {
    os << "[class " << name_ << "]";
}

std::string
Class::get_name() const {
    return name_;
}

} // namespace TypeChecker
