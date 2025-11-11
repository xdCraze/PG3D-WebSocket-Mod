#include "Commands.hpp"

namespace WebSocket
{
    json CreateCommand(Structs::CommandIDs id, json params, std::vector<int> u)
    {
        return
        {
            {"id", static_cast<int>(id)}, // command id
            {"ci", Helpers::RandHex()}, // not sure what this really is
            {"p", params}, // parameters of the command
            {"h", json::object()}, // hash; would be the hashed (md5) data of whatever slot youre modifying or whatever but its really difficult to setup
            {"v", Variables::Miscellaneous::m_strGameVersion}, // game version (not needed but might as well)
            {"u", u} // snapshot cause or something i forgot
        };
    }

    json CreateSnapshot(json params)
    {
        json result = {
            {"id", Structs::CommandIDs::Snapshot}, // snapshot
            {"i", Helpers::RandHex()}, // not sure what this is
            {"p", {{"c", params}}} // parameters
        };

        return result.dump();
    }
}

namespace Commands
{
    std::string AddCurrency(const std::string& curType, int curAmount, int curCause)
    {
        json command = WebSocket::CreateCommand(Structs::CommandIDs::AddCurrency, {
            {"c",curType},
            {"v", curAmount},
            {"ca", curCause}
            });

        return WebSocket::CreateSnapshot(json::array({ command }));
    }
}