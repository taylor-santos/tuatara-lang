//
// Created by taylor-santos on 8/13/2021 at 09:12.
//

#pragma once

#include <filesystem>
#include <istream>
#include <vector>

/***
 *  @brief  Wrapper for an std::istream that reads and stores individual lines, separated by common
 *  newline delimiters. Partially consumed lines are stored in full in the lines vector.
 */
class LineStream : public std::istream {
public:
    explicit LineStream(std::istream &input);
    ~LineStream() override;

    [[nodiscard]] inline const std::vector<std::string> &
    lines() const {
        return buffer_.lines();
    }

private:
    class LineBuffer : public std::streambuf {
    public:
        explicit LineBuffer(std::istream &input);

        LineBuffer(const LineBuffer &) = delete;

        std::streambuf::int_type
        underflow() override;

        [[nodiscard]] inline const std::vector<std::string> &
        lines() const {
            return lines_;
        }

    private:
        std::istream            &input_;
        std::vector<char>        line_;
        std::vector<std::string> lines_;
    };

    LineBuffer buffer_;
};
