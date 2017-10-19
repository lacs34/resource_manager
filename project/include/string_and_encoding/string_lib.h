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
#include <string>
#include <queue>
#include <vector>
#include <cuchar>
#include <cassert>
#if defined(WINDOWS_PLATFORM)
#include "windows_platform.h"
#endif

template<typename FROM_TYPE, typename TO_TYPE>
class CodingConverter {
public:
	virtual void Feed(FROM_TYPE source) = 0;
	virtual bool ContainNext() = 0;
	virtual TO_TYPE ReadNext() = 0;
	virtual void End() = 0;
	virtual ~CodingConverter() { }
};

template<typename FROM_TYPE, typename TO_TYPE>
class ConverterBase :
	public CodingConverter<FROM_TYPE, TO_TYPE> {
private:
	std::vector<FROM_TYPE> m_RawInput;
	std::queue<TO_TYPE> m_Decoded;
	size_t m_LastDecodeLeftCount;

protected:
	virtual std::size_t Decode(FROM_TYPE *input, std::size_t count) = 0;
	void Generate(TO_TYPE output) {
		m_Decoded.push(output);
	}

private:
	void DecodeInput() {
		size_t inputCount = m_RawInput.size();
		if (inputCount <= m_LastDecodeLeftCount) {
			return;
		}
		FROM_TYPE *input = m_RawInput.data();
		std::size_t consumed = Decode(input, inputCount);
		assert(consumed <= m_RawInput.size());
		m_RawInput.erase(m_RawInput.begin(), m_RawInput.begin() + consumed);
		m_LastDecodeLeftCount -= consumed;
	}

public:
	ConverterBase() :
		m_LastDecodeLeftCount(0_size) {
	}
	virtual void Feed(FROM_TYPE source) override {
		m_RawInput.push_back(source);
	}
	virtual bool ContainNext() override {
		if (!m_Decoded.empty()) {
			return true;
		}
		DecodeInput();
		return !m_Decoded.empty();
	}
	virtual TO_TYPE ReadNext() override {
		if (m_Decoded.empty()) {
			DecodeInput();
			if (m_Decoded.empty()) {
				throw NoMoreOutputException();
			}
		}
		char16_t next = m_Decoded.front();
		m_Decoded.pop();
		return next;
	}
	virtual void End() override {
		DecodeInput();
		if (m_RawInput.size() > 0) {
			throw IllegalInputFormatException();
		}
	}
};


class Utf8ToUtf16Converter :
    public ConverterBase<char, char16_t> {
private:
	mbstate_t m_DecoderState;

protected:
	virtual std::size_t Decode(char *input, std::size_t count) override;

public:
	Utf8ToUtf16Converter();
};

template<typename FROM_TYPE, typename TO_TYPE>
Array<TO_TYPE> Convert(CodingConverter<FROM_TYPE, TO_TYPE> &converter, SmartPointer<Iterator<FROM_TYPE>> source) {
	while (source->MoveNext()) {
		converter.Feed(source->GetCurrent());
	}
	converter.End();
	std::vector<char16_t> output;
	while (converter.ContainNext()) {
		output.push_back(converter.ReadNext());
	}
	StdArray<TO_TYPE> result(output.size());
	std::copy(output.begin(), output.end(), result.GetAddress());
	return result;
}

class NoMoreOutputException :
	public std::exception {
};

class IllegalInputFormatException :
	public std::exception {
};

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

	typedef Utf8ToUtf16Converter Utf8ToCharTypeEncoder;
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

template<typename FROM_TYPE, typename TO_TYPE>
class ConverterPicker{
};

template<>
class ConverterPicker<StdCharTraits<char>, StdCharTraits<char16_t>> {
public:
	typedef Utf8ToUtf16Converter CONVERTER_TYPE;
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
	TString(const Array<CharType> charArray, bool needCopy) :
		m_CharArray(charArray) {
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
	inline size_t GetLength() const {
		return m_CharArray.GetLength();
	}
	CharType operator [](size_t index) const {
		return m_CharArray.GetElementAt(index);
	}

	virtual SmartPointer<Iterator<CharType>> GetIterator() override {
		return m_CharArray.GetIterator();
	}

	template<typename FROM_CHAR_TRAITS>
	static TString<CHAR_TRAITS> From(const typename FROM_CHAR_TRAITS::CHAR_TYPE *chars, size_t length) {
		Converter<FROM_CHAR_TRAITS, CHAR_TRAITS> converter;
		Scoped<CArrayIterator<typename FROM_CHAR_TRAITS::CHAR_TYPE>> stringIterator(chars, length);
		Array<CharType> converted = Convert(converter, (&stringIterator).Cast<Iterator<typename FROM_CHAR_TRAITS::CHAR_TYPE>>());
		TString<CHAR_TRAITS> convertedString(converted, false);
		return convertedString;
	}
	template<typename FROM_CHAR_TRAITS>
	static TString<CHAR_TRAITS> From(const typename FROM_CHAR_TRAITS::CHAR_TYPE *cStyleString);
	template<typename FROM_CHAR_TRAITS>
	static TString<CHAR_TRAITS> From(const Array<typename FROM_CHAR_TRAITS::CHAR_TYPE> charArray);
	template<typename TO_CHAR_TRAITS>
	TString<TO_CHAR_TRAITS> ToString();
	template<typename TO_CHAR_TRAITS>
	Array<typename TO_CHAR_TRAITS::CHAR_TYPE> ToArray();

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

typedef TString<StdCharTraits<char16_t>> String;

String operator ""_s(const char16_t *chars, std::size_t length);
TString<StdCharTraits<char>> operator ""_s(const char *chars, std::size_t length);
TString<StdCharTraits<char32_t>> operator ""_s(const char32_t *chars, std::size_t length);

#define _S(content) u##content##_s

#endif /* PROJECT_INCLUDE_STRING_AND_ENCODING_STRING_LIB_H_ */
