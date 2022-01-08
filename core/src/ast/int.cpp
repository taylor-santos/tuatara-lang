//
// Created by taylor-santos on 1/2/2022 at 20:37.
//

#include "ast/int.hpp"
#include "json.hpp"

#include <ostream>

namespace AST {

template<typename T>
Int<T>::Int(T value, const yy::location &loc)
    : Node(loc)
    , value_{value} {}

template<typename T>
Int<T>::~Int() = default;

template<typename T>
void
Int<T>::to_json(std::ostream &os) const {
    os << "{"
       << R"("node":")" << type_name << R"(",)"
       << R"("location":)";
    loc_to_json(get_loc(), os);
    os << ","
       << R"("value":)";
    if constexpr (std::is_signed_v<T>) {
        os << static_cast<int64_t>(value_);
    } else {
        os << static_cast<uint64_t>(value_);
    }
    os << "}";
}

template<>
const char *Int<uint64_t>::type_name = "U64";
template<>
const char *Int<int64_t>::type_name = "I64";
template<>
const char *Int<uint32_t>::type_name = "U32";
template<>
const char *Int<int32_t>::type_name = "I32";
template<>
const char *Int<uint16_t>::type_name = "U16";
template<>
const char *Int<int16_t>::type_name = "I16";
template<>
const char *Int<uint8_t>::type_name = "U8";
template<>
const char *Int<int8_t>::type_name = "I8";

template class Int<uint64_t>;
template class Int<int64_t>;
template class Int<uint32_t>;
template class Int<int32_t>;
template class Int<uint16_t>;
template class Int<int16_t>;
template class Int<uint8_t>;
template class Int<int8_t>;

} // namespace AST
