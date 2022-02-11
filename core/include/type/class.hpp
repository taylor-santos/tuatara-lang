//
// Created by taylor-santos on 2/7/2022 at 19:19.
//

#pragma once

#include <unordered_map>

#include "type/type.hpp"

namespace TypeChecker {

class Class final : public Type {
public:
    explicit Class(std::string name);
    ~Class();

    [[nodiscard]] std::string
    get_name() const;

    void
    print(std::ostream &os) const override;

private:
    std::string                                                name_;
    std::unordered_map<std::string, const TypeChecker::Type &> fields_;
};

} // namespace TypeChecker
