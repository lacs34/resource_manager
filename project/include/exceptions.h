/*
 * exceptions.h
 *
 *  Created on: 2016Äê3ÔÂ20ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_EXCEPTIONS_H_
#define PROJECT_INCLUDE_EXCEPTIONS_H_

#include <exception>

class PosixException :
	public std::exception {
private:
	int m_ErrorCode;

public:
	PosixException(int errorCode) :
		m_ErrorCode(errorCode) {
	}
};

#endif /* PROJECT_INCLUDE_EXCEPTIONS_H_ */
