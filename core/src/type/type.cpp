//
// Created by taylor-santos on 2/7/2022 at 19:22.
//

#include "type/type.hpp"
#include "type/tuple.hpp"

#include <sstream>

namespace TypeChecker {

bool
is_a_relationship(Relation rel) {
    switch (rel) {
        case Relation::SAME_TYPE:
        case Relation::SUB_TYPE: return true;
        case Relation::SUPER_TYPE:
        case Relation::UNRELATED: break;
    }
    return false;
}

Relation
invert(Relation rel) {
    // Initialize to silence warning, despite switch covering all cases
    Relation ret = Relation::UNRELATED;
    switch (rel) {
        case Relation::SAME_TYPE:
        case Relation::UNRELATED: ret = rel; break;
        case Relation::SUB_TYPE: ret = Relation::SUPER_TYPE; break;
        case Relation::SUPER_TYPE: ret = Relation::SUB_TYPE; break;
    }
    return ret;
}

Type::Type(std::optional<yy::location> loc)
    : loc_{loc} {}

Type::~Type() = default;

std::optional<yy::location>
Type::get_loc() const {
    return loc_;
}

std::string
Type::print() const {
    std::stringstream ss;
    print(ss);
    return ss.str();
}

Relation
Type::compare(const Type &other) const {
    // Method is virtual and overridden by the Tuple type.
    // So we can assume that the type of `this` is not a Tuple, and treat it as a 1-tuple.
    auto *tup = dynamic_cast<const Tuple *>(&other);
    if (tup) {
        // `this` is not a Tuple, but `other` is.
        if (tup->types().empty()) {
            // `other` is a Unit, and all types are subtypes of Unit
            return Relation::SUB_TYPE;
        }
        // `other` is an N-tuple, where N > 1.
        // 1-tuples can only either be unrelated to or a supertype of N-tuples, depending on the
        // relation of its first element.
        auto cmp = compare(*tup->types().front());
        switch (cmp) {
            case Relation::SUPER_TYPE:
            case Relation::SAME_TYPE: return Relation::SUPER_TYPE;
            case Relation::SUB_TYPE:
            case Relation::UNRELATED: return Relation::UNRELATED;
        }
    }
    return get_relation(other);
}

std::ostream &
operator<<(std::ostream &os, const Type &type) {
    type.print(os);
    return os;
}

} // namespace TypeChecker
