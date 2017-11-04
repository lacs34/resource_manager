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

enum class ConvertResult {
	NEED_MORE_INPUT,
	NEED_MORE_OUTPUT_BUFFER,
	COMPLETE,
	FAILED
};

struct ConvertDetail {
	std::size_t m_ElementUsedInSource;
	std::size_t m_ElementUsedInDest;
};

enum class SyncResult {
	SUCCEED,
	FAILD,
	REACH_END,
	NOT_SUPPORTED
};

template<typename CODE_TYPE>
class CodingSynchronizer {
public:
	virtual SyncResult TrySyncToNextStartFromBegin(const Buffer<CODE_TYPE> buffer, std::size_t &syncIndex) = 0;
	virtual SyncResult TrySyncToPreviousStartFromEnd(const Buffer<CODE_TYPE> buffer, std::size_t &syncIndex) = 0;
	virtual ~CodingSynchronizer() { }
};

template<typename FROM_TYPE, typename TO_TYPE>
class CodingConverter {
public:
	virtual ConvertResult TryConvert(const Buffer<FROM_TYPE> sourceBuffer, Buffer<TO_TYPE> destBuffer, ConvertDetail &detail) = 0;
	virtual ~CodingConverter() { }
};

#endif