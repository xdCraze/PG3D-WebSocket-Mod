#pragma once

#include "../Pointers/Pointers.hpp"
#include "../WebSocket/Commands.hpp"

using namespace IL2CPP;

namespace Hooks
{
	namespace Socket
	{
		struct Packet
		{
			IL2CPP::String* Placeholder;
			LPVOID          TransportEventTypes;
			LPVOID          SocketIOEventTypes;
			INT             AttachmentCount;
			INT             Id;
			IL2CPP::String* Namespace;
			IL2CPP::String* Payload;
			IL2CPP::String* EventName;
			LPVOID          byte;
			BOOL            IsDecoded;
			IL2CPP::Object* DecodedArgs;
		};

		/* credit: sxitxma i think i cant remember */
		struct Response
		{
			json request;
			json response;
			std::string requestString;
			std::string ResponseStatus; // OK, FAIL
			std::function<void(Response)> OnRequestReceived; // ignore this please, thanks
			bool ForceExecute = false; // execute OnRequestReceived even if response gives fail status
		};

		extern void (*oldOnPacket)(Object*, Packet*);
		void hkOnPacket(Object* instance, Packet* packet);

		void Initialize();
		void Uninitialize();
	}

	namespace WebSocketManager
	{
		extern int (*oldSend)(Object*, String*, Object*);
		int hkSend(Object* instance, String* clientEvent, Object* data); // found with stacktrace
		int SendCommand(json data, std::function<void(Socket::Response)>onRequestReceived, bool forceExecute = true);
	}

	namespace PixelTime
	{
		extern void (*oldUpdate)(Object*);
		void hkUpdate(Object* instance);

		void Initialize();
		void Uninitialize();
	}

	void Load();
	void Unload();
}