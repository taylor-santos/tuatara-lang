//
// Created by taylor-santos on 6/29/2022 at 11:51.
//

#include "ast/type_value_definition.hpp"
#include "ast/type.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/error.hpp"
#include "printer.hpp"

#include <utility>
#include <ostream>
#include <sstream>

namespace AST {

TypeValueDefinition::TypeValueDefinition(
    std::string                    &&name,
    const yy::location              &name_loc,
    std::unique_ptr<AST::Type>       type,
    std::unique_ptr<AST::Expression> value,
    const yy::location              &loc)
    : Node(loc)
    , Definition(std::move(name), name_loc)
    , type_{std::move(type)}
    , value_{std::move(value)} {}

TypeValueDefinition::~TypeValueDefinition() = default;

void
TypeValueDefinition::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"type value definition",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("name":")" << escape_string(get_name()) << R"(",)"
       << R"("name location":)";
    loc_to_json(get_name_loc(), os);
    os << ","
       << R"("type":)";
    type_->to_json(os);
    os << ","
       << R"("value":)";
    value_->to_json(os);
    os << "}";
}

const TypeChecker::Type &
TypeValueDefinition::get_type(TypeChecker::Context &ctx) const {
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
    auto &value_type = value_->get_type(ctx);

    auto rel = value_type.compare(type);
    switch (rel) {
        case Relation::SAME_TYPE:
        case Relation::SUB_TYPE: break;
        case Relation::SUPER_TYPE:
        case Relation::UNRELATED:
            using namespace print;
            ctx.set_failure(true);
            {
                auto message = Message::error(get_loc().begin)
                                   .with_message("variable `", color::bold_gray)
                                   .with_message(name, color::bold_red)
                                   .with_message(
                                       "` declaration has conflicting type and value",
                                       color::bold_gray);
                auto type_name       = type.print();
                auto value_type_name = value_type.print();

                message.with_detail(*type.get_loc(), color::bold_yellow)
                    .with_message("declared type is `", color::bold_gray)
                    .with_message(type_name, color::bold_yellow)
                    .with_message("`", color::bold_gray);
                message.with_detail(*value_type.get_loc(), color::bold_red)
                    .with_message("value has type `", color::bold_gray)
                    .with_message(value_type_name, color::bold_red)
                    .with_message("`", color::bold_gray);
                ctx.add_message(message);
            }
            return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    }

    ctx.set_symbol(name, type, get_name_loc());
    return type;
}

} // namespace AST
