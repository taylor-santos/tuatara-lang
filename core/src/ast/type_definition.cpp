//
// Created by taylor-santos on 1/2/2022 at 20:23.
//

#include "ast/type_definition.hpp"
#include "ast/type.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/error.hpp"
#include "printer.hpp"

#include <utility>
#include <ostream>

namespace AST {

TypeDefinition::TypeDefinition(
    std::string              &&name,
    const yy::location        &name_loc,
    std::unique_ptr<AST::Type> type,
    const yy::location        &loc)
    : Node(loc)
    , Definition(std::move(name), name_loc)
    , type_{std::move(type)} {}

TypeDefinition::~TypeDefinition() = default;

void
TypeDefinition::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"type definition",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("name":")" << escape_string(get_name()) << R"(",)"
       << R"("name location":)";
    loc_to_json(get_name_loc(), os);
    os << ","
       << R"("type":)";
    type_->to_json(os);
    os << "}";
}

const TypeChecker::Type &
TypeDefinition::get_type(TypeChecker::Context &ctx) const {
    auto  name = get_name();
    auto &type = type_->get_type(ctx);
    auto  prev = ctx.get_symbol(name);
    if (prev) {
        using namespace print;
        ctx.set_failure(true);
        auto message = Message::error(get_loc().begin)
                           .with_message("variable `", color::bold_gray)
                           .with_message(name, color::bold_red)
                           .with_message("` already defined", color::bold_gray);
        auto prev_loc = ctx.get_symbol_loc(name);
        if (prev_loc) {
            message.with_detail(*prev_loc, color::bold_yellow)
                .with_message("previous definition of `", color::bold_gray)
                .with_message(name, color::bold_yellow)
                .with_message("` here", color::bold_gray);
        }
        message.with_detail(get_name_loc(), color::bold_red)
            .with_message("redefined `", color::bold_gray)
            .with_message(name, color::bold_red)
            .with_message("` here", color::bold_gray);
        ctx.add_message(message);
        return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    }
    using namespace TypeChecker;
    ctx.set_symbol(name, type, get_name_loc(), Uninit{Uninit::Reason::NOT_DEFINED, get_name_loc()});
    return type;
}

} // namespace AST
