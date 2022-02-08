%require "3.7"
%language "C++"
%locations
%defines
%parse-param {class Scanner &scanner}
%parse-param {std::ostream &output}
%parse-param {const std::vector<std::string> &lines}
%parse-param {std::vector<std::unique_ptr<AST::Expression>> &ast_out}
%parse-param {bool &parse_failure}

%define api.namespace {yy}
%define api.parser.class {Parser}
%define api.token.constructor
%define api.value.type variant
%define api.value.automove
%define parse.assert
%define parse.trace
%define parse.error verbose
%define parse.lac full
%define api.token.prefix {TOK_}

%code requires {
// This block is inserted at the top of parser.hpp

// Disable compiler-specific warnings for code generated by Bison
#if defined(__clang__)
#    pragma clang diagnostic push
#elif defined(__GNUC__) || defined(__GNUG__)
#    pragma GCC diagnostic push
#elif defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:                                                                                           \
         4065 /* switch statement contains 'default' but no 'case' labels */                                           \
     )
#endif

#include "ast/ast_includes.hpp"

} // %code requires

%code provides {
// This block is inserted at the bottom of parser.hpp

#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#    pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif

} // %code provides

%{
// This block is inserted at the top of parser.cpp

// Disable compiler-specific warnings for code generated by Bison
#if defined(__clang__)
#elif defined(__GNUC__) || defined(__GNUG__)
#elif defined(_MSC_VER)
#    pragma warning(disable:                                                                                           \
         4065 /* switch statement contains 'default' but no 'case' labels */                                           \
         4127 /* conditional expression is constant */                                                                 \
         4244 /* 'return': conversion from 'std::streamsize' to 'int', possible loss of data */                        \
     )
#endif

#include "scanner.hpp"

#define yylex scanner.scan

#define NODE(type, ...) std::make_unique<AST:: type>(__VA_ARGS__)

%}

%initial-action {
    // Invoked before parsing each time parse() is called.
    parse_failure = false;
}

/* TOKENS */
%token
    EOF 0       "end of file"
    SEMICOLON   ";"
    TYPE_DECL   "::"
    DEFINE      ":="

%token<uint64_t>
    U64     "u64"

%token<std::string>
    IDENT       "identifier"
    TYPENAME    "type name"

%type<std::vector<std::unique_ptr<AST::Expression>>>
    expressions

%type<std::unique_ptr<AST::Expression>>
    expression

%type<std::unique_ptr<AST::Definition>>
    definition

%type<std::unique_ptr<AST::SimpleExpression>>
    simple_expression

%type<std::unique_ptr<AST::Literal>>
    literal

%type<std::unique_ptr<AST::Type>>
    type

%start file

%%

file
    : %empty {
        ast_out.clear();
    }
    | expressions {
        ast_out = $1;
        if (parse_failure) YYABORT;
    }

expressions
    : expression ";" {
        $$.emplace_back($1);
    }
    | expressions expression ";" {
        $$ = $1;
        $$.emplace_back($2);
    }

expression
    : definition {
        $$ = $1;
    }
    | simple_expression {
        $$ = $1;
    }

definition
    : "identifier" ":=" expression {
        $$ = NODE(ValueDefinition, $1, @1, $3, @$);
    }
    | "identifier" "::" type {
        $$ = NODE(TypeDefinition, $1, @1, $3, @$);
    }

simple_expression
    : literal {
        $$ = $1;
    }
    | "identifier" {
        $$ = NODE(Variable, $1, @$);
    }
    | error {
        $$ = NODE(Error, @$);
    }

literal
    : "u64" {
        $$ = NODE(U64, $1, @$);
    }

type
    : "type name" {
        $$ = NODE(ObjectType, $1, @1, @$);
    }
    | error {
        $$ = NODE(Error, @$);
    }

%%

// This block is inserted at the bottom of parser.cpp

#include <iomanip>

namespace yy {

void
Parser::error(const location &loc, const std::string &message) {
    if (loc.begin.filename) output << *loc.begin.filename << ":";
    output << loc.begin.line << ":" << loc.begin.column << "-";
    if (loc.end.filename && loc.end.filename != loc.begin.filename) output << *loc.end.filename << ":";
    output << loc.end.line << ":" << loc.end.column;
    output << " " << message << std::endl;

    auto num_width = static_cast<int>(std::to_string(loc.end.line).size());
    auto begin_line = std::max(loc.begin.line - 1, 1);
    auto end_line = loc.end.line;

    for (auto line_no = begin_line; line_no <= end_line; line_no++) {
        auto line = lines[line_no - 1];
        auto first = line.find_first_not_of(' ');
        if (first == std::string::npos) continue;

        output << std::setw(num_width) << std::right << line_no << " | ";
        output << line << std::endl;

        if (line_no < loc.begin.line || loc.end.line < line_no) continue;

        first = std::max((int)first, (line_no == loc.begin.line) ? (loc.begin.column - 1) : 0);
        auto last = (line_no < end_line) ? line.find_last_not_of(' ') : (loc.end.column - 1);

        output << std::string(num_width, ' ') << " | ";
        output << std::string(first, ' ');
        output << std::string(last - first, '~') << std::endl;

    }

    parse_failure = true;
}

}
