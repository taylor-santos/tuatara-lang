//
// Created by taylor-santos on 7/25/2022 at 14:07.
//

#include "ast/array_type.hpp"
#include "type/type_checker.hpp"
#include "type/class.hpp"
#include "json.hpp"
#include "type/array.hpp"
#include "type/error.hpp"
#include "printer.hpp"

#include <utility>
#include <ostream>
#include <algorithm>

namespace AST {

ArrayType::ArrayType(std::unique_ptr<Type> type, const yy::location &loc)
    : Node(loc)
    , type_{std::move(type)} {}

ArrayType::~ArrayType() = default;

void
ArrayType::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"array type",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("type":)";
    type_->to_json(os);
    os << "}";
}

const TypeChecker::Type &
ArrayType::get_type(TypeChecker::Context &ctx) const {
    auto &type = type_->get_type(ctx);
    return ctx.add_type(std::make_unique<TypeChecker::Array>(type, get_loc()));
}

} // namespace AST
