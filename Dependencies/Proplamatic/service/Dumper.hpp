#pragma once
#include "../Main.hpp"
#include "../structures/String.hpp"

namespace IL2CPP
{
	namespace Dumper
	{
		struct DumperSettings
		{
			bool generatePattern = true;
			bool generateFullnameTyping = true;
			bool mergeDumpFiles = false;
		};

		std::string DumpImage(Image* imageToDump, const DumperSettings& settings);

		void StartDumper(const std::string& dumpDirectoryPath, const DumperSettings& settings);
	}
}