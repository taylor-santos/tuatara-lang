//
// Created by taylor-santos on 12/30/2021 at 18:38.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include <numeric>
#include <iomanip>

#include "driver.hpp"
#include "ast/expression.hpp"
#include "printer.hpp"

#ifdef __MINGW32__
#    pragma GCC diagnostic ignored "-Wcast-function-type"
#elif defined(_WIN32) || defined(_WIN64)
#    if !defined(NOMINMAX)
#        define NOMINMAX // Prevent collision with std::max
#    endif
#endif
#include "rang.hpp"

#define POS(FILE, LINE, COL) \
    yy::position { (FILE), (LINE), (COL) }
#define LOC(FILE, BEGIN_LINE, BEGIN_COL, END_LINE, END_COL) \
    yy::location { POS((FILE), (BEGIN_LINE), (BEGIN_COL)), POS((FILE), (END_LINE), (END_COL)) }

int
main(int argc, char **argv) try {
    rang::setControlMode(rang::control::Auto);
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    auto times = std::vector{std::chrono::high_resolution_clock::now()};

    std::ifstream input;
    input.exceptions(std::ios::badbit | std::ios::failbit);
    try {
        input.open(argv[1]);
    } catch (std::ios_base::failure &) {
        std::cerr << argv[1] << ": " << std::strerror(errno) << std::endl;
        return 1;
    }
    auto driver = Driver(input);

    times.push_back(std::chrono::high_resolution_clock::now());

    auto path         = std::filesystem::relative(argv[1]).string();
    auto parse_status = driver.parse(&path);

    times.push_back(std::chrono::high_resolution_clock::now());

    /*
    std::cout << "[";
    std::string sep;
    for (auto &node : driver.ast()) {
        std::cout << sep;
        sep = ",\n";
        node->to_json(std::cout);
    }
    std::cout << "]\n";
     */

    auto ctx              = driver.type_check();
    auto typecheck_status = ctx.did_fail();

    times.push_back(std::chrono::high_resolution_clock::now());

    {
        using namespace std::chrono;
        auto diffs = std::vector<long long>();
        std::transform(
            times.begin(),
            std::prev(times.end()),
            std::next(times.begin()),
            std::back_inserter(diffs),
            [](auto &a, auto &b) { return duration_cast<nanoseconds>(b - a).count(); });

        auto mtot = duration_cast<nanoseconds>(times.back() - times.front()).count();

        using namespace std::chrono_literals;
        auto ns_per_ms = duration_cast<nanoseconds>(1ms).count();
        auto ns_per_s  = duration_cast<nanoseconds>(1s).count();

        auto ftos = [](float f, int prec = 3) -> std::string {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(prec) << f;
            return ss.str();
        };

        auto LOC = driver.ast().size();
        auto LPS = (float)LOC / ((float)mtot / (float)ns_per_s);

        std::vector<std::string> strs;
        std::transform(diffs.begin(), diffs.end(), std::back_inserter(strs), [&](auto diff) {
            return ftos((float)diff / (float)ns_per_ms);
        });
        strs.push_back(std::to_string(LOC));
        strs.push_back(ftos(LPS));

        std::vector<size_t> lens;
        std::transform(strs.begin(), strs.end(), std::back_inserter(lens), [](auto &str) {
            return str.size();
        });

        auto w = *std::max_element(lens.begin(), lens.end());

        std::cout << "Init       " << std::setw(w) << strs[0] << "ms\n";
        std::cout << "Parse      " << std::setw(w) << strs[1] << "ms\n";
        std::cout << "Type Check " << std::setw(w) << strs[2] << "ms\n";
        std::cout << "Lines      " << std::setw(w - 4) << strs[3] << "\n";
        std::cout << "Lines/sec  " << std::setw(w) << strs[4] << "\n";
    }

    for (auto &message : driver.errors()) {
        message.print(driver.lines(), std::cout);
    }

    ctx.print_symbols(std::cout);

    return parse_status || typecheck_status;
} catch (std::exception &e) {
    std::cerr << e.what();
    exit(EXIT_FAILURE);
}
