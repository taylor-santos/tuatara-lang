//
// Created by taylor-santos on 7/15/2022 at 18:17.
//

#include "type/tuple.hpp"
#include "type/class.hpp"

#include <utility>

namespace TypeChecker {

Tuple::Tuple(std::vector<const Type *> types, yy::location loc)
    : Type(loc)
    , types_{std::move(types)} {}

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
Tuple::get_relation(const Type &other) const {
    const auto *tup_ptr = dynamic_cast<const Tuple *>(&other);
    if (!tup_ptr) return Relation::UNRELATED;

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
