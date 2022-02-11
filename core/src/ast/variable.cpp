//
// Created by taylor-santos on 1/2/2022 at 21:28.
//

#include "ast/variable.hpp"
#include "json.hpp"
#include "type/context.hpp"

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
    auto type = ctx.get_symbol(name_);
    if (!type) {
        // TODO: Proper error handling
        ctx.set_failure(true);
        throw std::runtime_error(name_ + " was not declared in this scope");
    } else {
        auto init = ctx.is_initialized(name_);
        if (!init) {
            // TODO: Proper error handling
            ctx.set_failure(true);
            std::cerr << name_ << " used before initialization" << std::endl;
        }
        return type->get();
    }
}

} // namespace AST
