//
// Created by taylor-santos on 7/14/2022 at 12:22.
//

#include "ast/block.hpp"
#include "json.hpp"
#include "type/type_checker.hpp"
#include "type/tuple.hpp"
#include "type/wrapper.hpp"
#include "type/error.hpp"

#include <ostream>

namespace AST {

Block::Block(
    std::vector<std::unique_ptr<AST::Expression>> lines,
    std::unique_ptr<AST::Expression>              last_line,
    const yy::location                           &loc)
    : Node(loc)
    , lines_{std::move(lines)}
    , last_line_{std::move(last_line)} {}

Block::Block(std::unique_ptr<AST::Expression> last_line, const yy::location &loc)
    : Node(loc)
    , lines_{}
    , last_line_{std::move(last_line)} {}

Block::Block(
    std::vector<std::unique_ptr<AST::Expression>> lines,
    const yy::location                           &val_loc,
    const yy::location                           &loc)
    : Node(loc)
    , lines_{std::move(lines)}
    , last_line_{val_loc} {}

Block::Block(const yy::location &val_loc, const yy::location &loc)
    : Node(loc)
    , lines_{}
    , last_line_{val_loc} {}

Block::~Block() = default;

// helper constant for std::visit
template<class>
inline constexpr bool always_false_v = false;

void
Block::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":"block",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("lines":[)";
    std::string sep;
    for (auto &line : lines_) {
        os << sep;
        sep = ",";
        line->to_json(os);
    }
    os << "],";
    std::visit(
        [&os](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::unique_ptr<AST::Expression>>) {
                os << R"("last line":)";
                arg->to_json(os);
            } else if constexpr (std::is_same_v<T, yy::location>) {
                os << R"("value loc":)";
                loc_to_json(arg, os);
            } else {
                static_assert(always_false_v<T>, "non-exhaustive visitor!");
            }
        },
        last_line_);
    os << "}";
}

const TypeChecker::Type &
Block::get_type(TypeChecker::Context &ctx) const {
    for (auto &line : lines_) {
        (void)line->get_type(ctx);
    }
    return std::visit(
        [&](auto &&arg) -> const TypeChecker::Type & {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::unique_ptr<AST::Expression>>) {
                return ctx.add_type(
                    std::make_unique<TypeChecker::Wrapper>(arg->get_type(ctx), arg->get_loc()));
            } else if constexpr (std::is_same_v<T, yy::location>) {
                // 0-tuple (Unit type)
                return ctx.add_type(std::make_unique<TypeChecker::Tuple>(arg));
            } else {
                static_assert(always_false_v<T>, "non-exhaustive visitor!");
            }
        },
        last_line_);
}

} // namespace AST
