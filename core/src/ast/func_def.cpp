//
// Created by taylor-santos on 7/15/2022 at 16:44.
//

#include "ast/func_def.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/func.hpp"
#include "type/error.hpp"
#include "printer.hpp"

#include <ostream>
#include <unordered_map>

namespace AST {

Function::Function(
    std::vector<arg_t>             &&args,
    const yy::location              &sig_loc,
    std::unique_ptr<AST::Expression> body,
    const yy::location              &loc)
    : Node(loc)
    , args_{std::move(args)}
    , sig_loc_{sig_loc}
    , ret_type_{}
    , body_{std::move(body)} {}

Function::Function(
    std::vector<arg_t>             &&args,
    std::unique_ptr<AST::Type>       ret_type,
    const yy::location              &args_loc,
    std::unique_ptr<AST::Expression> body,
    const yy::location              &loc)
    : Node(loc)
    , args_{std::move(args)}
    , sig_loc_{args_loc}
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
        std::get<2>(arg)->to_json(os);
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
    ctx_.emplace(ctx);
    auto &new_ctx = *ctx_;

    auto used_names = std::unordered_map<std::string, const arg_t *>();
    auto arg_types  = std::vector<const TypeChecker::Type *>();
    auto failed     = false;
    for (auto &arg : args_) {
        auto &[name, loc, type] = arg;

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
            failed = true;
        }

        auto &arg_type = type->get_type(new_ctx);
        new_ctx.set_symbol(name, arg_type, loc);
        used_names[name] = &arg;
        arg_types.push_back(&arg_type);
    }

    if (failed) {
        return ctx.add_type(std::make_unique<TypeChecker::Error>(sig_loc_));
    }

    auto &body_type = body_->get_type(new_ctx);

    if (!ret_type_) {
        // Unfortunately the location of the type needs to include the entire body of the function,
        // because the user did not specify an explicit return type.
        return ctx.add_type(std::make_unique<TypeChecker::Func>(arg_types, body_type, get_loc()));
    }

    auto &ret_type = (*ret_type_)->get_type(new_ctx);
    auto  rel      = body_type.compare(ret_type);
    if (!TypeChecker::is_a_relationship(rel)) {
        using namespace print;
        ctx.set_failure(true);

        auto ret_loc  = (*ret_type_)->get_loc();
        auto body_loc = body_type.get_loc().value_or(body_->get_loc());
        auto ret_str  = ret_type.print();
        auto body_str = body_type.print();

        auto message =
            Message::error(body_loc.begin)
                .with_message("function declared with return type `", color::bold_gray)
                .with_message(ret_str, color::bold_red)
                .with_message("` but returns a value of incompatible type `", color::bold_gray)
                .with_message(body_str, color::bold_yellow)
                .with_message("`", color::bold_gray);

        message.with_detail(ret_loc, color::bold_yellow)
            .with_message("return type `", color::bold_gray)
            .with_message(ret_str, color::bold_yellow)
            .with_message("` declared here", color::bold_gray);

        message.with_detail(body_loc, color::bold_red)
            .with_message("returns value with type `", color::bold_gray)
            .with_message(body_str, color::bold_red)
            .with_message("` here", color::bold_gray);

        ctx.add_message(message);
        return ctx.add_type(std::make_unique<TypeChecker::Error>(sig_loc_));
    }

    ctx.set_failure(ctx.did_fail() || new_ctx.did_fail());

    return ctx.add_type(std::make_unique<TypeChecker::Func>(arg_types, ret_type, sig_loc_));
}

} // namespace AST
