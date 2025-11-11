#pragma once

#include <json.hpp>

#include "../Data/Structs.hpp"
#include "../../Variables/Variables.hpp"
#include "../../Helpers/Helpers.hpp"

using json = nlohmann::ordered_json;

namespace WebSocket
{
    // helper to create commands
    json CreateCommand(Structs::CommandIDs id, json params, std::vector<int> u = { 140, 6 });

    // for snapshot you can have multiple commands inside of one so this is really useful
    json CreateSnapshot(json params);
}

namespace Commands
{
    /*
    command for adding currency.
    first param is the type of currency youre adding (GemsCurrency, Coins etc).
    second is the amount you want (eg 1000).
    last is the obtain cause, basically telling the game how you obtained this currency. some are more unbannable than others
    */
	std::string AddCurrency(const std::string& curType, int curAmount, int curCause = 1);
}