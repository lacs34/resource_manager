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
#include <cstddef>
#include <vector>
#include <queue>
#include <cassert>

template<typename FROM_TYPE, typename TO_TYPE>
class ConverterBase :
	public CodingConverter<FROM_TYPE, TO_TYPE> {
private:
	std::vector<FROM_TYPE> m_RawInput;
	std::queue<TO_TYPE> m_Decoded;
	std::size_t m_LastDecodeLeftCount;

protected:
	virtual std::size_t Decode(FROM_TYPE *input, std::size_t count) = 0;
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

#endif