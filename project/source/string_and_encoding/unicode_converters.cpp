#include "string_and_encoding/string_lib.h"
#include "exceptions.h"
#include <cuchar>

String operator ""_s(const char16_t *chars, std::size_t length) {
	return String(chars, length);
}

TString<StdCharTraits<char>> operator ""_s(const char *chars, std::size_t length) {
	return TString<StdCharTraits<char>>(chars, length);
}