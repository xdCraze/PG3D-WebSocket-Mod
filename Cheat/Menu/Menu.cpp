#include "Menu.hpp"

namespace Menu 
{

    void RenderMenu() 
    {  
        ImGui::Begin("PG3D WebSocket Mod"); 
        {
            ImGui::Text("WebSocket Status: %s", Variables::WebSocket::m_strSocketStatus.c_str());
            ImGui::Text("Command Status: %s", Variables::WebSocket::m_strCommandStatus.c_str());
            ImGui::Separator();
            if (ImGui::Button("Reload Socket")) Variables::WebSocket::m_bReloadSocket = true;

            ImGui::InputText("Currency Type", Variables::Account::m_strCurrency.data(), 500);
            ImGui::InputInt("Currency Amount", &Variables::Account::m_iCurrencyAmount);
            ImGui::InputInt("Currency Cause", &Variables::Account::m_iCurrencyCause);
            if (ImGui::Button("Add Currency")) Variables::Account::m_bAddCurrency = true;
        }

        ImGui::End();     
    }
}