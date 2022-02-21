//
// Created by taylor-santos on 2/8/2022 at 19:14.
//

#pragma once

#include <unordered_map>

#include "type/type.hpp"
#include "type/class.hpp"

namespace TypeChecker {

class Object final : public Type {
public:
    Object(const Class &cl, yy::location loc);
    ~Object() override;

    [[nodiscard]] const Class &
    get_class() const;

    void
    print(std::ostream &os) const override;

private:
    const Class &class_;
};

} // namespace TypeChecker
