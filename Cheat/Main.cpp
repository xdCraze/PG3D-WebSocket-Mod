#include "Includes.hpp"

#include "Cheat/Cheat.hpp"

Present origPresent;

static bool g_bInit = false;

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags)
{
	if (!Renderer::IsInitialized())
		if (!Renderer::Initialize(pSwapChain)) return origPresent(pSwapChain, syncInterval, flags);

	Renderer::Render();

	return origPresent(pSwapChain, syncInterval, flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved) 
{
	Helpers::Console::Init();

	IL2CPP::INIT();
	Cheat::Initialize();

	Helpers::Log("Init Successful!", Helpers::LogType::SUCCESS);

	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) 
		{
			kiero::bind(8, (void**)&origPresent, hkPresent);
			g_bInit = true;
		}

	} while (!g_bInit);

	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(hMod);
			CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
			break;
		case DLL_PROCESS_DETACH:
			Renderer::Shutdown();
			kiero::shutdown();
			break;
	}
	return TRUE;
}