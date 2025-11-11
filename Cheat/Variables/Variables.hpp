#pragma once

#include <Windows.h>
#include <string>
#include <cstdint>

namespace Variables
{
    namespace Gameplay
    {
        extern bool m_bEnableJetpack;
    }

    namespace Account
    {
        extern bool m_bAddCurrency;
        extern int m_iCurrencyAmount, m_iCurrencyCause;
        extern std::string m_strCurrency;
    }

    namespace WebSocket
    {
        extern bool m_bReloadSocket;
        extern std::string m_strSocketStatus, m_strCommandStatus;
    }

    namespace Miscellaneous
    {
        extern std::string m_strGameVersion;
    }

    namespace Global
    {
        extern uintptr_t g_dwBaseAddress;
    }
}