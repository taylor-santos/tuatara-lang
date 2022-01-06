//
// Created by taylor-santos on 1/4/2022 at 20:30.
//

#include "json.hpp"
#include <sstream>
#include <iomanip>

void
pos_to_json(const yy::position &pos, std::ostream &os) {
    os << "{";
    if (pos.filename) {
        os << R"("filename":")" << escape_string(*pos.filename) << R"(",)";
    }
    os << R"("line":)" << pos.line << ","
       << R"("column":)" << pos.column << "}";
}

void
loc_to_json(const yy::location &loc, std::ostream &os) {
    os << "{"
       << R"("begin":)";
    pos_to_json(loc.begin, os);
    os << ","
       << R"("end":)";
    pos_to_json(loc.end, os);
    os << "}";
}

std::string
escape_string(const std::string &str) {
    std::ostringstream o;
    for (auto c = str.cbegin(); c != str.cend(); c++) {
        switch (*c) {
            case '"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default:
                if ('\x00' <= *c && *c <= '\x1f') {
                    o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
                } else {
                    o << *c;
                }
        }
    }
    return o.str();
}
