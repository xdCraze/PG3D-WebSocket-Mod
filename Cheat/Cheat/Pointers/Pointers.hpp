#pragma once

#include <Proplamatic/IL2CPP.hpp>

#include "../../Helpers/Helpers.hpp"

using namespace IL2CPP;

namespace Dummy
{
	extern Object* WebSocketManagerInstance;
}

namespace Pointers
{
	namespace Json
	{
		extern Wrapper::Method<Object* (String*)> Decode;
		extern Wrapper::Method<String* (Object*)> Encode;

		void Initialize();
	}

	namespace Application
	{
		extern Wrapper::Method<String*()> get_version;

		void Initialize();
	}

	void Load();
}