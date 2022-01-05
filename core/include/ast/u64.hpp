//
// Created by taylor-santos on 1/2/2022 at 20:36.
//

#pragma once

#include "ast/literal.hpp"

namespace AST {

class U64 final : public Literal {
public:
    U64(uint64_t value, const yy::location &loc);
    ~U64() override;

    void
    to_json(std::ostream &os) const override;

private:
    uint64_t value_;
};

} // namespace AST
