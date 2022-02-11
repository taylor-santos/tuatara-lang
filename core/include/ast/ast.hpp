//
// Created by taylor-santos on 1/2/2022 at 20:07.
//

#pragma once

#include <memory>

#include "type/type.hpp"

namespace yy {
class location;
}

namespace TypeChecker {
class Context;
}

namespace AST {

class Node {
public:
    explicit Node(const yy::location &loc);
    Node(const Node &) = delete;
    virtual ~Node();

    virtual void
    to_json(std::ostream &os) const = 0;

    [[nodiscard]] const yy::location &
    get_loc() const;

    [[nodiscard]] virtual const TypeChecker::Type &
    get_type(TypeChecker::Context &ctx) const = 0;

private:
    std::unique_ptr<yy::location> loc_;
};

} // namespace AST
