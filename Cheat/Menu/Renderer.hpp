#pragma once

#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>

#include "Menu.hpp"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx11.h>
#include "../Helpers/Helpers.hpp"

namespace Renderer
{
    bool Initialize(IDXGISwapChain* pSwapChain);

    void Render();
    void Shutdown();

    LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool IsInitialized();
}