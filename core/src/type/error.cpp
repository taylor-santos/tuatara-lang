//
// Created by taylor-santos on 2/8/2022 at 12:46.
//

#include "type/error.hpp"

namespace TypeChecker {

Error::Error() = default;

Error::~Error() = default;

void
Error::print(std::ostream &os) const {
    os << "<error>";
}

} // namespace TypeChecker
