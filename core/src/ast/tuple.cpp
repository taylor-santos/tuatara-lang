//
// Created by taylor-santos on 7/15/2022 at 16:07.
//

#include "ast/tuple.hpp"
#include "type/error.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/tuple.hpp"

#include <ostream>
#include <algorithm>

namespace AST {

Tuple::Tuple(std::vector<std::unique_ptr<AST::SimpleExpression>> &&values, const yy::location &loc)
    : Node(loc)
    , values_{std::move(values)} {
    if (values_.size() == 1) {
        throw std::logic_error("Attempting to create a 1-tuple");
    }
}

Tuple::Tuple(const yy::location &loc)
    : Node(loc)
    , values_{} {}

Tuple::~Tuple() = default;

void
Tuple::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"tuple",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("values":[)";
    std::string sep;
    for (auto &value : values_) {
        os << sep;
        sep = ",";
        value->to_json(os);
    }
    os << "]}";
}

const TypeChecker::Type &
Tuple::get_type(TypeChecker::Context &ctx) const {
    std::vector<const TypeChecker::Type *> types;
    types.reserve(values_.size());
    std::transform(values_.begin(), values_.end(), std::back_inserter(types), [&ctx](auto &value) {
        return &value->get_type(ctx);
    });
    return ctx.add_type(std::make_unique<TypeChecker::Tuple>(types, get_loc()));
}

} // namespace AST
