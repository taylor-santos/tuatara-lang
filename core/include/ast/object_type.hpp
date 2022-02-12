//
// Created by taylor-santos on 1/2/2022 at 21:02.
//

#pragma once

#include <string>

#include "ast/type.hpp"

namespace AST {

class ObjectType final : public Type {
public:
    ObjectType(std::string name, const yy::location &name_loc, const yy::location &loc);
    ~ObjectType() override;

    void
    to_json(std::ostream &os) const override;

    [[nodiscard]] const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::string                   name_;
    std::unique_ptr<yy::location> name_loc_;
};

} // namespace AST
