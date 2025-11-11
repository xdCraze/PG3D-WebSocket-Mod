#include "Renderer.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Renderer 
{
    static bool m_bDrawMenu = false;
    static std::vector<int> menu_keys{ VK_INSERT, VK_DELETE, VK_RCONTROL, VK_F1 };

    static ID3D11Device*            g_pDevice = nullptr;
    static ID3D11DeviceContext*     g_pContext = nullptr;
    static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;
    static WNDPROC                  g_oWndProc = nullptr;
    static HWND                     g_window = nullptr;
    static bool                     g_bIsinitialized = false;

    static void InitFonts() 
    {

    }

    static void InitImGuiBackend() 
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        ImGui_ImplWin32_Init(g_window);
        ImGui_ImplDX11_Init(g_pDevice, g_pContext);

        ImGui::StyleColorsDark();

        InitFonts();
    }

    bool Initialize(IDXGISwapChain* pSwapChain)
    {
        if (g_bIsinitialized) return true;

        if (!pSwapChain) return false;

        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pDevice)))
        {
            g_pDevice->GetImmediateContext(&g_pContext);
            
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            g_window = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer = nullptr;
            if (FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)) || !pBackBuffer)
            {
                if (g_pContext) { g_pContext->Release(); g_pContext = nullptr; }
                if (g_pDevice) { g_pDevice->Release(); g_pDevice = nullptr; }
                return false;
            }

            if (FAILED(g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView))) 
            {
                pBackBuffer->Release();
                if (g_pContext) { g_pContext->Release(); g_pContext = nullptr; }
                if (g_pDevice) { g_pDevice->Release(); g_pDevice = nullptr; }
                return false;
            }
            pBackBuffer->Release();
            
            g_oWndProc = (WNDPROC)SetWindowLongPtr(g_window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            if (!g_oWndProc)
            {
                if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
                if (g_pContext) { g_pContext->Release(); g_pContext = nullptr; }
                if (g_pDevice) { g_pDevice->Release(); g_pDevice = nullptr; }
                return false;
            }

            InitImGuiBackend();
            g_bIsinitialized = true;
            return true;
        }
        return false;
    }

    void Render()
    {
        if (!g_bIsinitialized || !g_pContext || !g_mainRenderTargetView) return;
        
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (m_bDrawMenu) Menu::RenderMenu();

        ImGui::Render();

        for (int key : menu_keys)
            if (GetAsyncKeyState(key) & 1) m_bDrawMenu = !m_bDrawMenu;
        

        g_pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    void Shutdown() 
    {
        if (!g_bIsinitialized) return;
        
        if (g_oWndProc && g_window) 
        {
            SetWindowLongPtr(g_window, GWLP_WNDPROC, (LONG_PTR)g_oWndProc);
            g_oWndProc = nullptr; 
        }

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        if (ImGui::GetCurrentContext()) ImGui::DestroyContext();
        
        if (g_mainRenderTargetView)
        {
            g_mainRenderTargetView->Release();
            g_mainRenderTargetView = nullptr;
        }
        if (g_pContext)
        {
            g_pContext->Release();
            g_pContext = nullptr;
        }
        if (g_pDevice) 
        {
            g_pDevice->Release();
            g_pDevice = nullptr;
        }
        
        g_window = nullptr; 
        g_bIsinitialized = false;
    }

    LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        Helpers::MouseFix(m_bDrawMenu);
        ImGui::GetIO().MouseDrawCursor = m_bDrawMenu;

        if (g_bIsinitialized && m_bDrawMenu)
        {
            ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam); return true;
        }
        
        if (g_oWndProc) return CallWindowProc(g_oWndProc, hWnd, uMsg, wParam, lParam);   

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    bool IsInitialized()
    {
        return g_bIsinitialized;
    }
} 