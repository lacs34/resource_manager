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

enum MoveResult {
	MOVE_SUCCEED,
	MOVE_FAILED,
	MOVE_TO_END_WHOLE_CHAR,
	MOVE_TO_END_HALF_CHAR
};

class Utf16DataEncoder {
private:
	static constexpr CodePoint::VALUE_TYPE maxSingleChar = 0x10000;
	CodePoint::VALUE_TYPE m_CodePoint;
	char16_t *m_CurrentPosition;
	char16_t *m_EndPosition;
	enum FlushState {
		SINGLE_BEGIN = 0,
		DOUBLE_BEGIN = 1,
		DOUBLE_FLUSH_FIRST,
		END
	};
	FlushState m_State;

public:
	Utf16DataEncoder() :
		m_State(END) {
	}
	void ResetBuffer(const Buffer<char16_t> &sourceBuffer) {
		m_CurrentPosition = sourceBuffer.GetAddress();
		m_EndPosition = m_CurrentPosition + sourceBuffer.GetLength();
	}
	const char16_t* GetCurrentPosition() {
		return m_CurrentPosition;
	}
	bool Flush() {
		while (m_CurrentPosition < m_EndPosition && m_State != END) {
			switch (m_State) {
			case SINGLE_BEGIN:
				*m_CurrentPosition = char16_t(m_CodePoint);
				++m_CurrentPosition;
				m_State = END;
				break;
			case DOUBLE_BEGIN:
				*m_CurrentPosition = char16_t(((m_CodePoint >> 10) & 0x3FF) | 0xD800);
				++m_CurrentPosition;
				m_State = DOUBLE_FLUSH_FIRST;
				break;
			case DOUBLE_FLUSH_FIRST:
				*m_CurrentPosition = char16_t((m_CodePoint & 0x3FF) | 0xDC00);
				++m_CurrentPosition;
				m_State = END;
				break;
			}
		}
		return m_State == END;
	}
	bool Feed(CodePoint::VALUE_TYPE codePoint) {
		m_CodePoint = codePoint;
		m_State = ((codePoint >= maxSingleChar) ? DOUBLE_BEGIN : SINGLE_BEGIN);
		if (m_CurrentPosition < m_EndPosition) {
			do {
				switch (m_State) {
				case SINGLE_BEGIN:
					*m_CurrentPosition = char16_t(m_CodePoint);
					++m_CurrentPosition;
					m_State = END;
					break;
				case DOUBLE_BEGIN:
					*m_CurrentPosition = char16_t(((m_CodePoint >> 10) & 0x3FF) | 0xD800);
					++m_CurrentPosition;
					m_State = DOUBLE_FLUSH_FIRST;
					break;
				case DOUBLE_FLUSH_FIRST:
					*m_CurrentPosition = char16_t((m_CodePoint & 0x3FF) | 0xDC00);
					++m_CurrentPosition;
					m_State = END;
					break;
				}
			} while (m_CurrentPosition < m_EndPosition && m_State != END);
		}
		return m_State == END;
	}
};

class Utf16DataDecoder {
private:
	static constexpr char16_t prefixStart = u'\xD800';
	static constexpr char16_t prefixEnd = u'\xDBFF';
	static constexpr char16_t surfixStart = u'\xDC00';
	static constexpr char16_t surfixEnd = u'\xDFFF';
	static constexpr CodePoint::VALUE_TYPE offset = 0x010000;
	static inline bool NoPrefix(char16_t current) {
		return current < prefixStart || current > surfixEnd;
	}
	static inline bool IsPrefix(char16_t current) {
		return current >= prefixStart && current <= prefixEnd;
	}
	static inline bool IsSurfix(char16_t current) {
		return current >= surfixStart && current <= surfixEnd;
	}
	const char16_t *m_CurrentPosition;
	const char16_t *m_EndPosition;
	enum MoveState {
		NEW_CHAR_BEGIN = 0,
		PREFIX_READ = 1
	};
	MoveState m_State;
	CodePoint::VALUE_TYPE m_PartialCodePoint;

public:
	Utf16DataDecoder() :
		m_State(NEW_CHAR_BEGIN) {
	}
	void ResetBuffer(const Buffer<char16_t> &sourceBuffer) {
		m_CurrentPosition = sourceBuffer.GetAddress();
		m_EndPosition = m_CurrentPosition + sourceBuffer.GetLength();
	}
	const char16_t* GetCurrentPosition() {
		return m_CurrentPosition;
	}
	const MoveResult Move() {
		switch (m_State) {
		case NEW_CHAR_BEGIN: {
			if (m_CurrentPosition >= m_EndPosition) {
				return MOVE_TO_END_WHOLE_CHAR;
			}
			char16_t nextChar = *m_CurrentPosition;
			if (NoPrefix(nextChar)) {
				m_PartialCodePoint = nextChar;
			}
			else if (IsPrefix(nextChar)) {
				m_PartialCodePoint = nextChar - prefixStart;
				m_State = PREFIX_READ;
			}
			else {
				return MOVE_FAILED;
			}
			++m_CurrentPosition;
		}
		case PREFIX_READ: {
			if (m_CurrentPosition >= m_EndPosition) {
				return MOVE_TO_END_HALF_CHAR;
			}
			char16_t nextChar = *m_CurrentPosition;
			if (IsSurfix(nextChar)) {
				m_PartialCodePoint = (m_PartialCodePoint << 10) + (nextChar - surfixStart) + offset;
			}
			else {
				return MOVE_FAILED;
			}
			++m_CurrentPosition;
			break;
		}
		}
	}
	CodePoint::VALUE_TYPE GetCodePoint() {
		return m_PartialCodePoint;
	}
};

class Utf8DataEncoder {
private:
	static constexpr std::size_t BYTE_1_END = 0x7F;
	static constexpr std::size_t BYTE_2_END = 0x7FF;
	static constexpr std::size_t BYTE_3_END = 0xFFFF;
	static constexpr std::size_t BYTE_4_END = 0x10FFFF;
	CodePoint::VALUE_TYPE m_CodePoint;
	std::size_t m_LeftBytes;
	char *m_CurrentPosition;
	char *m_EndPosition;

public:
	Utf8DataEncoder() :
		m_LeftBytes(0) {
	}
	void ResetBuffer(const Buffer<char> &sourceBuffer) {
		m_CurrentPosition = sourceBuffer.GetAddress();
		m_EndPosition = m_CurrentPosition + sourceBuffer.GetLength();
	}
	const char* GetCurrentPosition() {
		return m_CurrentPosition;
	}
	bool Flush() {
		while (m_CurrentPosition < m_EndPosition && m_LeftBytes > 0)
		{
			*m_CurrentPosition = char(m_CodePoint >> (sizeof(CodePoint::VALUE_TYPE) * 8 - 8));
			++m_CurrentPosition;
			--m_LeftBytes;
			m_CodePoint <<= 6;
			m_CodePoint &= ~(3 << (sizeof(CodePoint::VALUE_TYPE) * 8 - 2));
			m_CodePoint |= (2 << (sizeof(CodePoint::VALUE_TYPE) * 8 - 2));
		}
		return m_LeftBytes == 0;
	}
	bool Feed(CodePoint::VALUE_TYPE codePoint) {
		m_CodePoint = codePoint;
		if (codePoint <= BYTE_1_END) {
			m_LeftBytes = 1;
			m_CodePoint <<= (sizeof(CodePoint::VALUE_TYPE) * 8 - 8);
		}
		else if (codePoint <= BYTE_2_END) {
			m_LeftBytes = 2;
			m_CodePoint |= 0x3000;
			m_CodePoint <<= (sizeof(CodePoint::VALUE_TYPE) * 8 - 14);
		}
		else if (codePoint <= BYTE_3_END) {
			m_LeftBytes = 3;
			m_CodePoint |= 0xE0000;
			m_CodePoint <<= (sizeof(CodePoint::VALUE_TYPE) * 8 - 20);
		}
		else {
			m_LeftBytes = 4;
			m_CodePoint |= 0x3C00000;
			m_CodePoint <<= (sizeof(CodePoint::VALUE_TYPE) * 8 - 26);
		}
		if (m_CurrentPosition < m_EndPosition) {
			do {
				*m_CurrentPosition = char(m_CodePoint >> (sizeof(CodePoint::VALUE_TYPE) * 8 - 8));
				++m_CurrentPosition;
				--m_LeftBytes;
				m_CodePoint <<= 6;
				m_CodePoint &= ~(3 << (sizeof(CodePoint::VALUE_TYPE) * 8 - 2));
				m_CodePoint |= (2 << (sizeof(CodePoint::VALUE_TYPE) * 8 - 2));
			} while (m_CurrentPosition < m_EndPosition && m_LeftBytes > 0);
		}
		return m_LeftBytes == 0;
	}
};

class Utf8DataDecoder {
private:
	const char *m_CurrentPosition;
	const char *m_EndPosition;
	CodePoint::VALUE_TYPE m_PartialCodePoint;
	std::size_t m_LeftCharCount;

public:
	Utf8DataDecoder() :
		m_LeftCharCount(0) {
	}
	void ResetBuffer(const Buffer<char> &sourceBuffer) {
		m_CurrentPosition = sourceBuffer.GetAddress();
		m_EndPosition = m_CurrentPosition + sourceBuffer.GetLength();
	}
	const char* GetCurrentPosition() {
		return m_CurrentPosition;
	}
	const MoveResult Move() {
		if (m_LeftCharCount == 0) {
			if (m_CurrentPosition < m_EndPosition) {
				return MOVE_TO_END_WHOLE_CHAR;
			}

			char next = *m_CurrentPosition;
			if (!(next & 0x80)) {
				m_PartialCodePoint = CodePoint::VALUE_TYPE(next);
				return MOVE_SUCCEED;
			}
			else if ((next & 0xE0) == 0xC0) {
				m_PartialCodePoint = CodePoint::VALUE_TYPE(next & 0x1F);
				m_LeftCharCount = 1;
			}
			else if ((next & 0xF0) == 0xE0) {
				m_PartialCodePoint = CodePoint::VALUE_TYPE(next & 0xF);
				m_LeftCharCount = 2;
			}
			else if ((next & 0xF8) == 0xF0) {
				m_PartialCodePoint = CodePoint::VALUE_TYPE(next & 0x7);
				m_LeftCharCount = 3;
			}
			else {
				return MOVE_FAILED;
			}
			++m_CurrentPosition;
		}
		if (m_CurrentPosition < m_EndPosition) {
			do {
				char next = *m_CurrentPosition;
				if ((next & 0xC0) != 0x80) {
					return MOVE_FAILED;
				}
				m_PartialCodePoint = (m_PartialCodePoint << 6) | (next & 0x3F);
			} while (m_CurrentPosition < m_EndPosition && m_LeftCharCount > 0);
		}
		if (m_LeftCharCount == 0) {
			if (m_PartialCodePoint < 0xD800 || (m_PartialCodePoint >= 0xE000 && m_PartialCodePoint <= 0x10FFFF)) {
				return MOVE_SUCCEED;
			}
			else {
				return MOVE_FAILED;
			}
		}
		else {
			return MOVE_TO_END_HALF_CHAR;
		}
	}
	CodePoint::VALUE_TYPE GetCodePoint() {
		return m_PartialCodePoint;
	}
};

class Utf16ToUtf8Converter :
	public CodingConverter<char16_t, char> {
private:
	Utf16DataDecoder m_Decoder;
	Utf8DataEncoder m_Encoder;

public:
	virtual ConvertResult TryConvert(const Buffer<char16_t> sourceBuffer, Buffer<char> destBuffer) {
		m_Decoder.ResetBuffer(sourceBuffer);
		m_Encoder.ResetBuffer(destBuffer);
		bool notReachEnd = m_Encoder.Flush();
		while (notReachEnd) {
			MoveResult result = m_Decoder.Move();
			switch (result) {
			case MOVE_FAILED: {
				ConvertResult returnResult(m_Decoder.GetCurrentPosition() - sourceBuffer.GetAddress());
				return returnResult;
			}
			case MOVE_TO_END_WHOLE_CHAR: {
				ConvertResult returnResult(m_Decoder.GetCurrentPosition() - sourceBuffer.GetAddress(), m_Encoder.GetCurrentPosition() - destBuffer.GetAddress(), ConvertFurtherAction::COMPLETE);
				return returnResult;
			}
			case MOVE_TO_END_HALF_CHAR: {
				ConvertResult returnResult(m_Decoder.GetCurrentPosition() - sourceBuffer.GetAddress(), m_Encoder.GetCurrentPosition() - destBuffer.GetAddress(), ConvertFurtherAction::NEED_MORE_INPUT);
				return returnResult;
			}
			default:
				notReachEnd = m_Encoder.Feed(m_Decoder.GetCodePoint());
			}
		}
		ConvertResult returnResult(m_Decoder.GetCurrentPosition() - sourceBuffer.GetAddress(), m_Encoder.GetCurrentPosition() - destBuffer.GetAddress(), ConvertFurtherAction::NEED_MORE_OUTPUT_BUFFER);
		return returnResult;
	}
};

class Utf8ToUtf16Converter :
	public CodingConverter<char, char16_t> {
private:
	Utf8DataDecoder m_Decoder;
	Utf16DataEncoder m_Encoder;

public:
	virtual ConvertResult TryConvert(const Buffer<char> sourceBuffer, Buffer<char16_t> destBuffer) {
		m_Decoder.ResetBuffer(sourceBuffer);
		m_Encoder.ResetBuffer(destBuffer);
		bool notReachEnd = m_Encoder.Flush();
		while (notReachEnd) {
			MoveResult result = m_Decoder.Move();
			switch (result) {
			case MOVE_FAILED: {
				ConvertResult returnResult(m_Decoder.GetCurrentPosition() - sourceBuffer.GetAddress());
				return returnResult;
			}
			case MOVE_TO_END_WHOLE_CHAR: {
				ConvertResult returnResult(m_Decoder.GetCurrentPosition() - sourceBuffer.GetAddress(), m_Encoder.GetCurrentPosition() - destBuffer.GetAddress(), ConvertFurtherAction::COMPLETE);
				return returnResult;
			}
			case MOVE_TO_END_HALF_CHAR: {
				ConvertResult returnResult(m_Decoder.GetCurrentPosition() - sourceBuffer.GetAddress(), m_Encoder.GetCurrentPosition() - destBuffer.GetAddress(), ConvertFurtherAction::NEED_MORE_INPUT);
				return returnResult;
			}
			default:
				notReachEnd = m_Encoder.Feed(m_Decoder.GetCodePoint());
			}
		}
		ConvertResult returnResult(m_Decoder.GetCurrentPosition() - sourceBuffer.GetAddress(), m_Encoder.GetCurrentPosition() - destBuffer.GetAddress(), ConvertFurtherAction::NEED_MORE_OUTPUT_BUFFER);
		return returnResult;
	}
};

template<typename FROM_TYPE, typename TO_TYPE>
Buffer<TO_TYPE> Convert(CodingConverter<FROM_TYPE, TO_TYPE> &converter, SmartPointer<Iterator<FROM_TYPE>> source) {
	StdArray<TO_TYPE> result(10);
	return result;
}

#endif