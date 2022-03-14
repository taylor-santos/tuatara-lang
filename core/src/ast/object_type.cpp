//
// Created by taylor-santos on 1/2/2022 at 21:03.
//

#include "ast/object_type.hpp"
#include "type/type_checker.hpp"
#include "type/class.hpp"
#include "json.hpp"
#include "type/object.hpp"
#include "type/error.hpp"
#include "printer.hpp"

#include <utility>
#include <ostream>

namespace AST {

ObjectType::ObjectType(std::string name, const yy::location &name_loc, const yy::location &loc)
    : Node(loc)
    , name_{std::move(name)}
    , name_loc_{name_loc} {}

ObjectType::~ObjectType() = default;

void
ObjectType::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"object type",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("class":")" << escape_string(name_) << R"(",)"
       << R"("name location":)";
    loc_to_json(name_loc_, os);
    os << "}";
}

const TypeChecker::Type &
ObjectType::get_type(TypeChecker::Context &ctx) const {
    auto cl = ctx.get_class(name_);
    if (!cl) {
        using namespace print;
        ctx.set_failure(true);
        auto message = Message::error(get_loc().begin)
                           .with_message("`", color::bold_gray)
                           .with_message(name_, color::bold_red)
                           .with_message("` does not name a type", color::bold_gray)
                           .with_detail(name_loc_, color::bold_red)
                           .with_message("`", color::bold_gray)
                           .with_message(name_, color::bold_red)
                           .with_message("` used here", color::bold_gray);
        ctx.add_message(message);
        return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    }
    return ctx.add_type(std::make_unique<TypeChecker::Object>(cl->get(), get_loc()));
}

} // namespace AST
