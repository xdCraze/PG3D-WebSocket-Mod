#include "Helpers.hpp"

void SetColor(int color) 
{
    HANDLE hGet = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hGet, color);
}

namespace Helpers
{
    namespace Console 
    {
        static std::string Title() 
        {
            std::string title = "PG3D WebSocket Mod ";

#ifdef _DEBUG
            title += "Debug ";
#else
            title += "Release ";
#endif
            title += Variables::Miscellaneous::m_strGameVersion;
            SetConsoleTitleA(title.c_str());
            return title;
        }

        void Init()
        {
            AllocConsole();
            FILE* f;
            freopen_s(&f, "CONOUT$", "w", stdout);
#ifdef _DEBUG
            freopen_s(&f, "CONOUT$", "w", stderr);
#endif
            SetConsoleOutputCP(CP_UTF8);
            Title();
        }
    }

    std::string CurrentTime()
    {
        time_t now = time(nullptr);
        tm timeStruct{};
        localtime_s(&timeStruct, &now);

        char buffer[80];
        strftime(buffer, sizeof(buffer), "[%H:%M:%S] ", &timeStruct);
        return std::string(buffer);
    }

    void Log(std::string message, LogType type, std::string customPrefix) 
    {
        std::string prefix;
        int color = 8;

        switch (type) 
        {
        case LogType::INFO:    color = 9; prefix = "[INFO]"; break;
        case LogType::SUCCESS: color = 2; prefix = "[SUCCESS]"; break;
        case LogType::WARNING: color = 6; prefix = "[WARNING]"; break;
        case LogType::ERR:   color = 4; prefix = "[ERROR]"; break;
        case LogType::DEBUG:   color = 8; prefix = "[DEBUG]"; break;
        case LogType::CUSTOM:  color = 8; prefix = "[" + customPrefix + "]"; break;
        }

        SetColor(color);
        std::cout << CurrentTime() << prefix;
        SetColor(8);
        std::cout << " - " << message << '\n';
    }

    void MouseFix(bool menuVisible)
    {
        HWND hwnd = FindWindowA(nullptr, "Pixel Gun 3D");
        if (!hwnd) return;

        HWND activeWindow = GetForegroundWindow();
        if (activeWindow != hwnd || IsIconic(hwnd)) return;

        if (!menuVisible)
        {
            RECT rect{};
            if (GetWindowRect(hwnd, &rect)) 
            {
                POINT center =  {
                    (rect.left + rect.right) / 2,
                    (rect.top + rect.bottom) / 2
                };
                SetCursorPos(center.x, center.y);
            }
        }
        else 
        {
            RECT screenRect = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
            ClipCursor(&screenRect);
        }
    }

    namespace Hooking
    {
        template<typename Func>
        bool Create(void* target, Func* replace, Func** original)
        {
            if (target == nullptr || replace == nullptr || original == nullptr)
                return false;

            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());

            if (DetourAttach(&target, reinterpret_cast<PVOID>(replace)) != NO_ERROR)
            {
                DetourTransactionAbort();
                return false;
            }

            if (DetourTransactionCommit() != NO_ERROR)
                return false;

            *original = (Func*)target;
            return true;
        }

        IL2CPP::Class* GetClass(const std::string& className, const std::string& assembly)
        {
            IL2CPP::Image* TargetClass = IL2CPP::Domain().OpenAssembly(assembly);
            return (IL2CPP::Class*)TargetClass->GetClass(className);
        }

        IL2CPP::Class* GetObfuscatedClass(const std::string& keyname)
        {
            return IL2CPP::ClassMapping::GetClass(keyname);
        }

        void Attach(const std::string& className, const std::string& methodName, void* replace, void* original, const std::string& assembly)
        {
            const auto klass = GetClass(className, assembly);
            const auto method = klass->GetMethod(methodName);
            if (!method || !Create(method->GetPointer(), replace, (void**)original)) [[unlikely]]
            {
                Log(std::format("failed to create hook: {}", replace), LogType::ERR, "");
            }
        }

        void Attach(const std::string& className, int index, void* replace, void* original, const std::string& assembly)
        {
            const auto klass = GetClass(className, assembly);
            const auto method = klass->GetMethod(index);
            if (!method || !Create(method->GetPointer(), replace, (void**)original)) [[unlikely]]
            {
                Log(std::format("failed to create hook: {}", replace), LogType::ERR, "");
            }
        }

        LPVOID GetMethodPointer(const std::string& className, const std::string& methodName, const std::string& assembly)
        {
            return (LPVOID)GetClass(className, assembly)->GetMethod(methodName)->GetPointer();
        }

        LPVOID GetMethodPointerViaIndex(const std::string& keyname, int index, bool obfuscated, const std::string& assembly)
        {
            auto result = (obfuscated ? (LPVOID)GetObfuscatedClass(keyname)->GetMethod(index)->GetPointer() : (LPVOID)GetClass(keyname, assembly)->GetMethod(index)->GetPointer());

            return result;
        }
    }

    std::string RandHex(short length)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, 255);

        std::stringstream ss; for (short i = 0; i < length; ++i)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << distrib(gen);
        }

        return ss.str();
    }
}