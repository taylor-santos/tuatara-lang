//
// Created by taylor-santos on 7/14/2022 at 12:20.
//

#pragma once

#include "ast/simple_expression.hpp"

#include <vector>
#include <variant>

namespace AST {

class Block final : public SimpleExpression {
public:
    Block(
        std::vector<std::unique_ptr<AST::Expression>> lines,
        std::unique_ptr<AST::Expression>              last_line,
        const yy::location                           &loc);
    Block(std::unique_ptr<AST::Expression> last_line, const yy::location &loc);
    Block(
        std::vector<std::unique_ptr<AST::Expression>> lines,
        const yy::location                           &val_loc,
        const yy::location                           &loc);
    Block(const yy::location &val_loc, const yy::location &loc);
    ~Block() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::vector<std::unique_ptr<AST::Expression>>                lines_;
    std::variant<std::unique_ptr<AST::Expression>, yy::location> last_line_;
};

} // namespace AST
