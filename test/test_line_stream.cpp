//
// Created by taylor-santos on 8/13/2021 at 09:12.
//

#include "doctest/doctest.h"
#include "line_stream.hpp"

#include <sstream>

DOCTEST_CLANG_SUPPRESS_WARNING("-Wunused-macros")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunused-template")

TEST_SUITE_BEGIN("Line Stream");
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING 1

SCENARIO("line stream test cases") {
    std::stringstream input;
    char              output[64];
    std::fill(std::begin(output), std::end(output), 'a');

    GIVEN("an empty stream") {
        input << "";
        WHEN("the line stream is consumed") {
            LineStream ls(input);
            ls.read(output, 64);
            std::string str(output, output + ls.gcount());
            THEN("the output should be empty") {
                CHECK(str == "");
            }
            THEN("it should have no lines") {
                REQUIRE(ls.lines().size() == 0);
            }
        }
    }

    GIVEN("one line without line break") {
        input << "foo";
        WHEN("the line stream is consumed") {
            LineStream ls(input);
            ls.read(output, 64);
            std::string str(output, output + ls.gcount());
            THEN("the output should be the line") {
                CHECK(str == "foo");
            }
            THEN("it should have one line") {
                REQUIRE(ls.lines().size() == 1);
                CHECK(ls.lines()[0] == "foo");
            }
        }
    }

    GIVEN("one line with LF") {
        input << "foo\n";
        WHEN("the line stream is consumed") {
            LineStream ls(input);
            ls.read(output, 64);
            std::string str(output, output + ls.gcount());
            THEN("the output should be the line with \\n") {
                CHECK(str == "foo\n");
            }
            THEN("it should have one line") {
                REQUIRE(ls.lines().size() == 1);
                CHECK(ls.lines()[0] == "foo");
            }
        }
    }

    GIVEN("one line with RS") {
        input << "foo\036";
        WHEN("the line stream is consumed") {
            LineStream ls(input);
            ls.read(output, 64);
            std::string str(output, output + ls.gcount());
            THEN("the output should be the line with \\n") {
                CHECK(str == "foo\n");
            }
            THEN("it should have one line") {
                REQUIRE(ls.lines().size() == 1);
                CHECK(ls.lines()[0] == "foo");
            }
        }
    }

    GIVEN("one line with CR") {
        input << "foo\r";
        WHEN("the line stream is consumed") {
            LineStream ls(input);
            ls.read(output, 64);
            std::string str(output, output + ls.gcount());
            THEN("the output should be the line with \\n") {
                CHECK(str == "foo\n");
            }
            THEN("it should have one line") {
                REQUIRE(ls.lines().size() == 1);
                CHECK(ls.lines()[0] == "foo");
            }
        }
    }

    GIVEN("one line with CRLF") {
        input << "foo\r\n";
        WHEN("the line stream is consumed") {
            LineStream ls(input);
            ls.read(output, 64);
            std::string str(output, output + ls.gcount());
            THEN("the output should be the line with \\n") {
                CHECK(str == "foo\n");
            }
            THEN("it should have one line") {
                REQUIRE(ls.lines().size() == 1);
                CHECK(ls.lines()[0] == "foo");
            }
        }
    }

    GIVEN("one line with LFCR") {
        input << "foo\n\r";
        WHEN("the line stream is consumed") {
            LineStream ls(input);
            ls.read(output, 64);
            std::string str(output, output + ls.gcount());
            THEN("the output should be the line with \\n") {
                CHECK(str == "foo\n");
            }
            THEN("it should have one line") {
                REQUIRE(ls.lines().size() == 1);
                CHECK(ls.lines()[0] == "foo");
            }
        }
    }

    GIVEN("two lines without ending line break") {
        input << "foo\nbar";
        WHEN("the line stream is consumed") {
            LineStream ls(input);
            ls.read(output, 64);
            std::string str(output, output + ls.gcount());
            THEN("the output should be both lines separated by \\n") {
                CHECK(str == "foo\nbar");
            }
            THEN("it should have two lines") {
                REQUIRE(ls.lines().size() == 2);
                CHECK(ls.lines()[0] == "foo");
                CHECK(ls.lines()[1] == "bar");
            }
        }
    }
}

SCENARIO("line stream ignores wrapped stream exceptions") {
    std::stringstream input;
    char              output[64];
    std::fill(std::begin(output), std::end(output), 'a');

    GIVEN("an input stream with some data") {
        input << "foo\r\nbar\r\n";
        WHEN("the input stream has exceptions disabled") {
            input.exceptions(std::ios::goodbit);
            THEN("consuming the stream will not throw an exception") {
                CHECK_NOTHROW(input.read(output, 64));
                AND_THEN("the output should contain the input text") {
                    std::string str(output, output + input.gcount());
                    CHECK(str == "foo\r\nbar\r\n");
                }
            }
            AND_WHEN("a line stream wraps the input stream") {
                LineStream ls(input);
                AND_WHEN("the line stream has exceptions disabled") {
                    ls.exceptions(std::ios::goodbit);
                    THEN("consuming the line stream will not throw an exception") {
                        CHECK_NOTHROW(ls.read(output, 64));
                        AND_THEN("the output should contain the input text") {
                            std::string str(output, output + ls.gcount());
                            CHECK(str == "foo\nbar\n");
                        }
                        AND_THEN("the line stream should have the correct lines") {
                            REQUIRE(ls.lines().size() == 2);
                            CHECK(ls.lines()[0] == "foo");
                            CHECK(ls.lines()[1] == "bar");
                        }
                    }
                }
                AND_WHEN("the line stream has exceptions enabled") {
                    ls.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
                    THEN("consuming the line stream will throw an exception") {
                        CHECK_THROWS_AS(ls.read(output, 64), std::ios::failure);
                        AND_THEN("the output should contain the input text") {
                            std::string str(output, output + ls.gcount());
                            CHECK(str == "foo\nbar\n");
                        }
                        AND_THEN("the line stream should have the correct lines") {
                            REQUIRE(ls.lines().size() == 2);
                            CHECK(ls.lines()[0] == "foo");
                            CHECK(ls.lines()[1] == "bar");
                        }
                    }
                }
            }
        }
        WHEN("the input stream has exceptions enabled") {
            input.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
            THEN("consuming the stream will throw an exception") {
                CHECK_THROWS_AS(input.read(output, 64), std::ios::failure);
                AND_THEN("the output should contain the input text") {
                    std::string str(output, output + input.gcount());
                    CHECK(str == "foo\r\nbar\r\n");
                }
            }
            AND_WHEN("a line stream wraps the input stream") {
                LineStream ls(input);
                AND_WHEN("the line stream has exceptions disabled") {
                    ls.exceptions(std::ios::goodbit);
                    THEN("consuming the line stream will not throw an exception") {
                        CHECK_NOTHROW(ls.read(output, 64));
                        AND_THEN("the output should contain the input text") {
                            std::string str(output, output + ls.gcount());
                            CHECK(str == "foo\nbar\n");
                        }
                        AND_THEN("the line stream should have the correct lines") {
                            REQUIRE(ls.lines().size() == 2);
                            CHECK(ls.lines()[0] == "foo");
                            CHECK(ls.lines()[1] == "bar");
                        }
                    }
                }
                AND_WHEN("the line stream has exceptions enabled") {
                    ls.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
                    THEN("consuming the line stream will throw an exception") {
                        CHECK_THROWS_AS(ls.read(output, 64), std::ios::failure);
                        AND_THEN("the output should contain the input text") {
                            std::string str(output, output + ls.gcount());
                            CHECK(str == "foo\nbar\n");
                        }
                        AND_THEN("the line stream should have the correct lines") {
                            REQUIRE(ls.lines().size() == 2);
                            CHECK(ls.lines()[0] == "foo");
                            CHECK(ls.lines()[1] == "bar");
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("partially consuming an input") {
    std::stringstream input;
    LineStream        ls(input);
    char              output[64];
    std::fill(std::begin(output), std::end(output), 'a');

    GIVEN("a long input") {
        input << "123456789\nabcdefghi\njklmnopqr\n";
        WHEN("less than one line is consumed") {
            ls.read(output, 5);
            THEN("the whole line should be available") {
                REQUIRE(ls.lines().size() >= 1);
                CHECK(ls.lines()[0] == "123456789");
            }
            THEN("the output should contain the first part of the input") {
                std::string str(output, output + ls.gcount());
                CHECK(str == "12345");
            }
            AND_WHEN("another line is partially consumed") {
                ls.read(output, 10);
                THEN("two lines should be fully available") {
                    REQUIRE(ls.lines().size() >= 2);
                    CHECK(ls.lines()[0] == "123456789");
                    CHECK(ls.lines()[1] == "abcdefghi");
                }
                THEN("the output should contain the next part of the input") {
                    std::string str(output, output + ls.gcount());
                    CHECK(str == "6789\nabcde");
                }
            }
        }
    }
}

SCENARIO("all newlines become \\n") {
    std::stringstream input;
    LineStream        ls(input);
    char              output[64];
    std::fill(std::begin(output), std::end(output), 'a');

    GIVEN("an input with \\n\\n") {
        input << "foo\n\n";
        WHEN("the input is consumed") {
            ls.read(output, 64);
            THEN("the output should contain \\n\\n") {
                std::string str(output, output + ls.gcount());
                CHECK(str == "foo\n\n");
            }
        }
    }
    GIVEN("an input with \\r\\n\\r\\n") {
        input << "foo\r\n\r\n";
        WHEN("the input is consumed") {
            ls.read(output, 64);
            THEN("the output should contain \\n\\n") {
                std::string str(output, output + ls.gcount());
                CHECK(str == "foo\n\n");
            }
        }
    }
    GIVEN("an input with \\r") {
        input << "foo\r\r";
        WHEN("the input is consumed") {
            ls.read(output, 64);
            THEN("the output should contain \\n\\n") {
                std::string str(output, output + ls.gcount());
                CHECK(str == "foo\n\n");
            }
        }
    }
    GIVEN("an input with \\n\\r") {
        input << "foo\n\r\n\r";
        WHEN("the input is consumed") {
            ls.read(output, 64);
            THEN("the output should contain \\n\\n") {
                std::string str(output, output + ls.gcount());
                CHECK(str == "foo\n\n");
            }
        }
    }
}
