/*
* coding_converters.h
*
*  Created on: 2016Äê3ÔÂ25ÈÕ
*      Author: ÌÎ
*/

#ifndef UNICODE_CONVERTERS_H_
#define UNICODE_CONVERTERS_H_

#include "string_and_encoding/coding_converter_interface.h"
#include "predefined_array.h"
#include "common_definition.h"
#include <cstddef>
#include <vector>
#include <queue>
#include <cassert>
#include <codecvt>
#include <cuchar>
#include <cwchar>

template<typename FROM_TYPE, typename TO_TYPE>
class ConverterBase :
	public CodingConverter<FROM_TYPE, TO_TYPE> {
private:
	std::vector<FROM_TYPE> m_RawInput;
	std::queue<TO_TYPE> m_Decoded;
	std::size_t m_LastDecodeLeftCount;

protected:
	virtual std::size_t Decode(const FROM_TYPE *input, std::size_t count) = 0;
	void Generate(TO_TYPE output) {
		m_Decoded.push(output);
	}

private:
	void DecodeInput() {
		std::size_t inputCount = m_RawInput.size();
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

template<typename TO_TYPE, typename STD_METHOD_CONTAINER>
class StdBasedFromUtf8Converter :
	public ConverterBase<char, TO_TYPE> {
private:
	std::mbstate_t m_DecoderState;

protected:
	virtual std::size_t Decode(const char *input, std::size_t count) override {
		std::size_t leftInputCount = count;
		TO_TYPE output;
		bool notFinished = true;
		while (leftInputCount > 0 && notFinished)
		{
			std::size_t consumedInput = STD_METHOD_CONTAINER::StdConvertMethod(&output, input, leftInputCount, &m_DecoderState);
			switch (consumedInput) {
			case std::size_t(-3):
				Generate(output);
				break;
			case std::size_t(-2):
				notFinished = false;
				break;
			case std::size_t(-1):
				throw IllegalInputFormatException();
			case 0_size:
				Generate(output);
				++input;
				--leftInputCount;
				break;
			default:
				Generate(output);
				input += consumedInput;
				leftInputCount -= consumedInput;
				break;
			}
		}
		return count - leftInputCount;
	}

public:
	StdBasedFromUtf8Converter() {
		memset(reinterpret_cast<void*>(&m_DecoderState), 0, sizeof(m_DecoderState));
	}
};

template<typename CODE_CONVERTER_TYPE, typename FROM_TYPE, typename TO_TYPE, typename STD_METHOD_CONTAINER>
class CodeConverterBasedConverter :
	public ConverterBase<FROM_TYPE, TO_TYPE> {
private:
	typedef typename CODE_CONVERTER_TYPE::result CONVERT_RESULT;
	std::mbstate_t m_DecoderState;
	CODE_CONVERTER_TYPE m_CodeConverter;
	static constexpr std::size_t OUTPUT_BUFFER_SIZE = 16_size;

protected:
	virtual std::size_t Decode(const FROM_TYPE *input, std::size_t count) override {
		const FROM_TYPE *inputEnd = input + count;
		TO_TYPE outputBuffer[OUTPUT_BUFFER_SIZE];
		TO_TYPE *outputEnd = outputBuffer + OUTPUT_BUFFER_SIZE;
		const FROM_TYPE *currentInput = input;
		while (currentInput != inputEnd) {
			const FROM_TYPE *newInputPlace = nullptr;
			TO_TYPE *newOutputPlace = nullptr;
			CONVERT_RESULT result =
				STD_METHOD_CONTAINER::StdConvert(m_CodeConverter, m_DecoderState, currentInput, inputEnd, newInputPlace, outputBuffer, outputEnd, newOutputPlace);
			switch (result) {
			case CODE_CONVERTER_TYPE::ok:
			case CODE_CONVERTER_TYPE::partial:
				for (TO_TYPE *currentOutput = outputBuffer; currentOutput < newOutputPlace; ++currentOutput) {
					Generate(*currentOutput);
				}
				currentInput = newInputPlace;
				break;
			case CODE_CONVERTER_TYPE::error:
			case CODE_CONVERTER_TYPE::noconv:
				throw IllegalInputFormatException();
			}
		}
		return currentInput - input;
	}

public:
	CodeConverterBasedConverter() {
		memset(reinterpret_cast<void*>(&m_DecoderState), 0, sizeof(m_DecoderState));
	}
};

template<typename CODE_CONVERTER_TYPE>
class CodeConverterInConverter :
	public CodeConverterBasedConverter<CODE_CONVERTER_TYPE, typename CODE_CONVERTER_TYPE::extern_type, typename CODE_CONVERTER_TYPE::intern_type, CodeConverterInConverter<CODE_CONVERTER_TYPE>> {
private:
	typedef typename CODE_CONVERTER_TYPE::extern_type FROM_TYPE;
	typedef typename CODE_CONVERTER_TYPE::intern_type TO_TYPE;

public:
	static inline typename CODE_CONVERTER_TYPE::result StdConvert(CODE_CONVERTER_TYPE &converter,
		std::mbstate_t& state,
		const FROM_TYPE *from,
		const FROM_TYPE *fromEnd,
		const FROM_TYPE *&fromNext,
		TO_TYPE *to,
		TO_TYPE *toEnd,
		TO_TYPE *&toNext) {
		return converter.in(state, from, fromEnd, fromNext, to, toEnd, toNext);
	}
};

template<typename CODE_CONVERTER_TYPE>
class CodeConverterOutConverter :
	public CodeConverterBasedConverter<CODE_CONVERTER_TYPE, typename CODE_CONVERTER_TYPE::intern_type, typename CODE_CONVERTER_TYPE::extern_type, CodeConverterInConverter<CODE_CONVERTER_TYPE>> {
private:
	typedef typename CODE_CONVERTER_TYPE::intern_type FROM_TYPE;
	typedef typename CODE_CONVERTER_TYPE::extern_type TO_TYPE;

public:
	static inline typename CODE_CONVERTER_TYPE::result StdConvert(CODE_CONVERTER_TYPE &converter,
		std::mbstate_t& state,
		const FROM_TYPE *from,
		const FROM_TYPE *fromEnd,
		const FROM_TYPE *&fromNext,
		TO_TYPE *to,
		TO_TYPE *toEnd,
		TO_TYPE *&toNext) {
		return converter.out(state, from, fromEnd, fromNext, to, toEnd, toNext);
	}
};

typedef CodeConverterInConverter<std::codecvt_utf8_utf16<char16_t>> Utf8ToUtf16Converter;
typedef CodeConverterOutConverter<std::codecvt_utf8_utf16<char16_t>> Utf16ToUtf8Converter;

//
//class Utf8ToUtf32Converter :
//	public StdBasedFromUtf8Converter<char16_t, Utf8ToUtf32Converter> {
//public:
//	static inline std::size_t StdConvertMethod(char32_t* pc32, const char* s, std::size_t n, std::mbstate_t* ps) {
//		return std::mbrtoc32(pc32, s, n, ps);
//	}
//};
//
//class Utf8ToWideStringConverter :
//	public StdBasedFromUtf8Converter<wchar_t, Utf8ToWideStringConverter> {
//public:
//	static inline std::size_t StdConvertMethod(wchar_t *pwc, const char *s, size_t n, std::mbstate_t *ps) {
//		return std::mbrtowc(pwc, s, n, ps);
//	}
//};
//
//template<typename FROM_TYPE, typename STD_METHOD_CONTAINER>
//class StdBasedToUtf8Converter :
//	public ConverterBase<FROM_TYPE, char> {
//private:
//	std::mbstate_t m_DecoderState;
//
//protected:
//	virtual std::size_t Decode(const FROM_TYPE *input, std::size_t count) override {
//		char output[MB_CUR_MAX];
//		FROM_TYPE *inputEnd = input + count,
//			currentInput = input;
//		while (currentInput < inputEnd)
//		{
//			std::size_t generatedOutput = STD_METHOD_CONTAINER::StdConvertMethod(output, *currentInput, &m_DecoderState);
//			switch (generatedOutput) {
//			case static_cast<std::size_t>(-1) :
//				throw IllegalInputFormatException();
//			default:
//				for (std::size_t index = 0_size; index < generatedOutput; ++index) {
//					Generate(output[index]);
//				}
//				++currentInput;
//				break;
//			}
//		}
//		return currentInput - input;
//	}
//
//public:
//	StdBasedToUtf8Converter() {
//		memset(reinterpret_cast<void*>(&m_DecoderState), 0, sizeof(m_DecoderState));
//	}
//};
//
//class Utf16ToUtf8Converter :
//	public StdBasedToUtf8Converter<char16_t, Utf16ToUtf8Converter> {
//public:
//	static inline std::size_t StdConvertMethod(char* s, char16_t c16, std::mbstate_t* ps) {
//		return std::c16rtomb(s, c16, ps);
//	}
//};
//
//class Utf32ToUtf8Converter :
//	public StdBasedToUtf8Converter<char16_t, Utf32ToUtf8Converter> {
//public:
//	static inline std::size_t StdConvertMethod(char* s, char16_t c32, std::mbstate_t* ps) {
//		return std::c32rtomb(s, c32, ps);
//	}
//};
//
//class WideStringToUtf8Converter :
//	public StdBasedToUtf8Converter<char16_t, WideStringToUtf8Converter> {
//public:
//	static inline std::size_t StdConvertMethod(char* s, char16_t wc, std::mbstate_t* ps) {
//		return std::wcrtomb(s, wc, ps);
//	}
//};

template<typename FROM_TYPE, typename TO_TYPE>
Buffer<TO_TYPE> Convert(CodingConverter<FROM_TYPE, TO_TYPE> &converter, SmartPointer<Iterator<FROM_TYPE>> source) {
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

#endif