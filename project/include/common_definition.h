/*
 * common_definition.h
 *
 *  Created on: 2016Äê3ÔÂ14ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_COMMON_DEFINITION_H_
#define PROJECT_INCLUDE_COMMON_DEFINITION_H_

#include <cstddef>

template<typename RETURN_TYPE, typename ...ARGS_TYPE>
using Function = RETURN_TYPE (*)(ARGS_TYPE...);

constexpr std::size_t operator "" _size(unsigned long long int value) {
	return std::size_t(value);
}

#endif /* PROJECT_INCLUDE_COMMON_DEFINITION_H_ */
