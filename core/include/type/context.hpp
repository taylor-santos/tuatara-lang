//
// Created by taylor-santos on 2/7/2022 at 19:33.
//

#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>

#include "type/class.hpp"

namespace TypeChecker {

class Type;
class Class;

class Context {
public:
    Context();
    ~Context();

    const Type &
    add_type(std::unique_ptr<Type> type);

    [[nodiscard]] std::optional<std::reference_wrapper<const Type>>
    get_symbol(const std::string &name) const;

    [[nodiscard]] bool
    is_initialized(const std::string &symbol) const;

    void
    set_symbol(const std::string &name, const Type &type, bool initialized = true);

    [[nodiscard]] std::optional<std::reference_wrapper<const Class>>
    get_class(const std::string &name) const;

    void
    print_symbols(std::ostream &out) const;

    void
    set_failure(bool failure);

    [[nodiscard]] bool
    did_fail() const;

    static struct Builtins { const TypeChecker::Class U64; } builtins;

private:
    struct symbol {
        const Type &type;
        bool        initialized;
    };
    bool                                           did_fail_ = false;
    std::vector<std::unique_ptr<Type>>             types_;
    std::unordered_map<std::string, const Class *> classes_;
    std::unordered_map<std::string, symbol>        symbols_;
};

} // namespace TypeChecker
