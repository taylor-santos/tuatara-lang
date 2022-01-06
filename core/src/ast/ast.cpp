//
// Created by taylor-santos on 1/2/2022 at 20:08.
//

#include "ast/ast.hpp"

#include "location.hh"

namespace AST {

Node::Node(const yy::location &loc)
    : loc_{std::make_unique<yy::location>(loc)} {}

Node::~Node() = default;

const yy::location &
Node::get_loc() const {
    return *loc_;
}

} // namespace AST
