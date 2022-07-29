//
// Created by taylor-santos on 7/26/2022 at 13:00.
//

#include "ast/op_def.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/error.hpp"
#include "printer.hpp"

#include <ostream>
#include <unordered_map>

namespace AST {

Operator::Operator(
    uint64_t                                  precedence,
    arg_t                                   &&lhs,
    std::string                             &&op,
    const yy::location                       &op_loc,
    arg_t                                   &&rhs,
    std::optional<std::unique_ptr<AST::Type>> ret_type,
    std::unique_ptr<Expression>               body,
    const yy::location                       &loc)
    : Node(loc)
    , Definition(std::move(op), op_loc)
    , precedence_{precedence}
    , lhs_{std::move(lhs)}
    , rhs_{std::move(rhs)}
    , ret_type_{std::move(ret_type)}
    , body_{std::move(body)} {}

Operator::Operator(
    uint64_t                                  precedence,
    std::string                             &&op,
    const yy::location                       &op_loc,
    arg_t                                   &&rhs,
    std::optional<std::unique_ptr<AST::Type>> ret_type,
    std::unique_ptr<Expression>               body,
    const yy::location                       &loc)
    : Node(loc)
    , Definition(std::move(op), op_loc)
    , precedence_{precedence}
    , lhs_{}
    , rhs_{std::move(rhs)}
    , ret_type_{std::move(ret_type)}
    , body_{std::move(body)} {}

Operator::Operator(
    uint64_t                                  precedence,
    arg_t                                   &&lhs,
    std::string                             &&op,
    const yy::location                       &op_loc,
    std::optional<std::unique_ptr<AST::Type>> ret_type,
    std::unique_ptr<Expression>               body,
    const yy::location                       &loc)
    : Node(loc)
    , Definition(std::move(op), op_loc)
    , precedence_{precedence}
    , lhs_{std::move(lhs)}
    , rhs_{}
    , ret_type_{std::move(ret_type)}
    , body_{std::move(body)} {}

Operator::~Operator() = default;

void
Operator::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"operator",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    // TODO nocheckin
    os << ","
       << R"("body":)";
    body_->to_json(os);
    os << "}";
}

const TypeChecker::Type &
Operator::get_type(TypeChecker::Context &ctx) const {
    // TODO nocheckin
    return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
}

} // namespace AST
