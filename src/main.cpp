//
// Created by taylor-santos on 12/30/2021 at 18:38.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>

#include "scanner.hpp"
#include "ast/expression.hpp"
#include "type/context.hpp"

int
main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    std::ifstream input;
    input.exceptions(std::ios::badbit | std::ios::failbit);
    try {
        input.open(argv[1]);
    } catch (std::ios_base::failure &) {
        std::cerr << argv[1] << ": " << std::strerror(errno) << std::endl;
        return 1;
    }
    auto lines   = LineStream(input);
    auto path    = std::filesystem::relative(argv[1]);
    auto scanner = yy::Scanner(path.string(), lines);
    auto ast     = std::vector<std::unique_ptr<AST::Expression>>();
    auto failed  = false;
    auto parser  = yy::Parser(scanner, std::cerr, lines.lines(), ast, failed);
    scanner.set_debug_level(0);
    parser.set_debug_level(0);
    parser.parse();
    if (failed) {
        exit(EXIT_FAILURE);
    }

    TypeChecker::Context                   ctx;
    std::vector<const TypeChecker::Type *> types;
    types.reserve(ast.size());
    std::transform(
        ast.begin(),
        ast.end(),
        std::back_inserter(types),
        [&ctx](const auto &expr) -> const TypeChecker::Type * { return &expr->get_type(ctx); });

    std::cout << "[";
    std::string sep;
    for (const auto &expr : ast) {
        std::cout << sep;
        sep = ",";
        expr->to_json(std::cout);
    }
    std::cout << "]";
    std::cout << std::endl;
    ctx.print_symbols(std::cout);

    return ctx.did_fail();
}
