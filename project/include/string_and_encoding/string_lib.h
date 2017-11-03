/*
 * string_lib.h
 *
 *  Created on: 2016Äê3ÔÂ25ÈÕ
 *      Author: ÌÎ
 */

#ifndef PROJECT_INCLUDE_STRING_AND_ENCODING_STRING_LIB_H_
#define PROJECT_INCLUDE_STRING_AND_ENCODING_STRING_LIB_H_

#include "arrays.h"
#include "smart_pointer.h"
#include "compare_result.h"
#include "common_definition.h"
#include "collections/collections_interfaces.h"
#include "predefined_array.h"
#include <basic_types.h>
#include <queue>
#include <vector>
#include <cuchar>
#include <cassert>
#if defined(WINDOWS_PLATFORM)
#include "windows_platform.h"
#endif

class NoMoreOutputException :
	public std::exception {
};

class IllegalInputFormatException :
	public std::exception {
};

#include "string_and_encoding/string_traits.h"

template<typename FROM_TYPE, typename TO_TYPE>
class ConverterPicker{
};

template<typename FROM_TYPE, typename TO_TYPE>
using Converter = typename ConverterPicker<FROM_TYPE, TO_TYPE>::CONVERTER_TYPE;

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
class CStyleStringIterator :
	public Iterator<typename CHAR_TRAITS::CHAR_TYPE> {
private:
	typedef typename CHAR_TRAITS::CHAR_TYPE CharType;
	const CharType *m_StartAddress;
	ResourceManager *m_Manager;

public:
	CStyleStringIterator(const CharType *startAddress, ResourceManager *manager = nullptr) :
		m_StartAddress(startAddress - 1),
		m_Manager(manager) {
		if (manager != nullptr) {
			manager->IncreaseReference();
		}
	}
	virtual bool MoveNext() override {
		if (*(m_StartAddress + 1) == CharType(0)) {
			return false;
		}
		++m_StartAddress;
		return true;
	}
	virtual CharType GetCurrent() override {
		return *m_StartAddress;
	}
	virtual ~CStyleStringIterator() {
		if (m_Manager != nullptr) {
			m_Manager->DecreaseReference();
		}
	}
};

template<typename CHAR_TRAITS>
class TString :
	public Iteratable<typename CHAR_TRAITS::CHAR_TYPE> {
private:
	typedef typename CHAR_TRAITS::CHAR_TYPE CharType;
	Buffer<CharType> m_CharArray;
	Buffer<CharType> CreateCharArray(const CharType *chars, size_t length) {
		StdArray<CharType> content(length);
		content.CopyFrom(chars, length);
		return content;
	}
	TString(const Buffer<CharType> charArray, bool needCopy) :
		m_CharArray(charArray) {
	}

public:
	TString(const CharType *chars, size_t length) :
		m_CharArray(CreateCharArray(chars, length)) {
	}
	TString(const CharType *cStyleString) :
		TString(cStyleString, CHAR_TRAITS::Length(cStyleString)) {
	}
	TString(const Buffer<CharType> charArray) :
		TString(charArray.GetAddress(), charArray.GetLength()) {
	}
	inline size_t GetLength() const {
		return m_CharArray.GetLength();
	}
	CharType operator [](size_t index) const {
		return m_CharArray.GetElementAt(index);
	}

	virtual SmartPointer<Iterator<CharType>> GetIterator() override {
		return m_CharArray.GetIterator();
	}

	template<typename FROM_CHAR_TRAITS, typename TO_CHAR_TRAITS>
	static Buffer<typename TO_CHAR_TRAITS::CHAR_TYPE> ConvertEncoding(SmartPointer<Iterator<typename FROM_CHAR_TRAITS::CHAR_TYPE>> sourceString) {
		Converter<FROM_CHAR_TRAITS, TO_CHAR_TRAITS> converter;
		return Convert(converter, sourceString);
	}

	template<typename FROM_CHAR_TRAITS>
	static TString<CHAR_TRAITS> From(const typename FROM_CHAR_TRAITS::CHAR_TYPE *chars, size_t length) {
		Scoped<CArrayIterator<typename FROM_CHAR_TRAITS::CHAR_TYPE>> stringIterator(chars, length);
		return TString<CHAR_TRAITS>(ConvertEncoding<FROM_CHAR_TRAITS, CHAR_TRAITS>((&stringIterator).Cast<Iterator<typename FROM_CHAR_TRAITS::CHAR_TYPE>>()), false);
	}
	template<typename FROM_CHAR_TRAITS>
	static TString<CHAR_TRAITS> From(const typename FROM_CHAR_TRAITS::CHAR_TYPE *cStyleString) {
		Scoped<CStyleStringIterator<FROM_CHAR_TRAITS>> stringIterator(cStyleString);
		return TString<CHAR_TRAITS>(ConvertEncoding<FROM_CHAR_TRAITS, CHAR_TRAITS>((&stringIterator).Cast<Iterator<typename FROM_CHAR_TRAITS::CHAR_TYPE>>()), false);
	}
	template<typename FROM_CHAR_TRAITS>
	static TString<CHAR_TRAITS> From(const Buffer<typename FROM_CHAR_TRAITS::CHAR_TYPE> charArray) {
		return From(charArray.GetAddress(), charArray.GetLength());
	}
	template<typename TO_CHAR_TRAITS>
	TString<TO_CHAR_TRAITS> ToString() {
		return TString<TO_CHAR_TRAITS>::From<CHAR_TRAITS>(m_CharArray);
	}
	template<typename TO_CHAR_TRAITS>
	Buffer<typename TO_CHAR_TRAITS::CHAR_TYPE> ToArray() {
		Scoped<CArrayIterator<CharType>> iterator(m_CharArray);
		return ConvertEncoding<CHAR_TRAITS, TO_CHAR_TRAITS>((&iterator).Cast<Iterator<CharType>>());
	}
	template<typename FROM_CHAR_TRAITS>
	static TString<CHAR_TRAITS> From(TString<FROM_CHAR_TRAITS> charArray) {
		return From(charArray.GetAddress(), charArray.GetLength());
	}

	friend bool operator ==<CHAR_TRAITS>(const TString<CHAR_TRAITS> string1, const TString<CHAR_TRAITS> string2);
	friend bool operator ==<CHAR_TRAITS>(const TString<CHAR_TRAITS> string1, const typename CHAR_TRAITS::CHAR_TYPE *string2);
	friend bool operator ==<CHAR_TRAITS>(const typename CHAR_TRAITS::CHAR_TYPE *string1, const TString<CHAR_TRAITS> string2);
};
template<typename CHAR_TRAITS>
bool operator ==(const TString<CHAR_TRAITS> string1, const TString<CHAR_TRAITS> string2) {
	size_t length1 = string1.GetLength();
	size_t length2 = string2.GetLength();
	return IsEqual<CHAR_TRAITS>(string1.m_CharArray.GetAddress(), length1, string2.m_CharArray.GetAddress(), length2);
}
template<typename CHAR_TRAITS>
bool operator ==(const TString<CHAR_TRAITS> string1, const typename CHAR_TRAITS::CHAR_TYPE *string2) {
	size_t length1 = string1.GetLength();
	size_t length2 = CHAR_TRAITS::Length(string2);
	return IsEqual<CHAR_TRAITS>(string1.m_CharArray.GetAddress(), length1, string2, length2);
}
template<typename CHAR_TRAITS>
bool operator ==(const typename CHAR_TRAITS::CHAR_TYPE *string1, const TString<CHAR_TRAITS> string2) {
	size_t length1 = CHAR_TRAITS::Length(string1);
	size_t length2 = string2.GetLength();
	return IsEqual<CHAR_TRAITS>(string1, length1, string2.m_CharArray.GetAddress(), length2);
}

typedef TString<Utf16Encoding> String;

// Stands for a character in the Universal Coded Character Set (UCS),
// which is a standard set of characters defined by the International Standard ISO/IEC 10646,
// Information technology ¡ª Universal Coded Character Set (UCS) (plus amendments to that standard),
// which is the basis of many character encodings.
class CodePoint {
public:
	static constexpr Int64 maxCodePoint = static_cast<Int64>(0x10FFFF);
	typedef SmallestUnsignedStorage<maxCodePoint> VALUE_TYPE;

private:
	SmallestUnsignedStorage<maxCodePoint> m_Value;

public:
	CodePoint(SmallestUnsignedStorage<maxCodePoint> value) :
		m_Value(value) {
	}
	String GetCharName();

};

#include "string_and_encoding/unicode_converters.h"

template<>
class ConverterPicker<StdCharTraits<char>, StdCharTraits<char16_t>> {
public:
	typedef Utf8ToUtf16Converter CONVERTER_TYPE;
};

String operator ""_s(const char16_t *chars, std::size_t length);
TString<StdCharTraits<char>> operator ""_s(const char *chars, std::size_t length);
TString<StdCharTraits<char32_t>> operator ""_s(const char32_t *chars, std::size_t length);

#define _S(content) u##content##_s

#endif /* PROJECT_INCLUDE_STRING_AND_ENCODING_STRING_LIB_H_ */
