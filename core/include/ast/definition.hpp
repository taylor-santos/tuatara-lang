//
// Created by taylor-santos on 1/2/2022 at 20:15.
//

#pragma once

#include <string>

#include "ast/expression.hpp"

namespace AST {

class Definition : public Expression {
public:
    Definition(std::string &&name, const yy::location &name_loc);

    ~Definition() override;

protected:
    [[nodiscard]] const std::string &
    get_name() const;

    [[nodiscard]] const yy::location &
    get_name_loc() const;

private:
    std::string                   name_;
    std::unique_ptr<yy::location> name_loc_;
};

} // namespace AST
