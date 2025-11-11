#include "Hooks.hpp"

namespace Hooks
{
	namespace Socket
	{
		std::vector<Response> Responses;

		void (*oldOnPacket)(Object*, Packet*) = nullptr;
		void hkOnPacket(Object* instance, Packet* packet)
		{
			oldOnPacket(instance, packet);

			if (packet->EventName && packet->IsDecoded)
			{
				std::string eventName = packet->EventName->ToString();
				std::string decodedArgs = Pointers::Json::Encode(packet->DecodedArgs)->ToString();
				json data = json::parse(decodedArgs);

				Helpers::Log(std::format("{} <- {}", eventName.c_str(), decodedArgs.c_str()), Helpers::LogType::CUSTOM, "SOCKET");

				// On Request Received
				int req_id = -1;
				json req_idJSON = data[0]["req_id"];
				if (!req_idJSON.is_null())
					req_id = req_idJSON.get<int>();
				

				if (req_id != -1)
				{
					for (int i = 0; i < Responses.size(); i++)
					{
						Response responseData = Responses[i];
						if (responseData.request["req_id"].get<int>() == req_id)
						{
							Responses.erase(Responses.begin() + i);
							responseData.response = data;
							json status = data[0]["status"];
							std::string fail = "FAIL";
							std::string ok = "OK";
							responseData.ResponseStatus = ok;

							if (!status.is_null())
							{
								if (strcmp(status.get<std::string>().c_str(), "ok") != 0)
								{
									responseData.ResponseStatus = fail;
									if (!responseData.ForceExecute) responseData.OnRequestReceived = nullptr;
								}
							}

							if (responseData.OnRequestReceived != nullptr) responseData.OnRequestReceived(responseData);
							break;
						}
					}
				}
			}
		}

		void Initialize()
		{
			Helpers::Hooking::Attach("BestHTTP.SocketIO.Socket", "BestHTTP.SocketIO.ISocket.OnPacket", hkOnPacket, &oldOnPacket);

			Helpers::Log("Socket initialized!");
		}

		void Uninitialize()
		{
			// add your functionality here
		}
	}

	namespace WebSocketManager
	{
		int (*oldSend)(Object*, String*, Object*) = nullptr;
		int hkSend(Object* instance, String* clientEvent, Object* data)
		{
			if (Dummy::WebSocketManagerInstance == nullptr)
			{
				Dummy::WebSocketManagerInstance = instance;
				Helpers::Log("Socket is active");
				Variables::WebSocket::m_strSocketStatus = "Active";
			}

			IL2CPP::String* jsonArgs = Pointers::Json::Encode(data);

			Helpers::Log(std::format("{} -> {}", clientEvent->ToString().c_str(), jsonArgs->ToString().c_str()), Helpers::LogType::CUSTOM, "SOCKET");

			return oldSend(instance, clientEvent, data);
		}

		int SendCommand(json data, std::function<void(Socket::Response)>onRequestReceived, bool forceExecute)
		{
			if (!data.is_array()) return -1;

			int req_id = Dummy::WebSocketManagerInstance->GetFieldRef<int>(0x4);
			data[1]["req_id"] = req_id;

			Socket::Response ResponseData;

			ResponseData.request = data[1];
			if (onRequestReceived != nullptr)
			{
				ResponseData.OnRequestReceived = onRequestReceived;
			}
			ResponseData.ForceExecute = forceExecute;

			Socket::Responses.emplace_back(ResponseData);

			if (Dummy::WebSocketManagerInstance != nullptr) // if you send a command when ws isnt active it will crash!!
				oldSend(Dummy::WebSocketManagerInstance, IL2CPP::String::Create(data[0].get<std::string>().c_str()), Pointers::Json::Decode(IL2CPP::String::Create(data[1].dump().c_str())));

			return req_id;
		}

		void Initialize()
		{
			Helpers::Hooking::Attach("Rilisoft.WebSocket.WebSocketManager", 0x16, hkSend, &oldSend);

			Helpers::Log("WebSocketManager initialized!");
		}
	}

	namespace PixelTime
	{
		// commands inside of pixelTime because its always active (credit to fedesito for finding this i think)
		void (*oldUpdate)(Object*) = nullptr;
		void hkUpdate(Object* instance)
		{
			if (instance)
			{
				Variables::Miscellaneous::m_strGameVersion = Pointers::Application::get_version()->ToString().c_str();

				if (Variables::WebSocket::m_bReloadSocket)
				{
					Variables::WebSocket::m_bReloadSocket = false;

					// simple reload
					WebSocketManager::SendCommand(json::array({ "update_progress", json::object()}), [](Socket::Response pkt) {

						// obviously this is useless for this command because we are sending nothing so it will just error
						if (pkt.ResponseStatus == "OK")
						{
							// do something
						}
						else
						{
							// do another thing
						}

						}, false); // false because we don't want forceExecute
				}

				if (Variables::Account::m_bAddCurrency)
				{
					Variables::Account::m_bAddCurrency = false;

					// i recommend making a helper for this and not just doing this over and over
					WebSocketManager::SendCommand(json::array({ "update_progress", json::parse(Commands::AddCurrency(Variables::Account::m_strCurrency, Variables::Account::m_iCurrencyAmount, Variables::Account::m_iCurrencyCause))}), [](Socket::Response pkt) {

						if (pkt.ResponseStatus == "OK") // if status is ok we will auto reload to apply currency (sadly you have to reload but you can get around this if you use ApplySlots)
						{
							Variables::WebSocket::m_bReloadSocket = true;
							Variables::WebSocket::m_strCommandStatus = "Success";
						}
						else
						{
							Helpers::Log("Failed to add currency!", Helpers::LogType::ERR);
							Variables::WebSocket::m_strCommandStatus = "Failed";
						}

						}, false);
				}
			}

			oldUpdate(instance);
		}

		void Initialize()
		{
			Helpers::Hooking::Attach("PixelTime", "Update", hkUpdate, &oldUpdate);

			Helpers::Log("PixelTime initialized!");
		}

		void Uninitialize()
		{
			// add your functionality here
		}
	}

	void Load()
	{
		PixelTime::Initialize();
		WebSocketManager::Initialize();
		Socket::Initialize();
	}

	void Unload()
	{
		// add your functionality here
	}
}