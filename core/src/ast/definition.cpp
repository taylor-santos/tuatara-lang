//
// Created by taylor-santos on 1/2/2022 at 20:16.
//

#include "ast/definition.hpp"
#include "location.hh"

#include <utility>

namespace AST {

Definition::Definition(std::string &&name, const yy::location &name_loc)
    : name_{std::move(name)}
    , name_loc_{std::make_unique<yy::location>(name_loc)} {}

Definition::~Definition() = default;

const std::string &
Definition::get_name() const {
    return name_;
}

const yy::location &
Definition::get_name_loc() const {
    return *name_loc_;
}

} // namespace AST
