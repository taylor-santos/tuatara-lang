//
// Created by taylor-santos on 1/2/2022 at 20:23.
//

#include "ast/type_definition.hpp"
#include "ast/type.hpp"
#include "json.hpp"

#include <utility>
#include <ostream>

namespace AST {

TypeDefinition::TypeDefinition(
    std::string              &&name,
    const yy::location        &name_loc,
    std::unique_ptr<AST::Type> type,
    const yy::location        &loc)
    : Node(loc)
    , Definition(std::move(name), name_loc)
    , type_{std::move(type)} {}

TypeDefinition::~TypeDefinition() = default;

void
TypeDefinition::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"type definition",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("name":")" << escape_string(get_name()) << R"(",)"
       << R"("name location":)";
    loc_to_json(get_name_loc(), os);
    os << ","
       << R"("type":)";
    type_->to_json(os);
    os << "}";
}

} // namespace AST
