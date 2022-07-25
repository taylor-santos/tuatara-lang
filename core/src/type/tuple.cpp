//
// Created by taylor-santos on 7/15/2022 at 18:17.
//

#include "type/tuple.hpp"
#include "type/class.hpp"

#include <utility>

namespace TypeChecker {

Tuple::Tuple(std::vector<const Type *> types, yy::location loc)
    : Type(loc)
    , types_{std::move(types)} {
    if (types_.size() == 1) {
        throw std::logic_error("Attempting to create a 1-tuple type");
    }
}

Tuple::Tuple(yy::location loc)
    : Type(loc)
    , types_{} {}

Tuple::~Tuple() = default;

void
Tuple::print(std::ostream &os, bool) const {
    os << "(";
    std::string sep;
    for (auto &type : types_) {
        os << sep;
        sep = ", ";
        type->print(os);
    }
    if (types_.size() == 1) os << ",";
    os << ")";
}

Relation
Tuple::compare(const Type &other) const {
    // The base Type defines this method to check if `other` is a Tuple type and do Tuple comparison
    // as if `this` is a 1-tuple. Since this method is overridden by Tuple, only non-Tuple types
    // are affected by this 1-tuple comparison.
    return get_relation(other);
}

Relation
Tuple::get_relation(const Type &other) const {
    // TODO: Unify 1-tuple implementation with N-tuple, by creating a 1-vector and treating it as
    //       the list of types.
    const auto *tup_ptr = dynamic_cast<const Tuple *>(&other);
    if (!tup_ptr) {
        // `other` is of type T, where T != Tuple.
        // Treat it instead as a 1-tuple of T.
        if (types_.empty()) {
            // `this` is a 0-tuple/unit. Unit is a supertype of all types.
            return Relation::SUPER_TYPE;
        }
        // `this` is an N-tuple, where N > 1.
        // N-tuples can only either be unrelated to or a subtype of 1-tuples, depending on the
        // relation of its first element.
        auto cmp = types_.front()->compare(other);
        switch (cmp) {
            case Relation::SUB_TYPE:
            case Relation::SAME_TYPE: return Relation::SUB_TYPE;
            case Relation::SUPER_TYPE:
            case Relation::UNRELATED: return Relation::UNRELATED;
        }
    }

    const auto &tup = *tup_ptr;

    auto n1 = types_.size();
    auto n2 = tup.types_.size();

    if (n1 < n2) {
        // Maybe tup1 is a supertype of tup2?
        for (size_t i = 0; i < n1; i++) {
            auto &t1  = *types_[i];
            auto &t2  = *tup.types_[i];
            auto  rel = t1.compare(t2);
            switch (rel) {
                case Relation::SUPER_TYPE:
                case Relation::SAME_TYPE:
                    // Could still be a super type
                    break;
                case Relation::SUB_TYPE:
                case Relation::UNRELATED: return Relation::UNRELATED;
            }
        }
        return Relation::SUPER_TYPE;
    } else if (n1 > n2) {
        // Maybe tup1 is a subtype of tup2?
        for (size_t i = 0; i < n2; i++) {
            auto &t1  = *types_[i];
            auto &t2  = *tup.types_[i];
            auto  rel = t1.compare(t2);
            switch (rel) {
                case Relation::SUB_TYPE:
                case Relation::SAME_TYPE:
                    // Could still be a sub type
                    break;
                case Relation::SUPER_TYPE:
                case Relation::UNRELATED: return Relation::UNRELATED;
            }
        }
        return Relation::SUB_TYPE;
    }
    // n1 == n2
    /* Tuple A is a subtype of tuple B iff A has at least as many elements as B, AND every element
     * of A is either the same type or a subtype of the same element in B.
     * If we start by assuming that they are the same type, we can refine our assumption by finding
     * any element in A that isn't the same type as the element in B.
     * - If these types are unrelated, A and B must be unrelated.
     * - If A's element is a subtype of B's element, A is a subtype of B iff the same can be said
     *   for the remaining elements. Otherwise they are unrelated.
     * - If A's element is a supertype of B's element, A is a supertype of B iff the same can be
     *   said for the remaining elements. Otherwise they are unrelated.
     */
    auto temp_rel = Relation::SAME_TYPE;
    for (size_t i = 0; i < n1; i++) {
        auto &t1  = *types_[i];
        auto &t2  = *tup.types_[i];
        auto  rel = t1.compare(t2);
        switch (rel) {
            case Relation::UNRELATED: return Relation::UNRELATED;
            case Relation::SAME_TYPE: break;
            case Relation::SUB_TYPE:
                if (temp_rel == Relation::SUPER_TYPE) {
                    return Relation::UNRELATED;
                }
                temp_rel = Relation::SUB_TYPE;
                break;
            case Relation::SUPER_TYPE:
                if (temp_rel == Relation::SUB_TYPE) {
                    return Relation::UNRELATED;
                }
                temp_rel = Relation::SUPER_TYPE;
                break;
        }
    }
    return temp_rel;
}

const std::vector<const Type *> &
Tuple::types() const {
    return types_;
}

} // namespace TypeChecker
