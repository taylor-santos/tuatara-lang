//
// Created by taylor-santos on 1/2/2022 at 20:37.
//

#include "ast/u64.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/object.hpp"

#include <ostream>

namespace AST {

U64::U64(uint64_t value, const yy::location &loc)
    : Node(loc)
    , value_{value} {}

U64::~U64() = default;

void
U64::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"u64",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("value":")" << value_ << R"(")"
       << "}";
}

const TypeChecker::Type &
U64::get_type(TypeChecker::Context &ctx) const {
    return ctx.add_type(std::make_unique<TypeChecker::Object>(TypeChecker::Context::builtins.U64));
}

} // namespace AST
