//
// Created by taylor-santos on 1/2/2022 at 20:18.
//

#include "ast/value_definition.hpp"
#include "json.hpp"

#include <utility>
#include <ostream>

namespace AST {

ValueDefinition::ValueDefinition(
    std::string                 &&name,
    const yy::location           &name_loc,
    std::unique_ptr<Expression> &&value,
    const yy::location           &loc)
    : Node(loc)
    , Definition(std::move(name), name_loc)
    , value_{std::move(value)} {}

ValueDefinition::~ValueDefinition() = default;

void
ValueDefinition::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"value definition",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("name":")" << escape_string(get_name()) << R"(",)"
       << R"("name location":)";
    loc_to_json(get_name_loc(), os);
    os << ","
       << R"("value":)";
    value_->to_json(os);
    os << "}";
}

} // namespace AST
