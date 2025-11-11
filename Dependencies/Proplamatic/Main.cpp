#include "IL2CPP.hpp"
#include "IL2CPP_DEFS.hpp"
#include <cassert>

namespace IL2CPP
{
	static bool initialized = false;
	HMODULE handle_GameAssembly = nullptr;

	Domain* CurrentDomain()
	{
		return (Domain*) IMPORT::il2cpp_domain_get();
	}

	IL2CPPThreadHandle* AttachCurrentThread()
	{
		return (IL2CPPThreadHandle*) IMPORT::il2cpp_thread_attach(
			IMPORT::il2cpp_domain_get()
		);
	}

	void DetachCurrentThread(IL2CPPThreadHandle* handle)
	{
		assert(handle && "Handle cannot be null when detaching thread.");

		IMPORT::il2cpp_thread_detach(
			(IMPORT::Il2CppThread*) handle
		);
	}

	void INIT()
	{
		if (initialized)
		{
			return;
		}

		handle_GameAssembly = GetModuleHandleA("GameAssembly.dll");

		INIT_API_FUNCTIONS(handle_GameAssembly);
		VM_FUNCTIONS::INIT();
		AttachCurrentThread();
		CommonCShrap::INIT();

		initialized = true;
	}
}