//
// Created by taylor-santos on 2/7/2022 at 19:06.
//

#pragma once

#include <ostream>
#include <optional>

#include "location.hh"

namespace TypeChecker {

enum class Relation {
    SUB_TYPE,
    SAME_TYPE,
    SUPER_TYPE,
    UNRELATED,
};

// Returns true if this Relation enum represents an is-a relationship.
// If A is a B (i.e. A can be used anywhere a B is expected), then A.compare(B) will return a
// Relation that if passed into this function will return true.
bool
is_a_relationship(Relation rel);

Relation
invert(Relation rel);

class Type {
public:
    explicit Type(std::optional<yy::location> loc);
    Type(const Type &) = delete;
    virtual ~Type();

    [[nodiscard]] std::optional<yy::location>
    get_loc() const;

    virtual void
    print(std::ostream &os, bool paren = false) const = 0;

    [[nodiscard]] std::string
    print() const;

    [[nodiscard]] virtual Relation
    compare(const Type &other) const;

protected:
    [[nodiscard]] virtual Relation
    get_relation(const Type &other) const = 0;

private:
    std::optional<yy::location> loc_;
};

std::ostream &
operator<<(std::ostream &os, const Type &type);

} // namespace TypeChecker
