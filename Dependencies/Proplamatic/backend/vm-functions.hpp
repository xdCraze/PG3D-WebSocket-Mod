#pragma once
#include "api-function.hpp"

namespace IL2CPP
{
	namespace VM_FUNCTIONS
	{
		namespace Class
		{
			extern bool (*Init)(IMPORT::Il2CppClass* klass);
		}

		void INIT();
	}
}