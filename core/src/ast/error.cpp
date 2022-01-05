//
// Created by taylor-santos on 1/2/2022 at 22:01.
//

#include "ast/error.hpp"
#include "json.hpp"

#include <ostream>

namespace AST {

Error::Error(const yy::location &loc)
    : Node(loc) {}

Error::~Error() = default;

void
Error::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"error",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << "}";
}

} // namespace AST
