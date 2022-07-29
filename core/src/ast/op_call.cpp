//
// Created by taylor-santos on 7/26/2022 at 11:52.
//

#include "ast/op_call.hpp"
#include "type/error.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/func.hpp"
#include "printer.hpp"

#include <ostream>
#include <sstream>

namespace AST {

OperatorCall::OperatorCall(
    std::unique_ptr<Expression> lhs,
    std::string               &&op,
    const yy::location         &op_loc,
    const yy::location         &loc)
    : Node(loc)
    , lhs_{std::move(lhs)}
    , operator_{std::move(op)}
    , op_loc_{op_loc} {}

OperatorCall::OperatorCall(
    std::string               &&op,
    const yy::location         &op_loc,
    std::unique_ptr<Expression> rhs,
    const yy::location         &loc)
    : Node(loc)
    , operator_{std::move(op)}
    , op_loc_{op_loc}
    , rhs_{std::move(rhs)} {}

OperatorCall::OperatorCall(
    std::unique_ptr<Expression> lhs,
    std::string               &&op,
    const yy::location         &op_loc,
    std::unique_ptr<Expression> rhs,
    const yy::location         &loc)
    : Node(loc)
    , lhs_{std::move(lhs)}
    , operator_{std::move(op)}
    , op_loc_{op_loc}
    , rhs_{std::move(rhs)} {}

OperatorCall::~OperatorCall() = default;

void
OperatorCall::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"operator call",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    if (lhs_) {
        os << ","
           << R"("lhs":)";
        (*lhs_)->to_json(os);
    }
    os << ","
       << R"("operator":")" << escape_string(operator_) << R"(")";
    if (rhs_) {
        os << ","
           << R"("rhs":)";
        (*rhs_)->to_json(os);
    }
    os << "}";
}

const TypeChecker::Type &
OperatorCall::get_type(TypeChecker::Context &ctx) const {
    // TODO nocheckin
    return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
}

} // namespace AST
