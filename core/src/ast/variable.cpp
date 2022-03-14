//
// Created by taylor-santos on 1/2/2022 at 21:28.
//

#include "ast/variable.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/error.hpp"
#include "printer.hpp"

#include <utility>
#include <ostream>

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
        using namespace print;

        ctx.set_failure(true);
        auto message = Message::error(get_loc().begin)
                           .with_message("`", color::bold_gray)
                           .with_message(name_, color::bold_red)
                           .with_message("` was not declared in this scope", color::bold_gray)
                           .with_detail(get_loc(), color::bold_red)
                           .with_message("`", color::bold_gray)
                           .with_message(name_, color::bold_red)
                           .with_message("` used here", color::bold_gray);
        ctx.add_message(message);
        return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    } else {
        auto uninit = ctx.is_uninitialized(name_);
        if (uninit) {
            using namespace print;

            ctx.set_failure(true);
            auto loc     = get_loc();
            auto def_loc = ctx.get_symbol_loc(name_);
            auto message = Message::error(get_loc().begin);
            switch (uninit->reason) {
                case Uninit::Reason::NOT_DEFINED:
                    message.with_message("variable `", color::bold_gray)
                        .with_message(name_, color::bold_red)
                        .with_message("` used before initialization", color::bold_gray);
                    message.with_detail(uninit->loc, color::bold_yellow)
                        .with_message("`", color::bold_gray)
                        .with_message(name_, color::bold_yellow)
                        .with_message(
                            "` declared without being initialized here",
                            color::bold_gray);
                    message.with_detail(loc, color::bold_red)
                        .with_message("`", color::bold_gray)
                        .with_message(name_, color::bold_red)
                        .with_message("` used here", color::bold_gray);
                    break;
                case Uninit::Reason::MOVED_FROM:
                    message.with_message("variable `", color::bold_gray)
                        .with_message(name_, color::bold_red)
                        .with_message("` used after being moved", color::bold_gray);
                    if (def_loc) {
                        message.with_detail(*def_loc, color::bold_yellow)
                            .with_message("value assigned to `", color::bold_gray)
                            .with_message(name_, color::bold_yellow)
                            .with_message("` here", color::bold_gray);
                    }
                    message.with_detail(uninit->loc, color::bold_yellow)
                        .with_message("value moved out of `", color::bold_gray)
                        .with_message(name_, color::bold_yellow)
                        .with_message("` here", color::bold_gray);
                    message.with_detail(loc, color::bold_red)
                        .with_message("`", color::bold_gray)
                        .with_message(name_, color::bold_red)
                        .with_message("` used here after move", color::bold_gray);
                    break;
            }
            ctx.add_message(message);
            // We can fall through and return `t` here instead of an `Error` type, because the
            // expected type of the variable can still be known even though it isn't allowed to be
            // used. This is still a type error, but later type errors can still be inferred from
            // this type.
        } else {
            ctx.set_symbol(name_, *type, get_loc(), Uninit{Uninit::Reason::MOVED_FROM, get_loc()});
        }
        return *type;
    }
}

} // namespace AST
