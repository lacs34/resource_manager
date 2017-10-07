/*
 * string_lib.h
 *
 *  Created on: 2016Äê3ÔÂ25ÈÕ
 *      Author: ÌÎ
 */

#ifndef PROJECT_INCLUDE_STRING_AND_ENCODING_STRING_LIB_H_
#define PROJECT_INCLUDE_STRING_AND_ENCODING_STRING_LIB_H_

#include "smart_pointer.h"
#include "compare_result.h"
#include "arrays.h"
#include "common_definition.h"
#include "collections_interfaces.h"
#include "predefined_array.h"
#include <string>

template<typename CHAR_TYPE>
class StdCharTraits;

template<>
class StdCharTraits<char> {
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

template<>
class StdCharTraits<char16_t> {
private:
	typedef std::char_traits<char16_t> StdTraits;

public:
	typedef char16_t CHAR_TYPE;
	static void Copy(char16_t *destination, const char16_t *source, size_t charCount) {
		StdTraits::copy(destination, source, charCount);
	}
	static CompareResult Compare(const char16_t *string1, const char16_t *string2, size_t charCount) {
		int result = StdTraits::compare(string1, string2, charCount);
		if (result < 0) {
			return CompareResult::Less;
		}
		else if (result == 0) {
			return CompareResult::Equal;
		}
		return CompareResult::Greater;
	}
	static size_t Length(const char16_t *string) {
		return StdTraits::length(string);
	}
};

template<typename CHAR_TRAITS>
bool IsEqual(
		const typename CHAR_TRAITS::CHAR_TYPE *string1,
		size_t length1,
		const typename CHAR_TRAITS::CHAR_TYPE *string2,
		size_t length2) {
	if (length1 != length2) {
		return false;
	}
	return CHAR_TRAITS::Compare(string1, string2, length1) == CompareResult::Equal;
}

template<typename CHAR_TRAITS>
class TString;

template<typename CHAR_TRAITS>
bool operator ==(const TString<CHAR_TRAITS> string1, const TString<CHAR_TRAITS> string2);
template<typename CHAR_TRAITS>
bool operator ==(const TString<CHAR_TRAITS> string1, const typename CHAR_TRAITS::CHAR_TYPE *string2);
template<typename CHAR_TRAITS>
bool operator ==(const typename CHAR_TRAITS::CHAR_TYPE *string1, const TString<CHAR_TRAITS> string2);

template<typename CHAR_TRAITS>
class TString :
	public Iteratable<typename CHAR_TRAITS::CHAR_TYPE> {
private:
	typedef typename CHAR_TRAITS::CHAR_TYPE CharType;
	Array<CharType> m_CharArray;
	Array<CharType> CreateCharArray(const CharType *chars, size_t length) {
		StdArray<CharType> content(length);
		content.CopyFrom(chars, length);
		return content;
	}

public:
	TString(const CharType *chars, size_t length) :
		m_CharArray(CreateCharArray(chars, length)) {
	}
	TString(const CharType *cStyleString) :
		TString(cStyleString, CHAR_TRAITS::Length(cStyleString)) {
	}
	TString(const Array<CharType> charArray) :
		TString(charArray.GetAddress(), charArray.GetLength()) {
	}
	inline size_t GetLength() {
		return m_CharArray.GetLength();
	}
	CharType operator [](size_t index) {
		return m_CharArray.GetElementAt(index);
	}

	virtual SmartPointer<Iterator<CharType>> GetIterator() override {
	}

	friend bool operator ==<CHAR_TRAITS>(const TString<CHAR_TRAITS> string1, const TString<CHAR_TRAITS> string2);
	friend bool operator ==<CHAR_TRAITS>(const TString<CHAR_TRAITS> string1, const typename CHAR_TRAITS::CHAR_TYPE *string2);
	friend bool operator ==<CHAR_TRAITS>(const typename CHAR_TRAITS::CHAR_TYPE *string1, const TString<CHAR_TRAITS> string2);
};
template<typename CHAR_TRAITS>
bool operator ==(const TString<CHAR_TRAITS> string1, const TString<CHAR_TRAITS> string2) {
	size_t length1 = string1.GetLength();
	size_t length2 = string2.GetLength();
	return IsEqual(string1.m_CharArray.GetAddress(), length1, string2.m_CharArray.GetAddress(), length2);
}
template<typename CHAR_TRAITS>
bool operator ==(const TString<CHAR_TRAITS> string1, const typename CHAR_TRAITS::CHAR_TYPE *string2) {
	size_t length1 = string1.GetLength();
	size_t length2 = CHAR_TRAITS::Length(string2);
	return IsEqual(string1.m_CharArray.GetAddress(), length1, string2, length2);
}
template<typename CHAR_TRAITS>
bool operator ==(const typename CHAR_TRAITS::CHAR_TYPE *string1, const TString<CHAR_TRAITS> string2) {
	size_t length1 = CHAR_TRAITS::Length(string1);
	size_t length2 = string2.GetLength();
	return IsEqual(string1, length1, string2.m_CharArray.GetAddress(), length2);
}

typedef TString<StdCharTraits<char16_t>> String;
#define _S(content) u##content

#endif /* PROJECT_INCLUDE_STRING_AND_ENCODING_STRING_LIB_H_ */
