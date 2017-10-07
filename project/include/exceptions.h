/*
 * exceptions.h
 *
 *  Created on: 2016��3��20��
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
