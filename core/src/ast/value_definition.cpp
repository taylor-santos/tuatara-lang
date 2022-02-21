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
    if (prev) {
        using namespace print;

        ctx.set_failure(true);
        auto message = Message::error(get_loc().begin)
                           .with_message("`" + name + "` already defined")
                           .in(color::bold_gray);
        auto prev_loc = ctx.get_symbol_loc(name);
        if (prev_loc) {
            message.with_detail_at(*prev_loc)
                .with_message("previous definition of `" + name + "` here")
                .in(color::bold_yellow);
        }
        message.with_detail_at(get_loc())
            .with_message("redefined `" + name + "` here")
            .in(color::bold_red);
        ctx.add_message(message);
        return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    }
    auto &type = value_->get_type(ctx);
    ctx.set_symbol(name, type, get_loc());
    return type;
}

} // namespace AST
