//
// Created by taylor-santos on 1/2/2022 at 21:03.
//

#include "ast/object_type.hpp"
#include "json.hpp"

#include <utility>
#include <ostream>

namespace AST {

ObjectType::ObjectType(std::string name, const yy::location &name_loc, const yy::location &loc)
    : Node(loc)
    , name_{std::move(name)}
    , name_loc_{std::make_unique<yy::location>(name_loc)} {}

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
    loc_to_json(*name_loc_, os);
    os << "}";
}

} // namespace AST
