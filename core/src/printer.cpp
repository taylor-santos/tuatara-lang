//
// Created by taylor-santos on 2/12/2022 at 16:46.
//

#include "printer.hpp"

#include <iomanip>
#include <map>
#include <numeric>
#include <optional>
#include <sstream>

#if defined(_WIN32) || defined(_WIN64)
#    define NOMINMAX // Prevent collision with std::max
#    include <windows.h>
#endif

#include "rang.hpp"

namespace print {

std::ostream &
operator<<(std::ostream &os, color fg) {
    if (static_cast<int>(fg) < static_cast<int>(rang::fg::reset)) {
        os << static_cast<rang::fg>(fg);
    } else {
        os << static_cast<rang::fgB>(fg);
    }
    return os;
}

std::ostream &
operator<<(std::ostream &os, background bg) {
    if (static_cast<int>(bg) < static_cast<int>(rang::bg::reset)) {
        os << static_cast<rang::bg>(bg);
    } else {
        os << static_cast<rang::bgB>(bg);
    }
    return os;
}

std::ostream &
operator<<(std::ostream &os, style s) {
    os << static_cast<rang::style>(s);
    return os;
}

std::ostream &
operator<<(std::ostream &os, const colored_text &txt) {
    os << txt.col.s << txt.col.fg << txt.col.bg;
    os << txt.message;
    os << background::reset << color::reset << style::reset;
    return os;
}

template<typename T>
std::ostream &
operator<<(std::ostream &os, const std::vector<T> &v) {
    for (const auto &el : v) {
        os << el;
    }
    return os;
}

Message::Builder
Message::error(yy::position pos) {
    return Builder(pos).with_message("error: ", color::bold_red);
}

Message::Message(yy::position pos)
    : pos_{pos} {}

Message::Builder::Builder(yy::position pos)
    : msg_{pos} {}

Message::Builder::operator Message() const {
    return msg_;
}

Message::Builder &
Message::Builder::with_message(const std::string &text, TextColor color) {
    if (msg_.details_.empty()) {
        msg_.message_.emplace_back(text, color);
    } else {
        msg_.details_.back().message.emplace_back(text, color);
    }
    last_color_ = color;
    return *this;
}

Message::Builder &
Message::Builder::with_message(const std::string &text) {
    if (msg_.details_.empty()) {
        msg_.message_.emplace_back(text, last_color_);
    } else {
        msg_.details_.back().message.emplace_back(text, last_color_);
    }
    return *this;
}

Message::Builder &
Message::Builder::with_message(const colored_text &text) {
    if (msg_.details_.empty()) {
        msg_.message_.emplace_back(text);
    } else {
        msg_.details_.back().message.emplace_back(text);
    }
    last_color_ = text.col;
    return *this;
}

Message::Builder &
Message::Builder::with_detail(yy::location loc, TextColor color) {
    msg_.details_.push_back({loc, color, {{""}}});
    last_color_ = color;
    return *this;
}

struct Characters {
    const char *hbar;
    const char *vbar;
    const char *xbar;
    const char *vbar_break;

    const char *uarrow;
    const char *rarrow;

    const char *ltop;
    const char *mtop;
    const char *rtop;
    const char *lbot;
    const char *rbot;
    const char *mbot;

    const char *lbox;
    const char *rbox;

    const char *lcross;
    const char *rcross;

    const char *underbar;
    const char *underline;

    static const Characters unicode;
    static const Characters ascii;
};

const Characters Characters::unicode{
    .hbar       = "─",
    .vbar       = "│",
    .xbar       = "┼",
    .vbar_break = "·",
    .uarrow     = "▲",
    .rarrow     = "▶",
    .ltop       = "╭",
    .mtop       = "┬",
    .rtop       = "╮",
    .lbot       = "╰",
    .rbot       = "╯",
    .mbot       = "┴",
    .lbox       = "[",
    .rbox       = "]",
    .lcross     = "├",
    .rcross     = "┤",
    .underbar   = "┬",
    .underline  = "─",
};
const Characters Characters::ascii{
    .hbar       = "-",
    .vbar       = "|",
    .xbar       = "+",
    .vbar_break = ":",
    .uarrow     = "^",
    .rarrow     = ">",
    .ltop       = ",",
    .mtop       = "v",
    .rtop       = ".",
    .lbot       = "`",
    .rbot       = "\'",
    .mbot       = "^",
    .lbox       = "[",
    .rbox       = "]",
    .lcross     = "|",
    .rcross     = "|",
    .underbar   = "|",
    .underline  = "^",
};

static void
color_text(std::vector<colored_text> &text, int begin, int end, TextColor color) {
    if (end <= begin) return;
    auto begin_col = 1;
    auto begin_it  = std::find_if(text.begin(), text.end(), [&](const colored_text &txt) -> bool {
        begin_col += (int)txt.message.length();
        return begin_col > begin;
    });
    auto end_col   = 1;
    auto end_it    = std::find_if(text.begin(), text.end(), [&](const colored_text &txt) -> bool {
        auto len = (int)txt.message.length();
        if (end <= end_col + len) return true;
        end_col += len;
        return false;
    });
    auto result    = std::vector<colored_text>(text.begin(), begin_it);
    auto message   = std::string("");

    if (begin_it != text.end()) {
        auto len = (int)begin_it->message.length();
        begin_col -= len;
        if (begin_col < begin) {
            result.emplace_back(begin_it->message.substr(0, begin - begin_col), begin_it->col);
        }
        message = begin_it->message.substr(
            begin - begin_col,
            (end - begin > len) ? (std::string::npos) : (end - begin));
        if (begin_it != end_it) {
            for (auto it = begin_it + 1; it != end_it; it++) {
                message += it->message;
            }
            if (end_it != text.end()) {
                message += end_it->message.substr(0, end - end_col);
            }
        }
    }

    result.emplace_back(message, color);

    if (end_it != text.end()) {
        result.emplace_back(end_it->message.substr(end - end_col), end_it->col);
        end_it++;
    }

    result.insert(result.end(), end_it, text.end());
    text = result;
}

template<typename T>
static std::string
repeat(const T &val, size_t times) {
    std::stringstream ss;
    for (size_t i = 0; i < times; i++) {
        ss << val;
    }
    return ss.str();
}

class SS { // stringstream wrapper, implicitly cast-able to string
public:
    SS()
        : ss_() {}

    operator std::string() const {
        return ss_.str();
    }
    template<typename T>
    friend SS &&
    operator<<(SS &&, T);

private:
    std::stringstream ss_;
};

template<typename T>
SS &&
operator<<(SS &&ss, T t) {
    ss.ss_ << t;
    return std::move(ss);
}

struct Context {
    const std::vector<detail>      &details;
    const Characters               &chars;
    const std::vector<std::string> &source_lines;
    const yy::position             &pos;
};

static auto
generate_filename_line(yy::position pos, const Context &ctx) {
    auto &chars = ctx.chars;
    auto  line  = std::vector<colored_text>();
    line.emplace_back(chars.lbox, color::gray);
    line.emplace_back(
        SS() << (pos.filename ? (*pos.filename + ":") : "") << pos.line << ":" << pos.column,
        color::bold_gray);
    line.emplace_back(chars.rbox, color::gray);
    return line;
}

struct all_details {
    std::vector<const detail *> in;
    std::vector<const detail *> begin;
    std::vector<const detail *> end;

    std::vector<const detail *> multi;
};

static std::map<int, all_details>
get_line_details(const Context &ctx) {
    auto line_details = std::map<int, all_details>();

    auto &details      = ctx.details;
    auto &source_lines = ctx.source_lines;
    {
        auto single_details = std::vector<const detail *>();
        auto multi_details  = std::vector<const detail *>();
        for (auto &d : details) {
            auto begin = d.loc.begin.line;
            auto end   = d.loc.end.line;
            if (begin == end) {
                single_details.push_back(&d);
                line_details[begin].in.push_back(&d);
            } else if (d.loc.end.column == 1 && d.loc.end.line == d.loc.begin.line + 1) {
                single_details.push_back(&d);
                line_details[begin].in.push_back(&d);
            } else {
                multi_details.push_back(&d);
            }
        }

        std::sort(
            multi_details.begin(),
            multi_details.end(),
            [](const detail *a, const detail *b) -> bool {
                return a->loc.end.line > b->loc.end.line;
            });

        for (const auto *d : multi_details) {
            auto begin = d->loc.begin.line;
            auto end   = d->loc.end.line;
            line_details[begin].begin.push_back(d);
            line_details[end].end.push_back(d);
            for (auto i = begin + 1; i < end; i++) {
                if (source_lines[i - 1].find_last_not_of(" \t") != std::string::npos) {
                    line_details[i]; // Include lines between start and end of range
                }
            }
        }
    }
    return line_details;
}

static auto
generate_source_line(
    const all_details &all_ds,
    int                line_number,
    int                multis_width,
    const Context     &ctx) {
    auto &chars       = ctx.chars;
    auto &source_line = ctx.source_lines[line_number - 1];
    auto  line        = std::vector<colored_text>();

    auto &[in_ds, begin_ds, end_ds, multi_ds] = all_ds;
    {
        auto it = std::find_if(multi_ds.rbegin(), multi_ds.rend(), [&](const detail *d) -> bool {
            return d && d->loc.begin.line != line_number && d->loc.end.line != line_number;
        });
        if (it == multi_ds.rend()) {
            line.emplace_back(source_line, color::gray);
        } else {
            line.emplace_back(source_line, (*it)->color);
        }
    }

    for (auto *const detail : begin_ds) {
        color_text(line, detail->loc.begin.column, (int)source_line.length() + 1, detail->color);
    }
    for (auto *const detail : end_ds) {
        color_text(line, 1, detail->loc.end.column, detail->color);
    }
    for (auto *const detail : in_ds) {
        color_text(line, detail->loc.begin.column, detail->loc.end.column, detail->color);
    }
    const detail *multi_cross = nullptr;
    auto          multi_line  = std::vector<colored_text>();
    for (auto mult : multi_ds) {
        if (mult) {
            if (mult->loc.begin.line == line_number) {
                if (multi_cross) {
                    multi_line.emplace_back(chars.rarrow, multi_cross->color);
                    multi_line.emplace_back(chars.ltop, mult->color);
                    multi_cross = mult;
                } else {
                    multi_cross = mult;
                    multi_line.emplace_back(SS() << " " << chars.ltop, mult->color);
                }
            } else if (mult->loc.end.line == line_number) {
                if (multi_cross) {
                    multi_line.emplace_back(chars.rarrow, multi_cross->color);
                    multi_line.emplace_back(chars.lcross, mult->color);
                    multi_cross = mult;
                } else {
                    multi_line.emplace_back(SS() << " " << chars.lcross, mult->color);
                    multi_cross = mult;
                }
            } else {
                if (multi_cross) {
                    multi_line.emplace_back(SS() << chars.hbar << chars.hbar, multi_cross->color);
                } else {
                    multi_line.emplace_back(SS() << " " << chars.vbar, mult->color);
                }
            }
        } else {
            if (multi_cross) {
                multi_line.emplace_back(SS() << chars.hbar << chars.hbar, multi_cross->color);
            } else {
                multi_line.emplace_back("  ");
            }
        }
    }
    for (auto i = (int)multi_ds.size(); i < multis_width; i++) {
        if (multi_cross) {
            multi_line.emplace_back(SS() << chars.hbar << chars.hbar, multi_cross->color);
        } else {
            multi_line.emplace_back("  ");
        }
    }
    if (multi_cross) {
        multi_line.emplace_back(SS() << chars.rarrow << " ", multi_cross->color);
    } else if (multis_width) {
        multi_line.emplace_back("  ");
    }
    line.insert(line.begin(), multi_line.begin(), multi_line.end());
    return line;
}

static int
extract_multis(std::map<int, all_details> &line_details) {
    auto multis       = std::vector<const detail *>();
    auto multis_width = 0;
    for (auto &[line_number, all_ds] : line_details) {
        for (auto d : all_ds.begin) {
            auto it = std::find(multis.rbegin(), multis.rend(), nullptr);
            if (it != multis.rend()) {
                *it = d;
            } else {
                multis.push_back(d);
                multis_width = std::max(multis_width, (int)multis.size());
            }
        }
        all_ds.multi = multis;
        for (auto &d : multis) {
            if (d && d->loc.end.line == line_number) {
                d = nullptr;
            }
        }
    }
    return multis_width;
}

static auto &
sort_all_details(all_details &all_ds) {
    auto &[in_ds, begin_ds, end_ds, multi_ds] = all_ds;
    std::sort(in_ds.begin(), in_ds.end(), [&](const detail *a, const detail *b) {
        return a->loc.begin.column < b->loc.begin.column;
    });
    std::sort(begin_ds.begin(), begin_ds.end(), [&](const detail *a, const detail *b) {
        return a->loc.begin.column < b->loc.begin.column;
    });
    std::sort(end_ds.begin(), end_ds.end(), [&](const detail *a, const detail *b) {
        return b->loc.end.column < a->loc.end.column;
    });
    return all_ds;
}

static std::optional<std::vector<colored_text>>
generate_multi_row_line(
    int                          line_number,
    std::vector<const detail *> &details,
    int                          multi_width,
    const Context               &ctx) {
    auto it = std::find_if(details.rbegin(), details.rend(), [&](const detail *d) {
        return d && d->loc.end.line == line_number;
    });
    if (it == details.rend()) {
        return std::nullopt;
    }
    const auto &chars      = ctx.chars;
    auto        line       = std::vector<colored_text>();
    auto       *mult_cross = (const detail *)nullptr;
    for (auto mult : details) {
        if (mult) {
            if (mult == *it) {
                mult_cross = mult;
                line.emplace_back(SS() << " " << chars.lbot, mult->color);
            } else {
                if (mult_cross) {
                    line.emplace_back(SS() << chars.hbar << chars.hbar, mult_cross->color);
                } else {
                    line.emplace_back(SS() << " " << chars.vbar, mult->color);
                }
            }

        } else {
            if (mult_cross) {
                line.emplace_back(SS() << chars.hbar << chars.hbar, mult_cross->color);
            } else {
                line.emplace_back("  ");
            }
        }
    }
    *it = nullptr;

    for (int i = (int)details.size(); i <= multi_width; i++) {
        line.emplace_back(SS() << chars.hbar << chars.hbar, mult_cross->color);
    }

    line.emplace_back(repeat(chars.hbar, mult_cross->loc.end.column - 1), mult_cross->color);

    line.emplace_back(SS() << " ", mult_cross->color);
    line.insert(line.end(), mult_cross->message.begin(), mult_cross->message.end());

    return line;
}

static std::vector<colored_text>
generate_under_line(std::queue<std::pair<const detail *, bool>> &overlaps, const Context &ctx) {
    auto &chars = ctx.chars;

    auto line = std::vector<colored_text>{};
    auto col  = 1;
    auto n    = overlaps.size();
    for (size_t i = 0; i < n; i++) {
        auto [detail, needs_underline] = overlaps.front();
        overlaps.pop();
        auto begin     = detail->loc.begin.column;
        auto end       = detail->loc.end.column;
        auto underline = col < begin ? std::string(begin - col, ' ') : "";
        col            = begin;
        if (i == n - 1 || end < overlaps.front().first->loc.begin.column) {
            auto width = end - begin;
            if (i < n - 1) {
                auto gap         = overlaps.front().first->loc.begin.column - end;
                auto message_len = std::accumulate(
                    detail->message.begin(),
                    detail->message.end(),
                    0,
                    [](int acc, const colored_text &text) -> int {
                        return acc + (int)text.message.length();
                    });
                bool fits_gap = message_len + 2 <= gap;
                if (needs_underline) {
                    if (width > 0) {
                        underline += repeat(chars.hbar, width);
                        col += width;
                        if (!fits_gap) {
                            underline += chars.rtop;
                            col++;
                        }
                    } else {
                        underline += chars.uarrow;
                        col++;
                    }
                } else if (!fits_gap) {
                    underline += std::string(width, ' ');
                    underline += chars.vbar;
                    col += width + 1;
                }
                if (fits_gap) {
                    underline += " ";
                    line.emplace_back(underline, detail->color);
                    line.insert(line.end(), detail->message.begin(), detail->message.end());
                    col += 1 + (int)message_len;
                    continue; // TODO: refactor this
                } else {
                    overlaps.emplace(detail, false);
                }
            } else {
                if (needs_underline) {
                    if (width > 0) {
                        underline += repeat(chars.hbar, width);
                    } else {
                        underline += chars.uarrow;
                    }
                } else {
                    underline += std::string(width, ' ');
                    underline += chars.lbot;
                    underline += chars.hbar;
                }
                underline += " ";
                line.emplace_back(underline, detail->color);
                line.insert(line.end(), detail->message.begin(), detail->message.end());
                continue; // TODO: refactor this
            }
        } else {
            overlaps.emplace(detail, true);
        }
        line.emplace_back(underline, detail->color);
    }
    return line;
}

static std::vector<std::vector<colored_text>>
generate_output(
    yy::position                     pos,
    const std::vector<colored_text> &message,
    const std::vector<detail>       &details,
    const Characters                &chars,
    const std::vector<std::string>  &source_lines) {
    auto ctx    = Context{details, chars, source_lines, pos};
    auto output = std::vector<std::vector<colored_text>>{message};

    auto filename = generate_filename_line(pos, ctx);
    if (details.empty()) {
        output.emplace_back(filename);
        return output;
    }

    auto line_details    = get_line_details(ctx);
    auto multis_width    = extract_multis(line_details);
    auto last_line_width = (int)std::to_string(line_details.rbegin()->first).length();

    filename.emplace(
        filename.begin(),
        SS() << std::setw(last_line_width + 1) << std::right << " " << chars.ltop << chars.hbar,
        color::gray);
    output.emplace_back(filename);

    auto prev_line = 0;

    for (auto &[line_number, all_ds] : line_details) {
        if (prev_line) {
            // If there is exactly one non-empty source line between two detailed lines, print it
            // anyways. Otherwise, print "..." to indicate multiple lines are being skipped.
            auto single_line = 0;
            for (auto i = prev_line + 1; i < line_number; i++) {
                auto &line = source_lines[i - 1];
                if (line.find_last_not_of(" \t") != std::string::npos) {
                    if (single_line) {
                        // Multiple intermediate lines have been found, just print "..."
                        auto text = std::vector<colored_text>();
                        text.emplace_back(
                            SS() << chars.vbar_break << chars.vbar_break << chars.vbar_break,
                            color::gray);
                        output.push_back(std::move(text));
                        single_line = 0;
                        break;
                    }
                    single_line = i;
                }
            }
            if (single_line) {
                auto text = std::vector<colored_text>();
                text.emplace_back(
                    SS() << std::setw(last_line_width) << std::right << single_line << " "
                         << chars.vbar << " ",
                    color::gray);
                text.emplace_back(source_lines[single_line - 1], color::gray);
                output.push_back(std::move(text));
            }
        }
        prev_line = line_number;

        auto &[in_ds, begin_ds, end_ds, multi_ds] = sort_all_details(all_ds);

        {
            auto line = generate_source_line(all_ds, line_number, multis_width, ctx);
            line.emplace(
                line.begin(),
                SS() << std::setw(last_line_width) << std::right << line_number << " " << chars.vbar
                     << " ",
                color::gray);
            output.emplace_back(line);
        }

        if (!in_ds.empty()) {
            auto overlaps =
                std::queue<std::pair<const detail *, bool>>(); // (detail, needs_underline)
            for (auto *const d : in_ds) {
                overlaps.emplace(d, true);
            }
            while (!overlaps.empty()) {
                auto line       = generate_under_line(overlaps, ctx);
                auto multi_line = std::vector<colored_text>();
                for (auto mult : multi_ds) {
                    if (mult) {
                        multi_line.emplace_back(SS() << " " << chars.vbar, mult->color);
                    } else {
                        multi_line.emplace_back("  ");
                    }
                }
                for (auto i = (int)multi_ds.size(); i < multis_width; i++) {
                    multi_line.emplace_back("  ");
                }
                if (multis_width) {
                    multi_line.emplace_back("  ");
                }
                line.insert(line.begin(), multi_line.begin(), multi_line.end());
                line.emplace(
                    line.begin(),
                    SS() << std::string(last_line_width + 1, ' ') << chars.vbar_break << " ",
                    color::gray);
                output.push_back(line);
            }
        }
        while (true) {
            auto line = generate_multi_row_line(line_number, multi_ds, multis_width, ctx);
            if (!line) break;
            line->emplace(
                line->begin(),
                SS() << std::string(last_line_width + 1, ' ') << chars.vbar_break << " ",
                color::gray);
            output.push_back(*line);
        }
    }

    /*
    auto last_line = line_details.rbegin()->first;
    for (auto line_number = last_line; line_number < (int)source_lines.size(); line_number++) {
        auto &line = source_lines[line_number];
        if (line.find_last_not_of(" \t") != std::string::npos) {
            auto text = std::vector<colored_text>();
            text.emplace_back(
                SS() << std::setw(last_line_width) << std::right << (line_number + 1) << " "
                     << chars.vbar << " ",
                color::gray);
            text.emplace_back(line, color::gray);
            output.push_back(std::move(text));
            break;
        }
    }
     */

    output.push_back(
        {{SS() << repeat(chars.hbar, last_line_width + 1) << chars.rbot, color::gray}});

    return output;
}

void
Message::print(const std::vector<std::string> &source_lines, std::ostream &out) const {
#if defined(_WIN32) || defined(_WIN64)
    // Enable proper UTF-8 printing on Windows
    auto prev_cp = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
#endif
    auto chars  = Characters::unicode;
    auto output = generate_output(pos_, message_, details_, chars, source_lines);
    for (auto &line : output) {
        out << line << std::endl;
    }

#if defined(_WIN32) || defined(_WIN64)
    SetConsoleOutputCP(prev_cp);
#endif
}

} // namespace print
