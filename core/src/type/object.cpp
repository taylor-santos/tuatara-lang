//
// Created by taylor-santos on 2/8/2022 at 19:14.
//

#include "type/object.hpp"
#include "type/class.hpp"

namespace TypeChecker {

Object::Object(const Class &cl, yy::location loc)
    : Type(loc)
    , class_{cl} {}

Object::~Object() = default;

void
Object::print(std::ostream &os) const {
    os << "[object " << class_ << "]";
}

const Class &
Object::get_class() const {
    return class_;
}

} // namespace TypeChecker
