//
// Created by taylor-santos on 8/23/2022 at 18:37.
//

#include "ast/struct.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/error.hpp"

#include <ostream>

namespace AST {

Struct::Struct(std::vector<std::unique_ptr<AST::Definition>> fields, const yy::location &loc)
    : Node(loc)
    , fields_{std::move(fields)} {}

Struct::~Struct() = default;

void
Struct::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"Struct",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    // TODO unimplemented
    os << "}";
}

const TypeChecker::Type &
Struct::get_type(TypeChecker::Context &ctx) const {
    // TODO unimplemented
    return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
}

} // namespace AST
