//
// Created by taylor-santos on 2/12/2022 at 16:46.
//

#include "printer.hpp"

#include <algorithm>
#include <iomanip>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <cassert>
#include <queue>

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

template<typename T>
std::ostream &
operator<<(std::ostream &os, const std::list<T> &v) {
    for (const auto &el : v) {
        os << el;
    }
    return os;
}

Message::Builder
Message::error(yy::position pos) {
    return Builder(pos).with_message("error: ").in(color::bold_red);
}

Message::Message(yy::position pos)
    : pos_{pos} {}

Message::Builder::Builder(yy::position pos)
    : msg_{pos} {}

Message::Builder::operator Message() const {
    return msg_;
}

Message::Builder &
Message::Builder::with_message(const std::string &text) {
    if (msg_.details_.empty()) {
        msg_.message_.push_back(colored_text{text});
        last_text = &msg_.message_.back();
    } else {
        msg_.details_.back().message.message = text;
        last_text                            = &msg_.details_.back().message;
    }
    return *this;
}

Message::Builder &
Message::Builder::with_detail_at(yy::location loc) {
    msg_.details_.push_back(detail{loc});
    return *this;
}

Message::Builder &
Message::Builder::in(TextColor color) {
    if (!last_text) {
        throw std::logic_error("No text has been added to get colored");
    }
    last_text->col = color;
    return *this;
}

Message::Builder &
Message::Builder::in(color color) {
    if (!last_text) {
        throw std::logic_error("No text has been added to get colored");
    }
    last_text->col = TextColor{color};
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

// static std::map<int, std::vector<detail>>
// split_details_by_line(const std::vector<detail> &details) {
//     auto lines = std::map<int, std::vector<detail>>();
//     // Gather details by line number
//     std::for_each(details.begin(), details.end(), [&lines](const detail &d) {
//         lines[d.loc.begin.line].push_back(d);
//         if (d.loc.end.line != d.loc.begin.line) {
//             lines[d.loc.end.line].push_back(d);
//         }
//     });
//     // Sort each line by column
//     std::for_each(lines.begin(), lines.end(), [](auto &line) {
//         auto &[l, d] = line;
//         std::sort(d.begin(), d.end(), [](const detail &a, const detail &b) {
//             return a.loc.begin.column < b.loc.begin.column;
//         });
//     });
//     return lines;
// }
//
// static std::string
// clean_string(std::string str) {
//     std::replace(str.begin(), str.end(), '\t', ' ');
//     return str;
// }
//
// static void
// print_column(std::ostream &out, size_t width, const std::string &str) {
//     out << rang::style::bold << std::setw(width) << std::right << str;
// }

static void
color_text(std::vector<colored_text> &text, int begin, int end, TextColor color) {
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
    }

    if (begin_it != end_it) {
        for (auto it = begin_it + 1; it != end_it; it++) {
            message += it->message;
        }
        if (end_it != text.end()) {
            message += end_it->message.substr(0, end - end_col);
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

void
Message::print(const std::vector<std::string> &source_lines, std::ostream &out) const {
#if defined(_WIN32) || defined(_WIN64)
    // Enable proper UTF-8 printing on Windows
    auto prev_cp = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);

    rang::setWinTermMode(rang::winTerm::Auto);
#endif
    rang::setControlMode(rang::control::Auto);

    auto chars = Characters::unicode;

    auto output = std::vector<std::vector<colored_text>>{message_};

    if (details_.empty()) {
        auto line = std::vector<colored_text>{{std::string(chars.hbar) + chars.lbox, color::gray}};
        auto ss   = std::stringstream();
        if (pos_.filename) {
            ss << *pos_.filename << ":";
        }
        ss << pos_.line << ":" << pos_.column;
        line.emplace_back(ss.str(), color::bold_gray);
        line.emplace_back(chars.rbox, color::gray);
        output.emplace_back(std::move(line));
    } else {
        struct all_details {
            std::vector<const detail *> in;
            std::vector<const detail *> begin;
            std::vector<const detail *> end;

            std::vector<const detail *> multi;
        };
        auto line_details = std::map<int, all_details>();

        for (auto &d : details_) {
            auto begin = d.loc.begin.line;
            auto end   = d.loc.end.line;
            if (begin == end) {
                line_details[begin].in.push_back(&d);
            } else {
                line_details[begin].begin.push_back(&d);
                line_details[end].end.push_back(&d);
                for (auto i = begin + 1; i < end; i++) {
                    if (source_lines[i - 1].find_last_not_of(" \t") != std::string::npos) {
                        line_details[i];
                    }
                }
            }
        }

        auto multi_details = std::map<int, std::vector<const detail *>>();
        auto multis        = std::vector<const detail *>();
        auto multis_width  = 0;
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

        auto last_line_width = (int)std::to_string(line_details.rbegin()->first).length();

        {
            auto line = std::vector<colored_text>();

            {
                auto ss = std::stringstream();
                ss << std::setw(last_line_width + 1) << std::right << " " << chars.ltop
                   << chars.hbar << chars.lbox;
                line.emplace_back(ss.str(), color::gray);
            }

            {
                auto ss = std::stringstream();
                if (pos_.filename) {
                    ss << *pos_.filename << ":";
                }
                ss << pos_.line << ":" << pos_.column;
                line.emplace_back(ss.str(), color::bold_gray);
            }
            line.emplace_back(chars.rbox, color::gray);
            output.emplace_back(std::move(line));
        }
        int last_line = 0;
        for (auto &[line_number, all_ds] : line_details) {
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

            {
                auto source_line = source_lines[line_number - 1];
                auto line        = std::vector<colored_text>();
                {
                    auto it = std::find_if(
                        multi_ds.rbegin(),
                        multi_ds.rend(),
                        [&](const detail *d) -> bool {
                            return d && d->loc.begin.line != line_number &&
                                   d->loc.end.line != line_number;
                        });
                    if (it == multi_ds.rend()) {
                        line.emplace_back(source_line, color::gray);
                    } else {
                        line.emplace_back(source_line, (*it)->message.col);
                    }
                }

                for (auto &detail : begin_ds) {
                    color_text(
                        line,
                        detail->loc.begin.column,
                        (int)source_line.length() + 1,
                        detail->message.col);
                }
                for (auto &detail : end_ds) {
                    color_text(line, 1, detail->loc.end.column, detail->message.col);
                }
                for (auto &detail : in_ds) {
                    color_text(
                        line,
                        detail->loc.begin.column,
                        detail->loc.end.column,
                        detail->message.col);
                }
                const detail *multi_cross = nullptr;
                auto          multi_line  = std::vector<colored_text>();
                for (auto mult : multi_ds) {
                    if (mult) {
                        if (mult->loc.begin.line == line_number) {
                            if (multi_cross) {
                                multi_line.emplace_back(chars.rarrow, multi_cross->message.col);
                                multi_line.emplace_back(chars.ltop, mult->message.col);
                                multi_cross = mult;
                            } else {
                                multi_cross = mult;

                                multi_line.emplace_back(
                                    (std::stringstream() << " " << chars.ltop).str(),
                                    mult->message.col);
                            }
                        } else if (mult->loc.end.line == line_number) {
                            if (multi_cross) {
                                multi_line.emplace_back(chars.rarrow, multi_cross->message.col);
                                multi_line.emplace_back(chars.lcross, mult->message.col);
                                multi_cross = mult;
                            } else {
                                multi_line.emplace_back(
                                    (std::stringstream() << " " << chars.lcross).str(),
                                    mult->message.col);
                                multi_cross = mult;
                            }
                        } else {
                            multi_line.emplace_back(
                                (std::stringstream() << " " << chars.vbar).str(),
                                mult->message.col);
                        }
                    } else {
                        if (multi_cross) {
                            multi_line.emplace_back(
                                (std::stringstream() << chars.hbar << chars.hbar).str(),
                                multi_cross->message.col);
                        } else {
                            multi_line.emplace_back("  ");
                        }
                    }
                }
                for (size_t i = multi_ds.size(); i < multis_width; i++) {
                    if (multi_cross) {
                        multi_line.emplace_back(
                            (std::stringstream() << chars.hbar << chars.hbar).str(),
                            multi_cross->message.col);
                    } else {
                        multi_line.emplace_back("  ");
                    }
                }
                if (multi_cross) {
                    multi_line.emplace_back(
                        (std::stringstream() << chars.rarrow << " ").str(),
                        multi_cross->message.col);
                } else if (multis_width) {
                    multi_line.emplace_back("  ");
                }
                line.insert(line.begin(), multi_line.begin(), multi_line.end());

                auto ss = std::stringstream();
                ss << std::setw(last_line_width) << std::right << line_number << " " << chars.vbar
                   << " ";
                line.emplace(line.begin(), ss.str(), color::gray);
                output.push_back(line);
            }
            if (!in_ds.empty()) {
                auto overlaps =
                    std::queue<std::pair<const detail *, bool>>(); // (detail, needs_underline)
                for (auto &d : in_ds) {
                    overlaps.emplace(d, true);
                }
                bool first_line = true;
                while (!overlaps.empty()) {
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
                                auto gap      = overlaps.front().first->loc.begin.column - end;
                                bool fits_gap = (int)detail->message.message.length() + 2 <= gap;
                                if (needs_underline) {
                                    underline += first_line ? chars.hbar : "└";
                                    for (int j = 1; j < width - 1; j++) {
                                        underline += chars.hbar;
                                    }
                                    if (width > 1)
                                        underline += first_line ? chars.hbar : chars.mbot;
                                    col += width;
                                    if (!fits_gap) {
                                        underline += chars.rtop;
                                        col++;
                                    }
                                } else if (!fits_gap) {
                                    underline += std::string(width, ' ');
                                    underline += chars.vbar;
                                    col += width + 1;
                                }
                                if (fits_gap) {
                                    underline += " " + detail->message.message;
                                    col += 1 + (int)detail->message.message.length();
                                } else {
                                    overlaps.emplace(detail, false);
                                }
                            } else {
                                if (needs_underline) {
                                    underline += first_line ? chars.hbar : "└";
                                    for (int j = 1; j < width - 1; j++) {
                                        underline += chars.hbar;
                                    }
                                    if (width > 1)
                                        underline += first_line ? chars.hbar : chars.mbot;
                                } else {
                                    underline += std::string(width, ' ');
                                    underline += chars.lbot;
                                    underline += chars.hbar;
                                }
                                underline += " " + detail->message.message;
                            }
                        } else {
                            overlaps.emplace(detail, true);
                        }
                        line.emplace_back(underline, detail->message.col);
                    }

                    auto multi_line = std::vector<colored_text>();
                    for (auto mult : multi_ds) {
                        if (mult) {
                            multi_line.emplace_back(
                                (std::stringstream() << " " << chars.vbar).str(),
                                mult->message.col);

                        } else {
                            multi_line.emplace_back("  ");
                        }
                    }
                    for (size_t i = multi_ds.size(); i < multis_width; i++) {
                        multi_line.emplace_back("  ");
                    }
                    if (multis_width) {
                        multi_line.emplace_back("  ");
                    }
                    line.insert(line.begin(), multi_line.begin(), multi_line.end());

                    auto ss = std::stringstream();
                    ss << std::string(last_line_width + 1, ' ') << chars.vbar_break << " ";
                    line.emplace(line.begin(), ss.str(), color::gray);
                    output.push_back(line);
                }
            }
            while (true) {
                auto it = std::find_if(multi_ds.rbegin(), multi_ds.rend(), [&](const detail *d) {
                    return d && d->loc.end.line == line_number;
                });
                if (it == multi_ds.rend()) {
                    break;
                }
                auto  line       = std::vector<colored_text>();
                auto *mult_cross = (const detail *)nullptr;
                for (auto mult : multi_ds) {
                    if (mult) {
                        if (mult == *it) {
                            mult_cross = mult;
                            line.emplace_back(
                                (std::stringstream() << " " << chars.lbot).str(),
                                mult->message.col);
                        } else {
                            if (mult_cross) {
                                line.emplace_back(chars.hbar, mult_cross->message.col);
                                line.emplace_back(chars.hbar, mult_cross->message.col);
                            } else {
                                line.emplace_back(
                                    (std::stringstream() << " " << chars.vbar).str(),
                                    mult->message.col);
                            }
                        }

                    } else {
                        if (mult_cross) {
                            line.emplace_back(
                                (std::stringstream() << chars.hbar << chars.hbar).str(),
                                mult_cross->message.col);
                        } else {
                            line.emplace_back("  ");
                        }
                    }
                }
                *it = nullptr;

                line.emplace_back(
                    (std::stringstream() << chars.hbar << chars.hbar << chars.hbar << " "
                                         << mult_cross->message.message)
                        .str(),
                    mult_cross->message.col);

                auto ss = std::stringstream();
                ss << std::string(last_line_width + 1, ' ') << chars.vbar_break << " ";
                line.emplace(line.begin(), ss.str(), color::gray);
                output.push_back(line);
            }
            last_line = line_number;
        }

        {
            auto ss = std::stringstream();
            for (int i = 0; i < last_line_width + 1; i++) {
                ss << chars.hbar;
            }
            ss << chars.rbot;
            output.push_back({{ss.str(), color::gray}});
        }
    }

    for (auto &line : output) {
        out << line << std::endl;
    }

#if defined(_WIN32) || defined(_WIN64)
    SetConsoleOutputCP(prev_cp);
#endif
}

} // namespace print
