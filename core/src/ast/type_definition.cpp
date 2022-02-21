//
// Created by taylor-santos on 1/2/2022 at 20:23.
//

#include "ast/type_definition.hpp"
#include "ast/type.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/error.hpp"

#include <utility>
#include <ostream>
#include <sstream>

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
    auto name = get_name();
    auto prev = ctx.get_symbol(name);
    if (prev) {
        using namespace print;
        ctx.set_failure(true);
        auto message = Message::error(get_loc().begin)
                           .with_message("`" + name + "` already defined")
                           .in(color::bold_gray);
        auto prev_loc = prev->get().get_loc();
        if (prev_loc) {
            message.with_detail_at(*prev_loc)
                .with_message("previous definition of `" + name + "` here")
                .in(color::bold_yellow);
        }
        ctx.add_message(message);
        return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    }
    auto &type = type_->get_type(ctx);
    using namespace TypeChecker;
    ctx.set_symbol(name, type, get_loc(), Uninit{Uninit::Reason::NOT_DEFINED, *this});
    return type;
}

} // namespace AST
