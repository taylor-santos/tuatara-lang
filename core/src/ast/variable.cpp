//
// Created by taylor-santos on 1/2/2022 at 21:28.
//

#include "ast/variable.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/error.hpp"

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
        using namespace print;

        ctx.set_failure(true);
        auto message = Message::error(get_loc().begin)
                           .with_message("`" + name_ + "` was not declared in this scope")
                           .in(color::bold_gray)
                           .with_detail_at(get_loc())
                           .with_message("used here")
                           .in(color::bold_red);
        ctx.add_message(message);
        return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    } else {
        auto &t      = type->get();
        auto  uninit = ctx.is_uninitialized(name_);
        if (uninit) {
            using namespace print;

            ctx.set_failure(true);
            auto loc      = get_loc();
            auto type_loc = type->get().get_loc();
            auto message  = Message::error(get_loc().begin);
            switch (uninit->reason) {
                case Uninit::Reason::NOT_DEFINED:
                    message.with_message("`" + name_ + "` used before initialization")
                        .in(color::bold_gray);
                    message.with_detail_at(uninit->node.get_loc())
                        .with_message("`" + name_ + "` declared without being initialized here")
                        .in(color::bold_yellow);
                    message.with_detail_at(loc)
                        .with_message("`" + name_ + "` used here")
                        .in(color::bold_red);
                    break;
                case Uninit::Reason::MOVED_FROM:
                    message.with_message("`" + name_ + "` used after being moved")
                        .in(color::bold_gray);
                    if (type_loc) {
                        message.with_detail_at(*type_loc)
                            .with_message("value assigned to `" + name_ + "` here")
                            .in(color::bold_yellow);
                    }
                    message.with_detail_at(uninit->node.get_loc())
                        .with_message("value moved out of `" + name_ + "` here")
                        .in(color::bold_cyan);
                    message.with_detail_at(loc)
                        .with_message("`" + name_ + "` used here after move")
                        .in(color::bold_red);
                    break;
            }
            ctx.add_message(message);
            // We can fall through and return `t` here instead of an `Error` type, because the
            // expected type of the variable can still be known even though it isn't allowed to be
            // used. This is still a type error, but later type errors can still be inferred from
            // this type.
        } else {
            ctx.set_symbol(name_, t, get_loc(), Uninit{Uninit::Reason::MOVED_FROM, *this});
        }
        return t;
    }
}

} // namespace AST
