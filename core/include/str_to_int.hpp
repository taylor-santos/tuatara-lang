//
// Created by taylor-santos on 1/6/2022 at 12:46.
//

#pragma once

template<typename T>
bool
str_to_int(const std::string &str, T &result, int base = 10) {
    auto res = T{};
    auto cutoff = std::numeric_limits<T>::max();
    auto cutlim = cutoff % static_cast<T>(base);
    cutoff /= static_cast<T>(base);
    for (auto c : str) {
        auto d = static_cast<T>(c);
        if (std::isdigit(c)) {
            d -= '0';
        } else if (std::isalpha(c)) {
            d -= std::isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            return true;
        }
        if (res > cutoff || (res == cutoff && d > cutlim)) {
            return true;
        }
        res = res * base + d;
    }
    result = res;
    return false;
}
