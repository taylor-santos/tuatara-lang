//
// Created by taylor-santos on 1/2/2022 at 20:37.
//

#include "ast/u64.hpp"
#include "json.hpp"

#include <ostream>

namespace AST {

U64::U64(uint64_t value, const yy::location &loc)
    : Node(loc)
    , value_{value} {}

U64::~U64() = default;

void
U64::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"u64",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("value":")" << value_ << R"(")"
       << "}";
}

} // namespace AST
