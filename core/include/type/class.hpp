//
// Created by taylor-santos on 2/7/2022 at 19:19.
//

#pragma once

#include <unordered_map>

#include "type/type.hpp"

namespace TypeChecker {

class Class final : public Type {
public:
    Class(std::string name, std::optional<yy::location> loc = {});
    ~Class() override;

    [[nodiscard]] std::string
    get_name() const;

    void
    print(std::ostream &os, bool paren) const override;

protected:
    [[nodiscard]] Relation
    get_relation(const Type &other) const override;

private:
    std::string                                                name_;
    std::unordered_map<std::string, const TypeChecker::Type &> fields_;
};

} // namespace TypeChecker
