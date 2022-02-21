//
// Created by taylor-santos on 2/8/2022 at 12:46.
//

#pragma once

#include "type/type.hpp"

namespace TypeChecker {

class Error final : public Type {
public:
    explicit Error(yy::location loc);
    ~Error() override;

    void
    print(std::ostream &os) const override;
};

} // namespace TypeChecker
