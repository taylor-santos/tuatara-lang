//
// Created by taylor-santos on 7/25/2022 at 16:19.
//

#pragma once

#include "ast/expression.hpp"

#include <vector>

namespace AST {

class Index final : public Expression {
public:
    Index(
        std::unique_ptr<AST::Expression> array,
        std::unique_ptr<AST::Expression> index,
        const yy::location              &loc);

    ~Index() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::unique_ptr<AST::Expression> array_;
    std::unique_ptr<AST::Expression> index_;
};

} // namespace AST
