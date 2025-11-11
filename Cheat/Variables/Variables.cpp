#include "Variables.hpp"

namespace Variables
{
    namespace Gameplay
    {
        bool m_bEnableJetpack;
    }

    namespace Account
    {
        bool m_bAddCurrency;
        int m_iCurrencyAmount = 100, m_iCurrencyCause = 1;
        std::string m_strCurrency = "GemsCurrency";
    }

    namespace WebSocket
    {
        bool m_bReloadSocket;
        std::string m_strSocketStatus = "Not Active", m_strCommandStatus = "-";
    }

    namespace Miscellaneous
    {
        std::string m_strGameVersion = "???";
    }

    namespace Global
    {
        uintptr_t g_dwBaseAddress = (uintptr_t)GetModuleHandleA("GameAssembly.dll");
    }
}
