//
// Created by taylor-santos on 12/30/2021 at 18:38.
//

#include <iostream>
#include <fstream>
#include <cstring>

#include "scanner.hpp"
#include "type/type_checker.hpp"
#include "driver.hpp"

#define POS(FILE, LINE, COL)  \
    yy::position {            \
        (FILE), (LINE), (COL) \
    }
#define LOC(FILE, BEGIN_LINE, BEGIN_COL, END_LINE, END_COL)                        \
    yy::location {                                                                 \
        POS((FILE), (BEGIN_LINE), (BEGIN_COL)), POS((FILE), (END_LINE), (END_COL)) \
    }

int
main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    std::vector<std::string> lines{
        "1234567890",
        "1234567890",
        "1234567890",
        "1234567890",
        "1234567890",
    };
    auto           fn      = std::string("foobar.tta");
    print::Message message = print::Message::error(POS(&fn, 2, 4))
                                 .with_message("this is an error")
                                 .in(print::color::bold_gray)
                                 .with_detail_at(LOC(&fn, 1, 8, 2, 3))
                                 .with_message("foo")
                                 .in(print::color::bold_red)
                                 .with_detail_at(LOC(&fn, 2, 7, 4, 2))
                                 .with_message("baz")
                                 .in(print::color::bold_green)
                                 .with_detail_at(LOC(&fn, 2, 4, 2, 6))
                                 .with_message("qux")
                                 .in(print::color::bold_magenta)
                                 .with_detail_at(LOC(&fn, 4, 4, 5, 4))
                                 .with_message("bar")
                                 .in(print::color::bold_yellow);
    message.print(lines, std::cout);

    //    std::ifstream input;
    //    input.exceptions(std::ios::badbit | std::ios::failbit);
    //    try {
    //        input.open(argv[1]);
    //    } catch (std::ios_base::failure &) {
    //        std::cerr << argv[1] << ": " << std::strerror(errno) << std::endl;
    //        return 1;
    //    }
    //    auto driver = Driver(input);
    //
    //    auto path = std::filesystem::relative(argv[1]).string();
    //    driver.parse(&path);
    //    auto ctx = driver.type_check();
    //
    //    for (auto &message : driver.errors()) {
    //        message.print(driver.lines(), std::cout);
    //    }
    //
    //    return 0;
}
