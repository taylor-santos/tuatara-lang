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
    Relation ret;
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
    /*
    auto *type = &other;
    while (true) {
        // A 1-tuple (T,) is equivalent to T. Use T's compare method instead of Tuple's method.
        auto *tup = dynamic_cast<const Tuple *>(type);
        if (!tup) break;
        type = tup;
    }
    return get_relation(*type);
     */
    return get_relation(other);
}

std::ostream &
operator<<(std::ostream &os, const Type &type) {
    type.print(os);
    return os;
}

} // namespace TypeChecker
