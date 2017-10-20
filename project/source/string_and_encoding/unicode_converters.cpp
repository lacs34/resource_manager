#include "string_and_encoding/unicode_converters.h"
#include "exceptions.h"
#include <cuchar>

String operator ""_s(const char16_t *chars, std::size_t length) {
	return String(chars, length);
}

TString<StdCharTraits<char>> operator ""_s(const char *chars, std::size_t length) {
	return TString<StdCharTraits<char>>(chars, length);
}

std::size_t Utf8ToUtf16Converter::Decode(char *input, std::size_t count) {
	std::size_t leftInputCount = count;
	char16_t output;
	bool notFinished = true;
	while (leftInputCount > 0 && notFinished)
	{
		std::size_t consumedInput = std::mbrtoc16(&output, input, leftInputCount, &m_DecoderState);
		switch (consumedInput) {
		case std::size_t(-3):
			Generate(output);
			break;
		case std::size_t(-2):
			notFinished = false;
			break;
		case std::size_t(-1):
			throw IllegalInputFormatException();
		case 0_size:
			Generate(output);
			++input;
			--leftInputCount;
			break;
		default:
			Generate(output);
			input += consumedInput;
			leftInputCount -= consumedInput;
			break;
		}
	}
	return count - leftInputCount;
}

Utf8ToUtf16Converter::Utf8ToUtf16Converter() {
	memset(reinterpret_cast<void*>(&m_DecoderState), 0, sizeof(m_DecoderState));
}