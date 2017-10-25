#include "string_and_encoding/unicode_converters.h"
#include "exceptions.h"
#include <cuchar>

String operator ""_s(const char16_t *chars, std::size_t length) {
	return String(chars, length);
}

TString<StdCharTraits<char>> operator ""_s(const char *chars, std::size_t length) {
	return TString<StdCharTraits<char>>(chars, length);
}