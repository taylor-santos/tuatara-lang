//
// Created by taylor-santos on 1/2/2022 at 20:18.
//

#include "ast/value_definition.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/error.hpp"

#include <utility>
#include <ostream>
#include <sstream>

namespace AST {

ValueDefinition::ValueDefinition(
    std::string                 &&name,
    const yy::location           &name_loc,
    std::unique_ptr<Expression> &&value,
    const yy::location           &loc)
    : Node(loc)
    , Definition(std::move(name), name_loc)
    , value_{std::move(value)} {}

ValueDefinition::~ValueDefinition() = default;

void
ValueDefinition::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"value definition",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("name":")" << escape_string(get_name()) << R"(",)"
       << R"("name location":)";
    loc_to_json(get_name_loc(), os);
    os << ","
       << R"("value":)";
    value_->to_json(os);
    os << "}";
}

const TypeChecker::Type &
ValueDefinition::get_type(TypeChecker::Context &ctx) const {
    auto name = get_name();
    auto prev = ctx.get_symbol(name);

    auto &type = [&]() -> const TypeChecker::Type & {
        try {
            return value_->get_type(ctx);
        } catch (std::exception &e) {
            // TODO: Proper error handling
            ctx.set_failure(true);
            ctx.report_error(get_loc(), e.what());
            return ctx.add_type(std::make_unique<TypeChecker::Error>());
        }
    }();

    if (prev) { // TODO: Check if subtype
        // TODO: Proper error handling
        ctx.set_failure(true);
        std::stringstream ss;
        ss << name << " already defined as ";
        prev->get().print(ss);
        throw std::runtime_error(ss.str());
    }
    ctx.set_symbol(name, type);
    return type;
}

} // namespace AST
