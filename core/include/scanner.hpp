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
    Scanner(std::string path, std::istream &input);
    ~Scanner() override;

    Scanner(const Scanner &) = delete;
    auto
    operator=(const Scanner &) = delete;

    void
    set_debug_level(int l);

    virtual Parser::symbol_type
    scan();

private:
    location      loc_;
    std::string   path_;
    std::istream &input_;

    std::string int_str_;
    enum class Base { DEC, HEX, OCT, BIN } int_base_ = Base::DEC;
    enum class Size { _64, _32, _16, _8 } int_size_  = Size::_64;
    bool int_signed_                                 = false;
};

} // namespace yy
