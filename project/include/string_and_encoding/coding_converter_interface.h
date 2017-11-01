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
#include "arrays.h"

enum class ConvertFurtherAction {
	NEED_MORE_INPUT,
	NEED_MORE_OUTPUT_BUFFER,
	COMPLETE
};

class ConvertResult {
private:
	bool m_Succeed;
	union {
		struct {
			std::size_t m_ElementUsedInSource;
			std::size_t m_ElementUsedInDest;
			ConvertFurtherAction m_FurtherAction;
		};
		std::size_t m_ErrorIndex;
	};

	void CopyDetails(bool succeed, const ConvertResult &result) {
		if (succeed) {
			m_ElementUsedInSource = result.m_ElementUsedInSource;
			m_ElementUsedInDest = result.m_ElementUsedInDest;
			m_FurtherAction = result.m_FurtherAction;
		}
		else {
			m_ErrorIndex = result.m_ErrorIndex;
		}
	}

public:
	ConvertResult(std::size_t errorIndex) :
	    m_Succeed(false), m_ErrorIndex(errorIndex) {
	}
	ConvertResult(std::size_t sourceUsed, std::size_t destUsed, ConvertFurtherAction furtherAction) :
		m_Succeed(true), m_ElementUsedInSource(sourceUsed), m_ElementUsedInDest(destUsed), m_FurtherAction(furtherAction) {
	}
	ConvertResult(const ConvertResult &result) :
		m_Succeed(result.m_Succeed) {
		CopyDetails(m_Succeed, result);
	}

	ConvertResult& operator =(const ConvertResult &result) {
		m_Succeed = result.m_Succeed;
		CopyDetails(m_Succeed, result);
	}

	std::size_t GetErrorIndex() {
		assert(!m_Succeed);
		return m_ErrorIndex;
	}

	std::size_t GetElementUsedInSource() {
		assert(m_Succeed);
		return m_ElementUsedInSource;
	}
	std::size_t GetElementUsedInDest() {
		assert(m_Succeed);
		return m_ElementUsedInDest;
	}
	ConvertFurtherAction GetFurtherAction() {
		assert(m_Succeed);
		return m_FurtherAction;
	}
};

template<typename FROM_TYPE, typename TO_TYPE>
class CodingConverter {
public:
	virtual ConvertResult TryConvert(const Buffer<FROM_TYPE> sourceBuffer, Buffer<TO_TYPE> destBuffer) = 0;
	virtual ~CodingConverter() { }
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

#endif