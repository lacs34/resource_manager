#include <exceptions.h>

#if defined(WINDOWS_PLATFORM)

void ThrowLastErrorException() {
	DWORD lastError = GetLastError();
	Win32Exception exception(lastError);
	throw exception;
}

String Win32Exception::GetErrorMessage() {
	LPTSTR messageBuffer;
	DWORD bufferLength = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		m_Error,
		LANGID(0),
		(LPTSTR)&messageBuffer,
		0,
		NULL);
	if (bufferLength == DWORD(0)) {
		ThrowLastErrorException();
	}
	AutoPointer<TCHAR, HLocalTraits> messageAutoPointer(messageBuffer);
	String message(messageBuffer);

	return message;
}

#endif