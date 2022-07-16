//
// Created by taylor-santos on 7/15/2022 at 16:07.
//

#include "ast/tuple.hpp"
#include "type/error.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"

#include <ostream>

namespace AST {

Tuple::Tuple(std::vector<std::unique_ptr<AST::SimpleExpression>> &&values, const yy::location &loc)
    : Node(loc)
    , values_{std::move(values)} {}

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
    return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc())); // TODO
}

} // namespace AST
