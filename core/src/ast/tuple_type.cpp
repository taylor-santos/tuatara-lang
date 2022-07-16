//
// Created by taylor-santos on 7/15/2022 at 18:19.
//

#include "ast/tuple_type.hpp"
#include "type/type_checker.hpp"
#include "type/class.hpp"
#include "json.hpp"
#include "type/tuple.hpp"
#include "type/error.hpp"
#include "printer.hpp"

#include <utility>
#include <ostream>

namespace AST {

TupleType::TupleType(std::vector<std::unique_ptr<Type>> &&types, const yy::location &loc)
    : Node(loc)
    , types_{std::move(types)} {}

TupleType::TupleType(const yy::location &loc)
    : Node(loc)
    , types_{} {}

TupleType::~TupleType() = default;

void
TupleType::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"tuple type",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("types":[")";
    std::string sep;
    for (auto &type : types_) {
        os << sep;
        sep = ",";
        type->to_json(os);
    }
    os << "]}";
}

const TypeChecker::Type &
TupleType::get_type(TypeChecker::Context &ctx) const {
    return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc())); // TODO
}

} // namespace AST