//
// Created by taylor-santos on 12/30/2021 at 18:46.
//

#pragma once

#include <filesystem>

#include "parser.hpp"

#undef YY_DECL
#define YY_DECL yy::Parser::symbol_type yy::Scanner::scan()

#ifndef __FLEX_LEXER_H
#    include <FlexLexer.h>
#endif

namespace yy {

class Scanner : public yyFlexLexer {
public:
    Scanner(std::string path, std::istream &input, std::ostream &output);
    ~Scanner() override;

    Scanner(const Scanner &) = delete;
    auto
    operator=(const Scanner &) = delete;

    void
    set_debug_level(int l);

    virtual Parser::symbol_type
    scan();

    void
    report_error(const location &loc, const std::string &message);

    [[nodiscard]] bool
    had_error() const;

private:
    location      loc_;
    std::string   path_;
    std::istream &input_;
    std::ostream &output_;
    bool          error_ = false;
};

} // namespace yy
