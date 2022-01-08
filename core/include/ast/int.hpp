//
// Created by taylor-santos on 1/2/2022 at 20:36.
//

#pragma once

#include "ast/literal.hpp"

namespace AST {

template<typename T>
class Int final : public Literal {
public:
    Int(T value, const yy::location &loc);
    ~Int() override;

    void
    to_json(std::ostream &os) const override;

public:
    static const char *type_name;

private:
    T value_;
};

} // namespace AST
