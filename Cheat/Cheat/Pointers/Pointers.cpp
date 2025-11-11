#include "Pointers.hpp"

namespace Dummy
{
	Object* WebSocketManagerInstance = nullptr;
}

namespace Pointers
{
	namespace Json
	{
		Wrapper::Method<Object* (String*)> Decode = nullptr;
		Wrapper::Method<String* (Object*)> Encode = nullptr;

		void Initialize()
		{
			Decode = Helpers::Hooking::GetMethodPointerViaIndex("BestHTTP.JSON.Json", 0x0, false);
			Encode = Helpers::Hooking::GetMethodPointerViaIndex("BestHTTP.JSON.Json", 0x2, false);

			Helpers::Log("Json pointer(s) initialized!");
		}
	}

	namespace Application
	{
		Wrapper::Method<String* ()> get_version = nullptr;

		void Initialize()
		{
			get_version = Helpers::Hooking::GetMethodPointer("UnityEngine.Application", "get_version", "UnityEngine.CoreModule.dll");
		}
	}

	void Load()
	{
		Json::Initialize();
		Application::Initialize();
	}
}