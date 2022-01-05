//
// Created by taylor-santos on 1/2/2022 at 20:18.
//

#pragma once

#include "ast/definition.hpp"

#include <memory>

namespace AST {

class ValueDefinition final : public Definition {
public:
    ValueDefinition(
        std::string                 &&name,
        const yy::location           &name_loc,
        std::unique_ptr<Expression> &&value,
        const yy::location           &loc);
    ~ValueDefinition() override;

    void
    to_json(std::ostream &os) const override;

private:
    std::unique_ptr<Expression> value_;
};

} // namespace AST
