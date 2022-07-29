//
// Created by taylor-santos on 7/6/2022 at 12:29.
//

#include "ast/func_call.hpp"
#include "type/error.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/func.hpp"
#include "printer.hpp"

#include <ostream>
#include <sstream>

namespace AST {

Call::Call(
    std::unique_ptr<AST::Expression>              func,
    std::vector<std::unique_ptr<AST::Expression>> args,
    const yy::location                           &args_loc,
    const yy::location                           &loc)
    : Node(loc)
    , func_{std::move(func)}
    , args_{std::move(args)}
    , args_loc_{args_loc} {}

Call::~Call() = default;

void
Call::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"call",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("function":)";
    func_->to_json(os);
    os << ","
       << R"("args":[)";
    std::string sep;
    for (auto &arg : args_) {
        os << sep;
        sep = ",";
        arg->to_json(os);
    }
    os << "]}";
}

const TypeChecker::Type &
Call::get_type(TypeChecker::Context &ctx) const {
    std::vector<const TypeChecker::Type *> arg_types;
    arg_types.reserve(args_.size());
    for (auto &arg : args_) {
        auto &type = arg->get_type(ctx);
        arg_types.push_back(&type);
    }

    auto &obj_type  = func_->get_type(ctx);
    auto *func_type = dynamic_cast<const TypeChecker::Func *>(&obj_type);

    if (!func_type) {
        using namespace print;
        ctx.set_failure(true);

        auto obj_str = obj_type.print();

        auto message = Message::error(func_->get_loc().begin)
                           .with_message("attempting to call a value of type `", color::bold_gray)
                           .with_message(obj_str, color::bold_red)
                           .with_message("`, expected a function type", color::bold_gray);

        message.with_detail(func_->get_loc(), color::bold_red)
            .with_message("value called here", color::bold_gray);

        auto type_loc = obj_type.get_loc();
        if (type_loc) {
            message.with_detail(*type_loc, color::bold_yellow)
                .with_message("value given type `", color::bold_gray)
                .with_message(obj_str, color::bold_yellow)
                .with_message("` here", color::bold_gray);
        }
        ctx.add_message(message);
        return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    }

    const auto *given_types    = &arg_types;
    const auto *expected_types = &func_type->arg_types();

    // Possible tuple-argument expansion
    if (given_types->size() == 1 && expected_types->size() != 1) {
        auto *tup = dynamic_cast<const TypeChecker::Tuple *>(arg_types.front());
        if (tup) {
            given_types = &tup->types();
        }
    }
    // Possible argument-tuple expansion
    if (arg_types.size() != 1 && expected_types->size() == 1) {
        auto *tup = dynamic_cast<const TypeChecker::Tuple *>(expected_types->front());
        if (tup) {
            expected_types = &tup->types();
        }
    }

    if (given_types->size() != expected_types->size()) {
        using namespace print;
        ctx.set_failure(true);

        auto message = Message::error(args_loc_.begin)
                           .with_message("function called with ", color::bold_gray)
                           .with_message(std::to_string(given_types->size()), color::bold_red)
                           .with_message(" argument", color::bold_gray);
        if (given_types->size() != 1) message.with_message("s", color::bold_gray);
        message.with_message(", but ", color::bold_gray)
            .with_message(std::to_string(expected_types->size()), color::bold_yellow)
            .with_message(" argument was expected", color::bold_gray);
        if (expected_types->size() != 1) message.with_message("s", color::bold_gray);

        auto func_name = func_type->Type::print();

        message.with_detail(func_->get_loc(), color::bold_magenta)
            .with_message("function called here", color::bold_gray);

        auto func_loc = func_type->get_loc().value_or(func_->get_loc());
        message.with_detail(func_loc, color::bold_yellow)
            .with_message("function has type `", color::bold_gray)
            .with_message(func_name, color::bold_yellow)
            .with_message("`, expecting ", color::bold_gray)
            .with_message(std::to_string(expected_types->size()), color::bold_yellow)
            .with_message(" argument", color::bold_gray);
        if (expected_types->size() != 1) message.with_message("s", color::bold_gray);

        message.with_detail(args_loc_, color::bold_red)
            .with_message(std::to_string(args_.size()), color::bold_red)
            .with_message(" argument", color::bold_gray);
        if (args_.size() != 1) message.with_message("s", color::bold_gray);
        message.with_message(" applied here", color::bold_gray);

        ctx.add_message(message);
        return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    }

    bool failed = false;
    for (size_t i = 0; i < given_types->size(); i++) {
        auto &got      = (*given_types)[i];
        auto &expected = (*expected_types)[i];
        auto  cmp      = got->compare(*expected);
        if (!TypeChecker::is_a_relationship(cmp)) {
            using namespace print;
            failed = true;
            ctx.set_failure(true);

            auto got_name = got->print();
            auto exp_name = expected->print();

            auto message =
                Message::error(args_loc_.begin).with_message("function argument", color::bold_gray);
            if (given_types->size() != 1) {
                message.with_message(" ", color::bold_gray)
                    .with_message(std::to_string(i + 1), color::bold_gray);
            }
            message.with_message(" given type `", color::bold_gray)
                .with_message(got_name, color::bold_red)
                .with_message("`, but type `", color::bold_gray)
                .with_message(exp_name, color::bold_yellow)
                .with_message("` was expected", color::bold_gray);

            message.with_detail(args_[i]->get_loc(), color::bold_red)
                .with_message("function called with type `", color::bold_gray)
                .with_message(got_name, color::bold_red)
                .with_message("`, but `", color::bold_gray)
                .with_message(exp_name, color::bold_yellow)
                .with_message("` was expected", color::bold_gray);

            auto func_loc = func_type->arg_types()[i]->get_loc().value_or(
                func_type->get_loc().value_or(func_->get_loc()));
            message.with_detail(func_loc, color::bold_yellow)
                .with_message("function expects argument", color::bold_gray);
            if (given_types->size() != 1) {
                message.with_message(" ", color::bold_gray)
                    .with_message(std::to_string(i + 1), color::bold_gray);
            }
            message.with_message(" to have type `", color::bold_gray)
                .with_message(exp_name, color::bold_yellow)
                .with_message("`", color::bold_gray);

            if (got->get_loc()) {
                // TODO: Don't show if loc is the same as argument location.
                // Example: foo(5) will give two different details on the `5` for both the argument
                // having the wrong type and the `5` being the source of its type.
                message.with_detail(*got->get_loc(), color::bold_magenta)
                    .with_message("argument value given type `", color::bold_gray)
                    .with_message(got_name, color::bold_magenta)
                    .with_message("` here", color::bold_gray);
            }

            ctx.add_message(message);
        }
    }
    if (failed) {
        return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    }
    /*
    auto arg_rel = arg_type.compare(expected_arg_type);
    if (!TypeChecker::is_a_relationship(arg_rel)) {
        using namespace print;
        ctx.set_failure(true);

        auto arg_name  = arg_type.print();
        auto func_name = func_type->Type::print();

        std::string exp_name;
        {
            std::stringstream ss;
            std::string       sep;
            for (auto &type : expected_arg_type.types()) {
                ss << sep;
                sep = ", ";
                type->print(ss);
            }
            exp_name = ss.str();
        }

        auto message =
            Message::error(arg_->get_loc().begin)
                .with_message("function called with incompatible argument type `", color::bold_gray)
                .with_message(arg_name, color::bold_red)
                .with_message("`, expects type `", color::bold_gray)
                .with_message(exp_name, color::bold_yellow)
                .with_message("`", color::bold_gray);

        message.with_detail(func_->get_loc(), color::bold_yellow)
            .with_message("function expects an argument of type `", color::bold_gray)
            .with_message(exp_name, color::bold_yellow)
            .with_message("`", color::bold_gray);

        message.with_detail(arg_->get_loc(), color::bold_red)
            .with_message("argument has incompatible type `", color::bold_gray)
            .with_message(arg_name, color::bold_red)
            .with_message("`", color::bold_gray);

        auto func_loc = func_type->get_loc();
        if (func_loc) {
            message.with_detail(*func_loc, color::bold_magenta)
                .with_message("function given type `", color::bold_gray)
                .with_message(func_name, color::bold_magenta)
                .with_message("` here", color::bold_gray);
        }
        ctx.add_message(message);
        return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    }
    */

    return func_type->ret_type();
}

} // namespace AST
