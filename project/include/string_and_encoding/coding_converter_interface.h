/*
* coding_converter_interface.h
*
*  Created on: 2016Äê3ÔÂ25ÈÕ
*      Author: ÌÎ
*/

#ifndef CODING_CONVERTER_INTERFACE_H_
#define CODING_CONVERTER_INTERFACE_H_

#include <cstddef>
#include <cassert>
#include "basic_types.h"

template<typename FROM_TYPE, typename TO_TYPE>
class CodingConverter {
public:
	//virtual ConvertResult TryConvert(const Buffer<FROM_TYPE> sourceBuffer, Buffer<TO_TYPE> destBuffer, bool noMore) = 0;
	virtual void Feed(FROM_TYPE source) = 0;
	virtual bool ContainNext() = 0;
	virtual TO_TYPE ReadNext() = 0;
	virtual void End() = 0;
	virtual ~CodingConverter() { }
};

class ConvertResult {
private:
	bool m_Succeed;
	union ResultDetail {
		ResultDetail(std::size_t errorIndex) :
			m_ErrorIndex(errorIndex) {
		}
		ResultDetail(std::size_t sourceUsed, std::size_t destUsed, bool notFinished) :
			m_SucceedResult(sourceUsed, destUsed, notFinished) {
		}
		struct SucceedDetails {
			SucceedDetails(std::size_t sourceUsed, std::size_t destUsed, bool notFinished) :
				m_ElementUsedInSource(sourceUsed), m_ElementUsedInDest(destUsed), m_NotFinished(notFinished) {
			}
			std::size_t m_ElementUsedInSource;
			std::size_t m_ElementUsedInDest;
			bool m_NotFinished;
		} m_SucceedResult;
		std::size_t m_ErrorIndex;
	} m_Details;

public:
	ConvertResult(std::size_t errorIndex) :
	    m_Succeed(false), m_Details(errorIndex) {
	}
	ConvertResult(std::size_t sourceUsed, std::size_t destUsed, bool notFinished) :
		m_Succeed(true), m_Details(sourceUsed, destUsed, notFinished) {
	}
	ConvertResult(const ConvertResult &result) :
		m_Succeed(result.m_Succeed), m_Details(result.m_Details) {
	}

	std::size_t GetErrorIndex() {
		assert(!m_Succeed);
		return m_Details.m_ErrorIndex;
	}

	std::size_t GetElementUsedInSource() {
		assert(m_Succeed);
		return m_Details.m_SucceedResult.m_ElementUsedInSource;
	}
	std::size_t GetElementUsedInDest() {
		assert(m_Succeed);
		return m_Details.m_SucceedResult.m_ElementUsedInDest;
	}
	bool GetNotFinished() {
		assert(m_Succeed);
		return m_Details.m_SucceedResult.m_NotFinished;
	}
};

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
};

class Utf16ToUtf8Converter :
	public CodingConverter<char16_t, char> {
private:
	enum MoveResult {
		MOVE_SUCCEED,
		MOVE_FAILED,
		MOVE_TO_END
	};
	class Utf16DataDecoder {
	private:
		static constexpr char16_t prefixStart = u'\xD800';
		static constexpr char16_t prefixEnd = u'\xDBFF';
		static constexpr char16_t surfixStart = u'\xDC00';
		static constexpr char16_t surfixEnd = u'\xDFFF';
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
		const char16_t* GetCurrentPosition(const Buffer<char16_t> sourceBuffer) {
			return m_CurrentPosition;
		}
		const MoveResult Move() {
			switch (m_State) {
			case NEW_CHAR_BEGIN: {
				if (m_CurrentPosition >= m_EndPosition) {
					return MOVE_TO_END;
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
					return MOVE_TO_END;
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
	Utf16DataDecoder m_Decoder;

public:
	virtual ConvertResult TryConvert(const Buffer<char16_t> sourceBuffer, Buffer<char> destBuffer, bool noMore) {
		m_Decoder.ResetBuffer(sourceBuffer);
		MoveResult result = m_Decoder.Move();
		while (result == MOVE_SUCCEED) {

		}
	}
};

#endif