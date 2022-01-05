//
// Created by taylor-santos on 1/2/2022 at 21:28.
//

#include "ast/variable.hpp"
#include "json.hpp"

#include <utility>
#include <ostream>

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

} // namespace AST
