/*
 * exceptions.h
 *
 *  Created on: 2016Äê3ÔÂ20ÈÕ
 *      Author: Administrator
 */

#ifndef PROJECT_INCLUDE_EXCEPTIONS_H_
#define PROJECT_INCLUDE_EXCEPTIONS_H_

#include "common_exceptions.h"

class PosixException :
	public std::exception {
private:
	int m_ErrorCode;

public:
	PosixException(int errorCode) :
		m_ErrorCode(errorCode) {
	}
};

#if defined(WINDOWS_PLATFORM)

#include "windows_platform.h"
#include "string_and_encoding/string_lib.h"

class Win32Exception {
private:
	DWORD m_Error;

public:
	Win32Exception(DWORD err) :
		m_Error(err) {
	}

	String GetErrorMessage();
};

void ThrowLastErrorException();

#endif

#endif /* PROJECT_INCLUDE_EXCEPTIONS_H_ */
