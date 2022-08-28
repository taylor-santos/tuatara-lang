//
// Created by taylor-santos on 8/20/2022 at 14:54.
//

#include "ast/pattern.hpp"
#include "type/type_checker.hpp"
#include "type/error.hpp"

#include <utility>

namespace AST {

Pattern::Pattern(const yy::location &loc)
    : Node(loc) {}

TypeNamePattern::TypeNamePattern(
    std::string              &&name,
    const yy::location        &name_loc,
    std::unique_ptr<AST::Type> type,
    const yy::location        &loc)
    : Pattern(loc)
    , name_{std::move(name)}
    , name_loc_{name_loc}
    , type_{std::move(type)} {}

TypeNamePattern::~TypeNamePattern() = default;

void
TypeNamePattern::to_json(std::ostream &os) const {
    // TODO unimplemented
    (void)os;
}

const TypeChecker::Type &
TypeNamePattern::get_type(TypeChecker::Context &ctx) const {
    // TODO unimplemented
    return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
}

GenericNamePattern::GenericNamePattern(std::string &&name, const yy::location &loc)
    : Pattern(loc)
    , name_{std::move(name)} {}

GenericNamePattern::~GenericNamePattern() = default;

void
GenericNamePattern::to_json(std::ostream &os) const {
    // TODO unimplemented
    (void)os;
}

const TypeChecker::Type &
GenericNamePattern::get_type(TypeChecker::Context &ctx) const {
    // TODO unimplemented
    return ctx.add_type(std::make_unique<TypeChecker::Error>(get_loc()));
}

} // namespace AST
