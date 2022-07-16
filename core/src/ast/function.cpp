//
// Created by taylor-santos on 7/15/2022 at 16:44.
//

#include "ast/function.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/func.hpp"
#include "type/error.hpp"
#include "printer.hpp"

#include <ostream>
#include <unordered_map>
#include <sstream>

namespace AST {

Function::Function(
    std::vector<arg_t>                   &&args,
    std::unique_ptr<AST::SimpleExpression> body,
    const yy::location                    &loc)
    : Node(loc)
    , args_{std::move(args)}
    , ret_type_{}
    , body_{std::move(body)} {}

Function::Function(
    std::vector<arg_t>                   &&args,
    std::unique_ptr<AST::Type>             ret_type,
    std::unique_ptr<AST::SimpleExpression> body,
    const yy::location                    &loc)
    : Node(loc)
    , args_{std::move(args)}
    , ret_type_{std::move(ret_type)}
    , body_{std::move(body)} {}

Function::~Function() = default;

void
Function::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"function",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("args":[)";
    std::string sep;
    for (auto &arg : args_) {
        os << sep;
        sep = ",";
        os << "{"
           << R"("name":")" << std::get<0>(arg) << R"(",)"
           << R"("type":)";
        std::get<1>(arg)->to_json(os);
        os << "}";
    }
    os << "]";
    if (ret_type_) {
        os << ","
           << R"("return type":)";
        (*ret_type_)->to_json(os);
    }
    os << ","
       << R"("body":)";
    body_->to_json(os);
    os << "}";
}

const TypeChecker::Type &
Function::get_type(TypeChecker::Context &ctx) const {
    auto new_ctx    = ctx;
    auto used_names = std::unordered_map<std::string, const arg_t *>();
    for (auto &arg : args_) {
        auto &[name, type, loc] = arg;

        auto prev = used_names.find(name);
        if (prev != used_names.end()) {
            using namespace print;
            ctx.set_failure(true);
            auto message =
                Message::error(loc.begin)
                    .with_message("multiple definitions of function argument `", color::bold_gray)
                    .with_message(name, color::bold_red)
                    .with_message("`", color::bold_gray);
            message.with_detail(std::get<yy::location>(*prev->second), color::bold_yellow)
                .with_message("`", color::bold_gray)
                .with_message(name, color::bold_yellow)
                .with_message("` first used here", color::bold_gray);
            message.with_detail(loc, color::bold_red)
                .with_message("`", color::bold_gray)
                .with_message(name, color::bold_red)
                .with_message("` reused here", color::bold_gray);
            ctx.add_message(message);
            return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
        }

        auto &arg_type = type->get_type(new_ctx);
        new_ctx.set_symbol(name, arg_type, loc);
        used_names[name] = &arg;
    }

    auto &body_type = body_->get_type(new_ctx);

    if (ret_type_) {
        auto &ret_type = (*ret_type_)->get_type(new_ctx);
        auto  rel      = body_type.compare(ret_type);
        if (!TypeChecker::is_a_relationship(rel)) {
            using namespace print;
            ctx.set_failure(true);
            auto ret_loc = (*ret_type_)->get_loc();
            auto message = Message::error(ret_loc.begin).with_message("TODO", color::bold_gray);
            {
                std::stringstream ret_str;
                ret_type.print(ret_str);
                message.with_detail(ret_loc, color::bold_red)
                    .with_message(ret_str.str(), color::bold_red);
            }
            {
                std::stringstream body_str;
                body_type.print(body_str);
                auto body_loc = body_type.get_loc().value_or(body_->get_loc());
                message.with_detail(body_loc, color::bold_yellow)
                    .with_message(body_str.str(), color::bold_yellow);
            }
            ctx.add_message(message);
            return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
        }
    }

    ctx.set_failure(ctx.did_fail() || new_ctx.did_fail());

    return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc())); // TODO
}

} // namespace AST
