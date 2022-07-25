//
// Created by taylor-santos on 7/15/2022 at 18:48.
//

#include "type/func.hpp"

#include <utility>
#include "type/class.hpp"

namespace TypeChecker {

Func::Func(std::vector<const Type *> arg_types, const Type &ret_type, yy::location loc)
    : Type(loc)
    , arg_types_{std::move(arg_types)}
    , ret_type_{std::move(ret_type)} {}

Func::~Func() = default;

void
Func::print(std::ostream &os, bool paren) const {
    if (paren) os << "(";
    if (arg_types_.size() != 1) os << "(";
    std::string sep;
    for (auto &type : arg_types_) {
        os << sep;
        sep = ", ";
        type->print(os, arg_types_.size() == 1);
    }
    if (arg_types_.size() != 1) os << ")";
    os << "->";
    ret_type_.print(os);
    if (paren) os << ")";
}

Relation
Func::get_relation(const Type &other) const {
    const auto *func_ptr = dynamic_cast<const Func *>(&other);
    if (!func_ptr) return Relation::UNRELATED;
    auto &func = *func_ptr;

    auto *self_args  = &arg_types_;
    auto *other_args = &func.arg_types_;

    if (self_args->size() == 1 && other_args->size() != 1) {
        auto *tup = dynamic_cast<const TypeChecker::Tuple *>(self_args->front());
        if (tup) {
            self_args = &tup->types();
        }
    }
    if (self_args->size() != 1 && other_args->size() == 1) {
        auto *tup = dynamic_cast<const TypeChecker::Tuple *>(other_args->front());
        if (tup) {
            other_args = &tup->types();
        }
    }

    if (other_args->size() != self_args->size()) {
        return Relation::UNRELATED;
    }

    auto temp_rel = Relation::SAME_TYPE;
    for (size_t i = 0; i < self_args->size(); i++) {
        auto &a   = (*self_args)[i];
        auto &b   = (*other_args)[i];
        auto  rel = a->compare(*b);
        switch (rel) {
            case Relation::UNRELATED: return Relation::UNRELATED;
            case Relation::SAME_TYPE:
                break;
                // Functions are contravariant in their argument types:
            case Relation::SUB_TYPE:
                if (temp_rel == Relation::SUB_TYPE) return Relation::UNRELATED;
                temp_rel = Relation::SUPER_TYPE;
                break;
            case Relation::SUPER_TYPE:
                if (temp_rel == Relation::SUPER_TYPE) return Relation::UNRELATED;
                temp_rel = Relation::SUB_TYPE;
                break;
        }
    }

    auto ret_rel = ret_type_.compare(func.ret_type_);
    switch (ret_rel) {
        case Relation::UNRELATED: return Relation::UNRELATED;
        case Relation::SAME_TYPE:
            break;
            // Functions are covariant in their return types:
        case Relation::SUB_TYPE:
            if (temp_rel == Relation::SUPER_TYPE) return Relation::UNRELATED;
            temp_rel = Relation::SUB_TYPE;
            break;
        case Relation::SUPER_TYPE:
            if (temp_rel == Relation::SUB_TYPE) return Relation::UNRELATED;
            temp_rel = Relation::SUPER_TYPE;
            break;
    }

    return temp_rel;
}

const std::vector<const Type *> &
Func::arg_types() const {
    return arg_types_;
}

const Type &
Func::ret_type() const {
    return ret_type_;
}

} // namespace TypeChecker
