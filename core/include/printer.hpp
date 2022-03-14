//
// Created by taylor-santos on 2/12/2022 at 16:46.
//

#pragma once

#include <utility>
#include <vector>
#include <variant>
#include <queue>

#include "location.hh"

namespace print {

enum class color {
    black        = 30,
    red          = 31,
    green        = 32,
    yellow       = 33,
    blue         = 34,
    magenta      = 35,
    cyan         = 36,
    gray         = 37,
    reset        = 39,
    bold_black   = 90,
    bold_red     = 91,
    bold_green   = 92,
    bold_yellow  = 93,
    bold_blue    = 94,
    bold_magenta = 95,
    bold_cyan    = 96,
    bold_gray    = 97,
};

enum class background {
    black        = 40,
    red          = 41,
    green        = 42,
    yellow       = 43,
    blue         = 44,
    magenta      = 45,
    cyan         = 46,
    gray         = 47,
    reset        = 49,
    bold_black   = 100,
    bold_red     = 101,
    bold_green   = 102,
    bold_yellow  = 103,
    bold_blue    = 104,
    bold_magenta = 105,
    bold_cyan    = 106,
    bold_gray    = 107
};

enum class style {
    reset     = 0,
    bold      = 1,
    dim       = 2,
    italic    = 3,
    underline = 4,
    blink     = 5,
    rblink    = 6,
    reversed  = 7,
    conceal   = 8,
    crossed   = 9
};

struct TextColor {
    color      fg = color::reset;
    style      s  = style::reset;
    background bg = background::reset;

    TextColor() = default;
    /* implicit */ TextColor(color col)
        : fg{col} {}
    TextColor(color col, background bg)
        : fg{col}
        , bg{bg} {}
    TextColor(color col, style style)
        : fg{col}
        , s{style} {}
    TextColor(color col, style style, background bg)
        : fg{col}
        , s{style}
        , bg{bg} {}
};

struct colored_text {
    std::string message;
    TextColor   col = {};

    colored_text() = default;
    colored_text(std::string msg)
        : message{std::move(msg)} {}
    colored_text(const char *msg)
        : message(msg) {}
    colored_text(std::string msg, TextColor col)
        : message{std::move(msg)}
        , col{col} {}
};

std::ostream &
operator<<(std::ostream &os, const colored_text &txt);

struct detail {
    yy::location              loc;
    TextColor                 color;
    std::vector<colored_text> message;

    inline bool
    operator<(const detail &other) const {
        return (loc.begin.line < other.loc.begin.line) ||
               (loc.begin.line == other.loc.begin.line &&
                loc.begin.column < other.loc.begin.column);
    }
};

class Message {
public:
    class Builder;
    friend class Builder;

    void
    print(const std::vector<std::string> &source_lines, std::ostream &out = std::cerr) const;

    static Builder
    error(yy::position pos);

private:
    Message(yy::position pos);

    yy::position              pos_;
    std::vector<colored_text> message_;
    std::vector<detail>       details_;
};

class Message::Builder {
public:
    explicit Builder(yy::position pos);

    /* implicit */ operator Message() const;

    Builder &
    with_message(const std::string &text, TextColor color);

    Builder &
    with_message(const std::string &text);

    Builder &
    with_message(const colored_text &text);

    Builder &
    with_detail(yy::location loc, TextColor color);

private:
    Message   msg_;
    TextColor last_color_ = color::bold_gray;
};

} // namespace print
