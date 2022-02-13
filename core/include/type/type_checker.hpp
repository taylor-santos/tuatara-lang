//
// Created by taylor-santos on 2/7/2022 at 19:33.
//

#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>

#include "type/class.hpp"
#include "ast/ast.hpp"

namespace TypeChecker {

class Type;
class Class;

struct Uninit {
    enum class Reason { NOT_DEFINED, MOVED_FROM } reason;
    const AST::Node &node;
};

class Context {
public:
    explicit Context(std::ostream &out);
    ~Context();

    const Type &
    add_type(std::unique_ptr<Type> type);

    [[nodiscard]] std::optional<std::reference_wrapper<const Type>>
    get_symbol(const std::string &name) const;

    [[nodiscard]] std::optional<Uninit>
    is_uninitialized(const std::string &symbol) const;

    void
    set_symbol(const std::string &name, const Type &type);

    void
    set_symbol(const std::string &name, const Type &type, std::optional<Uninit> uninit_reason);

    [[nodiscard]] std::optional<std::reference_wrapper<const Class>>
    get_class(const std::string &name) const;

    void
    print_symbols(std::ostream &out) const;

    void
    set_failure(bool failure);

    [[nodiscard]] bool
    did_fail() const;

    void
    report_error(yy::location loc, const std::string &message) const;

    static struct Builtins { const TypeChecker::Class U64; } builtins;

private:
    struct symbol {
        const Type           &type;
        std::optional<Uninit> uninit_reason;
    };
    std::ostream                                  &out_;
    bool                                           did_fail_ = false;
    std::vector<std::unique_ptr<Type>>             types_;
    std::unordered_map<std::string, const Class *> classes_;
    std::unordered_map<std::string, symbol>        symbols_;
};

} // namespace TypeChecker
