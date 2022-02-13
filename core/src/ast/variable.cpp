//
// Created by taylor-santos on 1/2/2022 at 21:28.
//

#include "ast/variable.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"

#include <utility>
#include <ostream>
#include <sstream>

namespace AST {

Variable::Variable(std::string name, const yy::location &loc)
    : Node(loc)
    , name_{std::move(name)} {}

Variable::~Variable() = default;

void
Variable::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"variable",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("name":")" << escape_string(name_) << R"(")"
       << "}";
}

const TypeChecker::Type &
Variable::get_type(TypeChecker::Context &ctx) const {
    using namespace TypeChecker;
    auto type = ctx.get_symbol(name_);
    if (!type) {
        // TODO: Proper error handling
        ctx.set_failure(true);
        throw std::runtime_error("`" + name_ + "` was not declared in this scope");
    } else {
        auto &t      = type->get();
        auto  uninit = ctx.is_uninitialized(name_);
        if (uninit) {
            // TODO: Proper error handling
            ctx.set_failure(true);
            std::stringstream error;
            switch (uninit->reason) {
                case Uninit::Reason::NOT_DEFINED:
                    error << "`" << name_ << "` used before initialization";
                    break;
                case Uninit::Reason::MOVED_FROM:
                    error << "`" << name_ << "` used after being moved";
                    break;
            }
            ctx.report_error(get_loc(), error.str());
            // We can fall through and return `t` here instead of an `Error` type, because the
            // expected type of the variable can still be known even though it isn't allowed to be
            // used. This is still a type error, but later type errors can still be inferred from
            // this type.
        } else {
            ctx.set_symbol(name_, t, Uninit{Uninit::Reason::MOVED_FROM, *this});
        }
        return t;
    }
}

} // namespace AST
