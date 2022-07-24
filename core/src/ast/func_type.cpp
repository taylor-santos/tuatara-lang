//
// Created by taylor-santos on 7/15/2022 at 18:46.
//

#include "ast/func_type.hpp"
#include "type/type_checker.hpp"
#include "type/class.hpp"
#include "json.hpp"
#include "printer.hpp"
#include "type/func.hpp"

#include <utility>
#include <ostream>

namespace AST {

FuncType::FuncType(
    std::vector<std::unique_ptr<Type>> arg_types,
    std::unique_ptr<Type>              ret_type,
    const yy::location                &loc)
    : Node(loc)
    , arg_types_{std::move(arg_types)}
    , ret_type_{std::move(ret_type)} {}

FuncType::~FuncType() = default;

void
FuncType::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"function type",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("arg types":[)";
    std::string sep;
    for (auto &type : arg_types_) {
        os << sep;
        sep = ",";
        type->to_json(os);
    }
    os << "],"
       << R"("ret type":)";
    ret_type_->to_json(os);
    os << "}";
}

const TypeChecker::Type &
FuncType::get_type(TypeChecker::Context &ctx) const {
    auto &ret = ret_type_->get_type(ctx);

    std::vector<const TypeChecker::Type *> arg_types;

    for (auto &arg : arg_types_) {
        auto &type = arg->get_type(ctx);
        arg_types.push_back(&type);
    }

    return ctx.add_type(std::make_unique<TypeChecker::Func>(arg_types, ret, get_loc()));
}

} // namespace AST
