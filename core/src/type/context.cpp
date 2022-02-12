//
// Created by taylor-santos on 2/7/2022 at 19:35.
//

#include "type/context.hpp"
#include "type/class.hpp"

#include <ostream>
#include <iomanip>
#include <algorithm>

namespace TypeChecker {

Context::Builtins Context::builtins = {.U64 = TypeChecker::Class("U64")};

Context::Context() {
    classes_["U64"] = &builtins.U64;
}

Context::~Context() = default;

const Type &
Context::add_type(std::unique_ptr<Type> type) {
    types_.emplace_back(std::move(type));
    return *types_.back();
}

std::optional<std::reference_wrapper<const Type>>
Context::get_symbol(const std::string &name) const {
    auto it = symbols_.find(name);
    if (it == symbols_.end()) {
        return {};
    }
    return it->second.type;
}

bool
Context::is_initialized(const std::string &symbol) const {
    auto it = symbols_.find(symbol);
    return !(it == symbols_.end()) && it->second.initialized;
}

void
Context::set_symbol(const std::string &name, const Type &type, bool initialized) {
    symbols_.emplace(name, Context::symbol{type, initialized});
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
    auto width      = std::max(
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
        if (!symbol.initialized) {
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

} // namespace TypeChecker
