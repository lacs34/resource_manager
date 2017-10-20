/*
* coding_converter_interface.h
*
*  Created on: 2016Äê3ÔÂ25ÈÕ
*      Author: ÌÎ
*/

#ifndef CODING_CONVERTER_INTERFACE_H_
#define CODING_CONVERTER_INTERFACE_H_

template<typename FROM_TYPE, typename TO_TYPE>
class CodingConverter {
public:
	virtual void Feed(FROM_TYPE source) = 0;
	virtual bool ContainNext() = 0;
	virtual TO_TYPE ReadNext() = 0;
	virtual void End() = 0;
	virtual ~CodingConverter() { }
};

#endif