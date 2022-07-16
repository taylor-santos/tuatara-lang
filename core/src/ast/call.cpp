//
// Created by taylor-santos on 7/6/2022 at 12:29.
//

#include "ast/call.hpp"
#include "type/error.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"

#include <ostream>

namespace AST {

Call::Call(
    std::unique_ptr<AST::Expression> func,
    std::unique_ptr<AST::Expression> arg,
    const yy::location              &loc)
    : Node(loc)
    , func_{std::move(func)}
    , arg_{std::move(arg)} {}

Call::~Call() = default;

void
Call::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"call",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("function":)";
    func_->to_json(os);
    os << ","
       << R"("argument":)";
    arg_->to_json(os);
    os << "}";
}

const TypeChecker::Type &
Call::get_type(TypeChecker::Context &ctx) const {
    return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc())); // TODO
}

} // namespace AST
