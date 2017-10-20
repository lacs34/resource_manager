/*
* string_traits.h
*
*  Created on: 2016Äê3ÔÂ20ÈÕ
*      Author: Administrator
*/

#ifndef STRING_TRAITS_H_
#define STRING_TRAITS_H_

#include <string>

class MultibyteCharactersStringsTraits {
private:
	typedef std::char_traits<char> StdTraits;

public:
	typedef char CHAR_TYPE;
	static void Copy(char *destination, const char *source, size_t charCount) {
		StdTraits::copy(destination, source, charCount);
	}
	static CompareResult Compare(const char *string1, const char *string2, size_t charCount) {
		int result = StdTraits::compare(string1, string2, charCount);
		if (result < 0) {
			return CompareResult::Less;
		}
		else if (result == 0) {
			return CompareResult::Equal;
		}
		return CompareResult::Greater;
	}
	static size_t Length(const char *string) {
		return StdTraits::length(string);
	}
};

class Utf8Traits {
private:
	typedef std::char_traits<char> StdTraits;

public:
	typedef char CHAR_TYPE;
	static void Copy(char *destination, const char *source, size_t charCount) {
		StdTraits::copy(destination, source, charCount);
	}
	static CompareResult Compare(const char *string1, const char *string2, size_t charCount) {
		int result = StdTraits::compare(string1, string2, charCount);
		if (result < 0) {
			return CompareResult::Less;
		}
		else if (result == 0) {
			return CompareResult::Equal;
		}
		return CompareResult::Greater;
	}
	static size_t Length(const char *string) {
		return StdTraits::length(string);
	}
};

class Utf16Traits {
private:
	typedef std::char_traits<char32_t> StdTraits;

public:
	typedef char16_t CHAR_TYPE;
	static void Copy(char32_t *destination, const char32_t *source, size_t charCount) {
		StdTraits::copy(destination, source, charCount);
	}
	static CompareResult Compare(const char32_t *string1, const char32_t *string2, size_t charCount) {
		int result = StdTraits::compare(string1, string2, charCount);
		if (result < 0) {
			return CompareResult::Less;
		}
		else if (result == 0) {
			return CompareResult::Equal;
		}
		return CompareResult::Greater;
	}
	static size_t Length(const char32_t *string) {
		return StdTraits::length(string);
	}
};

class Utf32Traits {
private:
	typedef std::char_traits<char32_t> StdTraits;

public:
	typedef char16_t CHAR_TYPE;
	static void Copy(char32_t *destination, const char32_t *source, size_t charCount) {
		StdTraits::copy(destination, source, charCount);
	}
	static CompareResult Compare(const char32_t *string1, const char32_t *string2, size_t charCount) {
		int result = StdTraits::compare(string1, string2, charCount);
		if (result < 0) {
			return CompareResult::Less;
		}
		else if (result == 0) {
			return CompareResult::Equal;
		}
		return CompareResult::Greater;
	}
	static size_t Length(const char32_t *string) {
		return StdTraits::length(string);
	}
};

class WideStringTraits {
private:
	typedef std::char_traits<wchar_t> StdTraits;

public:
	typedef wchar_t CHAR_TYPE;
	static void Copy(wchar_t *destination, const wchar_t *source, size_t charCount) {
		StdTraits::copy(destination, source, charCount);
	}
	static CompareResult Compare(const wchar_t *string1, const wchar_t *string2, size_t charCount) {
		int result = StdTraits::compare(string1, string2, charCount);
		if (result < 0) {
			return CompareResult::Less;
		}
		else if (result == 0) {
			return CompareResult::Equal;
		}
		return CompareResult::Greater;
	}
	static size_t Length(const wchar_t *string) {
		return StdTraits::length(string);
	}
};

template<typename CHAR_TYPE>
class CharTraits;
template<>
class CharTraits<char> {
public:
	typedef Utf8Traits TRAITS;
};
template<>
class CharTraits<char16_t> {
public:
	typedef Utf16Traits TRAITS;
};
template<>
class CharTraits<char32_t> {
public:
	typedef Utf32Traits TRAITS;
};
template<>
class CharTraits<wchar_t> {
public:
	typedef WideStringTraits TRAITS;
};

template<typename CHAR_TYPE>
using StdCharTraits = typename CharTraits<CHAR_TYPE>::TRAITS;

typedef Utf8Traits Utf8Encoding;
typedef Utf16Traits Utf16Encoding;
typedef WideStringTraits Utf32Encoding;


#endif // !STRING_TRAITS_H_
