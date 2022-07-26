//
// Created by taylor-santos on 7/25/2022 at 14:16.
//

#include "ast/array.hpp"
#include "type/error.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/array.hpp"
#include "type/tuple.hpp"
#include "printer.hpp"

#include <ostream>
#include <algorithm>

namespace AST {

Array::Array(std::vector<std::unique_ptr<AST::Expression>> &&values, const yy::location &loc)
    : Node(loc)
    , values_{std::move(values)} {}

Array::Array(const yy::location &loc)
    : Node(loc)
    , values_{} {}

Array::~Array() = default;

void
Array::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"array",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("values":[)";
    std::string sep;
    for (auto &value : values_) {
        os << sep;
        sep = ",";
        value->to_json(os);
    }
    os << "]}";
}

const TypeChecker::Type &
Array::get_type(TypeChecker::Context &ctx) const {
    if (values_.empty()) {
        // Cannot infer anything about the array type, it can only be array of ()
        auto &unit = ctx.add_type(std::make_unique<TypeChecker::Tuple>(get_loc()));
        return ctx.add_type(std::make_unique<TypeChecker::Array>(unit, get_loc()));
    }
    std::vector<const TypeChecker::Type *> types;
    types.reserve(values_.size());
    std::transform(values_.begin(), values_.end(), std::back_inserter(types), [&ctx](auto &value) {
        return &value->get_type(ctx);
    });
    auto *curr_type = types.front();
    auto  failures  = std::vector<std::tuple<size_t, const TypeChecker::Type *, yy::location>>();
    for (size_t i = 1; i < types.size(); i++) {
        auto &type = types[i];
        auto  rel  = type->compare(*curr_type);
        switch (rel) {
            case TypeChecker::Relation::SAME_TYPE:
            case TypeChecker::Relation::SUB_TYPE: break;
            case TypeChecker::Relation::SUPER_TYPE: curr_type = type; break;
            case TypeChecker::Relation::UNRELATED:
                failures.emplace_back(i, type, values_[i]->get_loc());
                break;
        }
    }
    if (!failures.empty()) {
        failures.emplace(failures.begin(), 0, types.front(), values_.front()->get_loc());
        using namespace print;
        ctx.set_failure(true);
        auto message = Message::error(get_loc().begin)
                           .with_message(
                               "unable to determine common type of array elements",
                               color::bold_gray);
        for (auto &[index, type, loc] : failures) {
            auto name = type->print();
            // TODO: add functionality to cycle through a list of colors
            message.with_detail(loc, color::bold_red)
                .with_message("element ", color::bold_gray)
                .with_message(std::to_string(index), color::bold_red)
                .with_message(" has type `", color::bold_gray)
                .with_message(name, color::bold_red)
                .with_message("`", color::bold_gray);
        }
        ctx.add_message(message);
        return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
    }
    return ctx.add_type(std::make_unique<TypeChecker::Array>(*curr_type, get_loc()));
}

} // namespace AST
