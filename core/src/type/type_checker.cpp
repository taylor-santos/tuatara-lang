//
// Created by taylor-santos on 2/7/2022 at 19:35.
//

#include "type/type_checker.hpp"
#include "type/class.hpp"
#include "location.hh"
#include "printer.hpp"

#include <ostream>
#include <iomanip>
#include <algorithm>

namespace TypeChecker {

Context::Builtins Context::builtins = {.U64 = TypeChecker::Class("U64")};

Context::Context(std::vector<print::Message> &errors)
    : errors_{errors} {
    classes_["U64"] = &builtins.U64;
}

Context::Context(Context &&other) = default;

Context::~Context() = default;

const Type &
Context::add_type(std::unique_ptr<Type> type) {
    types_.emplace_back(std::move(type));
    return *types_.back();
}

const Type *
Context::get_symbol(const std::string &name) const {
    auto it = symbols_.find(name);
    return (it == symbols_.end()) ? nullptr : &it->second.type;
}

std::optional<Uninit>
Context::is_uninitialized(const std::string &symbol) const {
    auto it = symbols_.find(symbol);
    return (it == symbols_.end()) ? std::nullopt : std::optional{it->second.uninit_reason};
}

std::optional<yy::location>
Context::get_symbol_loc(const std::string &symbol) const {
    auto it = symbols_.find(symbol);
    return (it == symbols_.end()) ? std::nullopt : std::optional{it->second.init_loc};
}

void
Context::set_symbol(const std::string &name, const Type &type, yy::location init_loc) {
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        // Keep the original symbol init location
        // TODO: This might interfere with nested scopes?
        //       Keeping the old location should be more explicit
        init_loc = it->second.init_loc;
        symbols_.erase(name);
    }
    symbols_.emplace(name, Context::symbol{type, {}, init_loc});
}

void
Context::set_symbol(
    const std::string    &name,
    const Type           &type,
    yy::location          init_loc,
    std::optional<Uninit> uninit_reason) {
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        // Keep the original symbol init location
        // TODO: This might interfere with nested scopes?
        //       Keeping the old location should be more explicit
        init_loc = it->second.init_loc;
        symbols_.erase(name);
    }
    symbols_.emplace(name, Context::symbol{type, uninit_reason, init_loc});
}

std::optional<std::reference_wrapper<const Class>>
Context::get_class(const std::string &name) const {
    auto it = classes_.find(name);
    if (it == classes_.end()) {
        return {};
    }
    return *it->second;
}

void
Context::print_symbols(std::ostream &out) const {
    auto name_label = std::string("Symbol");
    auto width      = symbols_.empty()
                          ? name_label.size()
                          : std::max(
                           std::max_element(
                               symbols_.begin(),
                               symbols_.end(),
                               [](auto &a, auto &b) { return a.first.size() < b.first.size(); })
                               ->first.size(),
                           name_label.size());
    ;
    out << std::setw(width) << std::left << name_label << " Type" << std::endl;
    for (auto &[name, symbol] : symbols_) {
        out << std::setw(width) << std::left << name << " " << symbol.type;
        if (symbol.uninit_reason) {
            out << " (uninitialized)";
        }
        out << std::endl;
    }
}

// TODO: Make this implicit in the error reporting procedure, so that it cannot be missed.
//       It shouldn't be up to the caller to decide whether or not a type error is considered a
//       failure, unless somehow there could be a non-fatal type error?
void
Context::set_failure(bool failure) {
    did_fail_ = failure;
}

bool
Context::did_fail() const {
    return did_fail_;
}

void
Context::add_message(const print::Message &message) const {
    errors_.push_back(message);
}

} // namespace TypeChecker
