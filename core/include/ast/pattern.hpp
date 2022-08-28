//
// Created by taylor-santos on 8/20/2022 at 14:54.
//

#pragma once

#include "ast/ast.hpp"
#include "ast/type.hpp"

namespace AST {

class Pattern : public Node {
public:
    explicit Pattern(const yy::location &loc);
};

class TypeNamePattern : public Pattern {
public:
    TypeNamePattern(
        std::string              &&name,
        const yy::location        &name_loc,
        std::unique_ptr<AST::Type> type_,
        const yy::location        &loc);

    ~TypeNamePattern() override;

    void
    to_json(std::ostream &os) const override;

    const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::string                name_;
    yy::location               name_loc_;
    std::unique_ptr<AST::Type> type_;
};

class GenericNamePattern : public Pattern {
public:
    GenericNamePattern(std::string &&name, const yy::location &loc);

    ~GenericNamePattern() override;

    void
    to_json(std::ostream &os) const override;

    const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const override;

private:
    std::string name_;
};

} // namespace AST
