#pragma once

#include <Windows.h>
#include <string.h>
#include <vector>
#include <chrono>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iostream>
#include <random>

#include <detours.h>
#include <Proplamatic/IL2CPP.hpp>
#include "../Variables/Variables.hpp"

namespace Helpers
{
	enum class LogType 
	{
		INFO,
		SUCCESS,
		ERR,
		WARNING,
		DEBUG,
		CUSTOM
	};

	namespace Console
	{
		void Init();
	}

	std::string CurrentTime();
	void Log(std::string message, LogType type = LogType::INFO, std::string customPrefix = "");

	void MouseFix(bool menuVisible);

	namespace Hooking
	{
		template<typename Func>
		bool Create(void* target, Func* replace, Func** original);
		IL2CPP::Class* GetClass(const std::string& className, const std::string& assembly = "Assembly-CSharp.dll");
		void Attach(const std::string& className, const std::string& methodName, void* replace, void* original, const std::string& assembly = "Assembly-CSharp.dll");
		void Attach(const std::string& className, int index, void* replace, void* original, const std::string& assembly = "Assembly-CSharp.dll");
		LPVOID GetMethodPointerViaIndex(const std::string& keyname, int index, bool obfuscated, const std::string& assembly = "Assembly-CSharp.dll");
		LPVOID GetMethodPointer(const std::string& className, const std::string& methodName, const std::string& assembly = "Assembly-CSharp.dll");
	}

	std::string RandHex(short length = 4);
}