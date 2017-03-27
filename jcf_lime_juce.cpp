

#include "jcf_lime_juce.h"

namespace jcf
{
	using namespace juce;
#include "ui/jcf_font_awesome.cpp"
#include "utils/pitch.cpp"
#include "crypto/jcf_blowfish_extended.cpp"
#include "crypto/jcf_secure_credentials.cpp"
#include "utils/app_options.cpp"

	bool isValidWindowsFilename(const String& file)
	{
		if (file.containsAnyOf("<>:\"/\\|?*"))
			return false;

		for (auto t(file.getCharPointer()); !t.isEmpty();)
			if (t.getAndAdvance() < ' ')
				return false;

		auto ucFile = file.toUpperCase();

		if (String("CPANL").containsChar(ucFile[0]))
		{
			const char* reservedNames[] = {"CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};

			for (auto& r : reservedNames)
				if (ucFile == r)
					return false;
		}

		if (file.endsWithChar('.'))
			return false;

		return true;
	}

	bool isValidWindowsPathLength(const File& file)
	{
		auto p = file.getFullPathName();

		if (p.length() > 260)
			return false;

		return true;
	}
}

