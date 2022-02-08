//
// Created by taylor-santos on 8/13/2021 at 09:12.
//

#include "line_stream.hpp"

#include <optional>
#include <vector>

LineStream::LineStream(std::istream &input)
    : std::istream(&buffer_)
    , buffer_(input) {}

LineStream::~LineStream() = default;

LineStream::LineBuffer::LineBuffer(std::istream &input)
    : input_{input} {}

std::streambuf::int_type
LineStream::LineBuffer::underflow() {
    try {
        input_.peek();
    } catch (std::ios::failure &) {
        // peek() will throw if the stream's eofbit is set and EOF is encountered, catch it.
    }
    if (input_.eof()) {
        return traits_type::eof();
    }
    std::string line;
    char        c;
    bool        linebr = false;
    // Read until line break
    while (input_.get(c)) {
        int next = '\0';
        // Internal stream might throw on EOF, so catch it
        try {
            next = input_.peek();
        } catch (std::ios::failure &) {
            next = '\0';
        }

        // Check for various line ending types, and break if found
        if (c == '\r') {
            // CR or CRLF
            linebr = true;
            if (next == '\n') {
                input_.get();
            }
            break;
        } else if (c == '\n') {
            // LF or LFCR
            linebr = true;
            if (next == '\r') {
                input_.get();
            }
            break;
        } else if (c == '\036') {
            // RS
            linebr = true;
            break;
        }

        line.push_back(c);
        if (input_.eof()) {
            break;
        }
    }
    // lines_ vector should not contain newline characters...
    lines_.push_back(line);
    // ...but the output stream should if the input did.
    if (linebr) line.push_back('\n');
    line_ = std::vector<char>(line.begin(), line.end());
    setg(line_.data(), line_.data(), line_.data() + line_.size());
    return traits_type::to_int_type(*gptr());
}
