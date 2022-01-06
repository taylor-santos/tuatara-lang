//
// Created by taylor-santos on 1/2/2022 at 20:23.
//

#pragma once

#include "ast/definition.hpp"

#include <memory>

namespace AST {
class Type;
} // namespace AST

namespace AST {

class TypeDefinition final : public Definition {
public:
    TypeDefinition(
        std::string              &&name,
        const yy::location        &name_loc,
        std::unique_ptr<AST::Type> type,
        const yy::location        &loc);
    ~TypeDefinition() override;

    void
    to_json(std::ostream &os) const override;

private:
    std::unique_ptr<AST::Type> type_;
};

} // namespace AST
