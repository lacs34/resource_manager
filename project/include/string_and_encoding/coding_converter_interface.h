/*
* coding_converter_interface.h
*
*  Created on: 2016��3��25��
*      Author: ��
*/

#ifndef CODING_CONVERTER_INTERFACE_H_
#define CODING_CONVERTER_INTERFACE_H_

#include <cstddef>

struct ConvertResult {
	std::size_t ElementUsedInSource;
	std::size_t ElementUsedInDest;
	bool NotFinished;
	bool Fallback;
};

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

#endif