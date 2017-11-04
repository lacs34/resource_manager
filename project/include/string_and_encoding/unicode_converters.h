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

enum EncodeResult {
	ENCODE_SUCCEED,
	UNABLE_TO_ENCODE,
	ENCODE_TO_END
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
	inline void ResetBuffer(const Buffer<char16_t> &sourceBuffer) {
		m_CurrentPosition = sourceBuffer.GetAddress();
		m_EndPosition = m_CurrentPosition + sourceBuffer.GetLength();
	}
	inline const char16_t* GetCurrentPosition() {
		return m_CurrentPosition;
	}
	EncodeResult Flush() {
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
		return m_State == END ? ENCODE_SUCCEED : ENCODE_TO_END;
	}
	EncodeResult Feed(CodePoint::VALUE_TYPE codePoint) {
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
		return m_State == END ? ENCODE_SUCCEED : ENCODE_TO_END;
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
	const char16_t* GetEndPosition() {
		return m_EndPosition;
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
	SyncResult SyncForward() {
		while (m_CurrentPosition < m_EndPosition) {
			char16_t current = *m_CurrentPosition;
			switch (m_State) {
			case NEW_CHAR_BEGIN:
				if (current < surfixStart || current > surfixEnd) {
					return SyncResult::SUCCEED;
				}
				++m_CurrentPosition;
				m_State = PREFIX_READ;
				break;
			case PREFIX_READ:
				if (current < surfixStart || current > surfixEnd) {
					return SyncResult::SUCCEED;
				}
				return SyncResult::FAILD;
			}
		}
		return SyncResult::REACH_END;
	}
	SyncResult SyncBackward() {
		while (m_CurrentPosition < m_EndPosition) {
			char16_t current = *(m_EndPosition - 1);
			switch (m_State) {
			case NEW_CHAR_BEGIN:
				if (current < surfixStart || current > surfixEnd) {
					return SyncResult::SUCCEED;
				}
				--m_EndPosition;
				m_State = PREFIX_READ;
				break;
			case PREFIX_READ:
				if (current < surfixStart || current > surfixEnd) {
					return SyncResult::SUCCEED;
				}
				return SyncResult::FAILD;
			}
		}
		return SyncResult::REACH_END;
	}
};

class Utf16Synchronizer :
	public CodingSynchronizer<char16_t> {
private:
	Utf16DataDecoder m_Decoder;

public:
	virtual SyncResult TrySyncToNextStartFromBegin(const Buffer<char16_t> buffer, std::size_t &syncIndex) {
		m_Decoder.ResetBuffer(buffer);
		SyncResult result = m_Decoder.SyncForward();
		if (result != SyncResult::REACH_END) {
			syncIndex = m_Decoder.GetCurrentPosition() - buffer.GetAddress();
		}
		return result;
	}
	virtual SyncResult TrySyncToPreviousStartFromEnd(const Buffer<char16_t> buffer, std::size_t &syncIndex) {
		m_Decoder.ResetBuffer(buffer);
		SyncResult result = m_Decoder.SyncBackward();
		if (result != SyncResult::REACH_END) {
			syncIndex = (m_Decoder.GetEndPosition() - buffer.GetAddress()) - 1;
		}
		return result;
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
	EncodeResult Flush() {
		while (m_CurrentPosition < m_EndPosition && m_LeftBytes > 0)
		{
			*m_CurrentPosition = char(m_CodePoint >> (sizeof(CodePoint::VALUE_TYPE) * 8 - 8));
			++m_CurrentPosition;
			--m_LeftBytes;
			m_CodePoint <<= 6;
			m_CodePoint &= ~(3 << (sizeof(CodePoint::VALUE_TYPE) * 8 - 2));
			m_CodePoint |= (2 << (sizeof(CodePoint::VALUE_TYPE) * 8 - 2));
		}
		return m_LeftBytes == 0 ? ENCODE_SUCCEED : ENCODE_TO_END;
	}
	EncodeResult Feed(CodePoint::VALUE_TYPE codePoint) {
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
		return m_LeftBytes == 0 ? ENCODE_SUCCEED : ENCODE_TO_END;
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
				if ((next & 0xFE) == 0xC0) {
					return MOVE_FAILED;
				}
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

class Utf32DataEncoder {
private:
	static constexpr CodePoint::VALUE_TYPE invalid = 0x110000;
	CodePoint::VALUE_TYPE m_CodePoint;
	char32_t *m_CurrentPosition;
	char32_t *m_EndPosition;

public:
	Utf32DataEncoder() :
		m_CodePoint(invalid) {
	}
	void ResetBuffer(const Buffer<char32_t> &sourceBuffer) {
		m_CurrentPosition = sourceBuffer.GetAddress();
		m_EndPosition = m_CurrentPosition + sourceBuffer.GetLength();
	}
	const char32_t* GetCurrentPosition() {
		return m_CurrentPosition;
	}
	EncodeResult Flush() {
		if (m_CodePoint == invalid) {
			return ENCODE_SUCCEED;
		}
		if (m_CurrentPosition < m_EndPosition) {
			*m_CurrentPosition = char32_t(m_CodePoint);
			return ENCODE_SUCCEED;
		}
		return ENCODE_TO_END;
	}
	EncodeResult Feed(CodePoint::VALUE_TYPE codePoint) {
		if (m_CurrentPosition < m_EndPosition) {
			*m_CurrentPosition = char32_t(m_CodePoint);
			return ENCODE_SUCCEED;
		}
		m_CodePoint = codePoint;
		return ENCODE_TO_END;
	}
};

class Utf32DataDecoder {
private:
	const char32_t *m_CurrentPosition;
	const char32_t *m_EndPosition;
	CodePoint::VALUE_TYPE m_CodePoint;
	std::size_t m_LeftCharCount;

public:
	Utf32DataDecoder() :
		m_LeftCharCount(0) {
	}
	void ResetBuffer(const Buffer<char32_t> &sourceBuffer) {
		m_CurrentPosition = sourceBuffer.GetAddress();
		m_EndPosition = m_CurrentPosition + sourceBuffer.GetLength();
	}
	const char32_t* GetCurrentPosition() {
		return m_CurrentPosition;
	}
	const MoveResult Move() {
		if (m_CurrentPosition < m_EndPosition) {
			m_CodePoint = *m_CurrentPosition;
			if (m_CodePoint > 0x10FFFF || (m_CodePoint >= 0xD800 && m_CodePoint <= 0xDFFF)) {
				return MOVE_FAILED;
			}
			return MOVE_SUCCEED;
		}
		return MOVE_TO_END_WHOLE_CHAR;
	}
	CodePoint::VALUE_TYPE GetCodePoint() {
		return m_CodePoint;
	}
};

class Utf16ToUtf8Converter :
	public CodingConverter<char16_t, char> {
private:
	Utf16DataDecoder m_Decoder;
	Utf8DataEncoder m_Encoder;

	inline void OutputDetail(const Buffer<char16_t> &sourceBuffer, Buffer<char> &destBuffer, ConvertDetail &detail) {
		detail.m_ElementUsedInSource = m_Decoder.GetCurrentPosition() - sourceBuffer.GetAddress();
		detail.m_ElementUsedInDest = m_Encoder.GetCurrentPosition() - destBuffer.GetAddress();
	}

public:
	virtual ConvertResult TryConvert(const Buffer<char16_t> sourceBuffer, Buffer<char> destBuffer, ConvertDetail &detail) {
		m_Decoder.ResetBuffer(sourceBuffer);
		m_Encoder.ResetBuffer(destBuffer);
		EncodeResult encodeResult = m_Encoder.Flush();
		while (encodeResult == ENCODE_SUCCEED) {
			MoveResult result = m_Decoder.Move();
			switch (result) {
			case MOVE_FAILED: {
				OutputDetail(sourceBuffer, destBuffer, detail);
				return ConvertResult::FAILED;
			}
			case MOVE_TO_END_WHOLE_CHAR: {
				OutputDetail(sourceBuffer, destBuffer, detail);
				return ConvertResult::COMPLETE;
			}
			case MOVE_TO_END_HALF_CHAR: {
				OutputDetail(sourceBuffer, destBuffer, detail);
				return ConvertResult::NEED_MORE_INPUT;
			}
			default:
				encodeResult = m_Encoder.Feed(m_Decoder.GetCodePoint());
			}
		}
		if (encodeResult == UNABLE_TO_ENCODE) {
			OutputDetail(sourceBuffer, destBuffer, detail);
			return ConvertResult::FAILED;
		}
		OutputDetail(sourceBuffer, destBuffer, detail);
		return ConvertResult::NEED_MORE_OUTPUT_BUFFER;
	}
};

class Utf8ToUtf16Converter :
	public CodingConverter<char, char16_t> {
private:
	Utf8DataDecoder m_Decoder;
	Utf16DataEncoder m_Encoder;

	inline void OutputDetail(const Buffer<char> &sourceBuffer, Buffer<char16_t> &destBuffer, ConvertDetail &detail) {
		detail.m_ElementUsedInSource = m_Decoder.GetCurrentPosition() - sourceBuffer.GetAddress();
		detail.m_ElementUsedInDest = m_Encoder.GetCurrentPosition() - destBuffer.GetAddress();
	}

public:
	virtual ConvertResult TryConvert(const Buffer<char> sourceBuffer, Buffer<char16_t> destBuffer, ConvertDetail &detail) {
		m_Decoder.ResetBuffer(sourceBuffer);
		m_Encoder.ResetBuffer(destBuffer);
		EncodeResult encodeResult = m_Encoder.Flush();
		while (encodeResult == ENCODE_SUCCEED) {
			MoveResult result = m_Decoder.Move();
			switch (result) {
			case MOVE_FAILED: {
				OutputDetail(sourceBuffer, destBuffer, detail);
				return ConvertResult::FAILED;
			}
			case MOVE_TO_END_WHOLE_CHAR: {
				OutputDetail(sourceBuffer, destBuffer, detail);
				return ConvertResult::COMPLETE;
			}
			case MOVE_TO_END_HALF_CHAR: {
				OutputDetail(sourceBuffer, destBuffer, detail);
				return ConvertResult::NEED_MORE_INPUT;
			}
			default:
				encodeResult = m_Encoder.Feed(m_Decoder.GetCodePoint());
			}
		}
		if (encodeResult == UNABLE_TO_ENCODE) {
			OutputDetail(sourceBuffer, destBuffer, detail);
			return ConvertResult::FAILED;
		}
		OutputDetail(sourceBuffer, destBuffer, detail);
		return ConvertResult::NEED_MORE_OUTPUT_BUFFER;
	}
};

template<typename FROM_TYPE, typename TO_TYPE, std::size_t BUFFER_SIZE = 20>
Buffer<TO_TYPE> Convert(CodingConverter<FROM_TYPE, TO_TYPE> &converter, Buffer<FROM_TYPE> source) {
	ScopedArray<TO_TYPE, BUFFER_SIZE> staticBuffer;
	std::vector<Buffer<TO_TYPE>> outputs;
	Buffer<TO_TYPE> buffer(staticBuffer);
	ConvertDetail detail;
	ConvertResult result = converter.TryConvert(source, buffer, detail);
	while (result == ConvertResult::NEED_MORE_OUTPUT_BUFFER) {
		outputs.push_back(buffer);
		StdArray<TO_TYPE> newBuffer(BUFFER_SIZE);
		buffer = newBuffer;
		result = converter.TryConvert(source, buffer, detail);
	}
	const std::size_t outputCount = outputs.size();
	StdArray<TO_TYPE> result(BUFFER_SIZE * outputCount + detail.m_ElementUsedInDest);
	for (std::size_t i = 0; i < outputCount; ++i) {
		std::size_t bufferStart = BUFFER_SIZE * i;
		result.GetSubBuffer(bufferStart, bufferStart + BUFFER_SIZE).CopyFrom(outputs[i]);
	}
	if (detail.m_ElementUsedInDest > 0) {
		std::size_t bufferStart = BUFFER_SIZE * outputCount;
		result.GetSubBuffer(bufferStart, bufferStart + detail.m_ElementUsedInDest).CopyFrom(buffer.GetSubBuffer(0, detail.m_ElementUsedInDest));
	}
	return result;
}

#endif