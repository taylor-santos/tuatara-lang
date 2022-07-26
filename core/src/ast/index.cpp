//
// Created by taylor-santos on 7/25/2022 at 16:21.
//

#include "ast/index.hpp"
#include "type/error.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/array.hpp"
#include "type/object.hpp"
#include "printer.hpp"

#include <ostream>
#include <sstream>

namespace AST {

Index::Index(
    std::unique_ptr<AST::Expression> array,
    std::unique_ptr<AST::Expression> index,
    const yy::location              &loc)
    : Node(loc)
    , array_{std::move(array)}
    , index_{std::move(index)} {}

Index::~Index() = default;

void
Index::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"index",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("array":)";
    array_->to_json(os);
    os << ","
       << R"("index":)";
    index_->to_json(os);
    os << "}";
}

const TypeChecker::Type &
Index::get_type(TypeChecker::Context &ctx) const {
    auto &index_type = index_->get_type(ctx);
    auto &obj_type   = array_->get_type(ctx);

    auto *array_type = dynamic_cast<const TypeChecker::Array *>(&obj_type);

    if (!array_type) {
        using namespace print;
        ctx.set_failure(true);

        auto obj_str = obj_type.print();

        auto message =
            Message::error(array_->get_loc().begin)
                .with_message("attempting to index into a value of type `", color::bold_gray)
                .with_message(obj_str, color::bold_red)
                .with_message("`, expected an array type", color::bold_gray);

        message.with_detail(array_->get_loc(), color::bold_red)
            .with_message("value indexed here", color::bold_gray);

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

    // TODO: support indexing with more than just U64
    auto *obj = dynamic_cast<const TypeChecker::Object *>(&index_type);
    if (obj) {
        auto int_rel = obj->get_class().compare(ctx.builtins.U64);
        if (TypeChecker::is_a_relationship(int_rel)) {
            return array_type->element_type();
        }
    }
    // Either the index value isn't an object or it's an object of non-U64 type.
    using namespace print;
    ctx.set_failure(true);

    auto name    = index_type.print();
    auto message = Message::error(index_->get_loc().begin)
                       .with_message("array cannot be indexed by value of type `", color::bold_gray)
                       .with_message(name, color::bold_red)
                       .with_message("`", color::bold_gray);

    message.with_detail(index_->get_loc(), color::bold_red)
        .with_message("value used as index has type `", color::bold_gray)
        .with_message(name, color::bold_red)
        .with_message("`", color::bold_gray);

    ctx.add_message(message);
    return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
}

} // namespace AST
