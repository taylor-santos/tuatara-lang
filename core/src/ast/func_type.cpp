//
// Created by taylor-santos on 7/15/2022 at 18:46.
//

#include "ast/func_type.hpp"
#include "type/type_checker.hpp"
#include "type/class.hpp"
#include "json.hpp"
#include "type/error.hpp"
#include "printer.hpp"

#include <utility>
#include <ostream>

namespace AST {

FuncType::FuncType(
    std::unique_ptr<Type> arg_type,
    std::unique_ptr<Type> ret_type,
    const yy::location   &loc)
    : Node(loc)
    , arg_type_{std::move(arg_type)}
    , ret_type_{std::move(ret_type)} {}

FuncType::~FuncType() = default;

void
FuncType::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"function type",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("arg type":)";
    arg_type_->to_json(os);
    os << ","
       << R"("ret type":)";
    ret_type_->to_json(os);
    os << "}";
}

const TypeChecker::Type &
FuncType::get_type(TypeChecker::Context &ctx) const {
    return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc())); // TODO
}

} // namespace AST
