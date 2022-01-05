//
// Created by taylor-santos on 1/2/2022 at 20:07.
//

#pragma once

#include <memory>

namespace yy {
class location;
}

namespace AST {

class Node {
public:
    explicit Node(const yy::location &loc);
    virtual ~Node();

    virtual void
    to_json(std::ostream &os) const = 0;

    [[nodiscard]] const yy::location &
    get_loc() const;

private:
    std::unique_ptr<yy::location> loc_;
};

} // namespace AST
