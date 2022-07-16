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
    os << class_;
}

const Class &
Object::get_class() const {
    return class_;
}

Relation
Object::compare(const Type &other) const {
    const auto *obj_ptr = dynamic_cast<const Object *>(&other);
    if (!obj_ptr) return Relation::UNRELATED;

    return class_.compare(obj_ptr->class_);
}

} // namespace TypeChecker
