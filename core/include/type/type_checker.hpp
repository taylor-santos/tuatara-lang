//
// Created by taylor-santos on 2/7/2022 at 19:33.
//

#pragma once

#include <map>
#include <vector>
#include <memory>
#include <optional>

#include "ast/ast.hpp"
#include "type/class.hpp"

namespace print {
class Message;
}

namespace TypeChecker {

struct Uninit {
    enum class Reason { NOT_DEFINED, MOVED_FROM } reason;
    yy::location loc;
};

class Context {
public:
    explicit Context(std::vector<print::Message> &errors);

    Context(const Context &other);

    Context(Context &&other) noexcept;

    ~Context();

    const Type &
    add_type(std::unique_ptr<Type> type);

    [[nodiscard]] const Type *
    get_symbol(const std::string &name) const;

    [[nodiscard]] std::optional<Uninit>
    is_uninitialized(const std::string &symbol) const;

    [[nodiscard]] std::optional<yy::location>
    get_symbol_loc(const std::string &symbol) const;

    void
    set_symbol(const std::string &name, const Type &type, yy::location loc);

    void
    set_symbol(
        const std::string    &name,
        const Type           &type,
        yy::location          loc,
        std::optional<Uninit> uninit_reason);

    [[nodiscard]] std::optional<std::reference_wrapper<const Class>>
    get_class(const std::string &name) const;

    void
    print_symbols(std::ostream &out) const;

    void
    set_failure(bool failure);

    [[nodiscard]] bool
    did_fail() const;

    void
    add_message(const print::Message &message) const;

    static struct Builtins {
        const TypeChecker::Class &U64;
        const TypeChecker::Class &Dummy;
    } builtins;

private:
    struct symbol {
        const Type           &type;
        std::optional<Uninit> uninit_reason;
        yy::location          init_loc;
    };
    std::vector<print::Message>         &errors_;
    bool                                 did_fail_ = false;
    std::vector<std::unique_ptr<Type>>   types_;
    std::map<std::string, const Class *> classes_;
    std::map<std::string, symbol>        symbols_;
};

} // namespace TypeChecker
