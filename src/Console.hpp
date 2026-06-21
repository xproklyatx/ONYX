#pragma once
#ifndef NDEBUG
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
class Console
{
  public:
    static void Init()
    {
        AllocConsole();
        SetConsoleTitle("ONYX DBG Console");
        FILE* pFile;
        freopen_s(&pFile, "CONOUT$", "w", stdout);
        FILE* pErr;
        freopen_s(&pErr, "CONOUT$", "w", stderr);
        FILE* pIn;
        freopen_s(&pIn, "CONIN$", "r", stdin);
        std::ios::sync_with_stdio();
    }

    static void PrintNormal(const char* printString)
    {
        SetColor(15, 0);
        std::cout << "PRINT: " << printString << "\n";
    }
    static void PrintError(const char* printString)
    {
        SetColor(0, 4);
        std::cout << "ERROR: " << printString;
        SetColor(15, 0);
        std::cout << "\n";
    }
    static void PrintDiagnosis(const char* printString)
    {
        SetColor(9, 0);
        std::cout << "DIAG: " << printString;
        SetColor(15, 0);
        std::cout << "\n";
    }
    static void PrintLog(const char* printString)
    {
        SetColor(2, 0);
        std::cout << "LOG: " << printString;
        SetColor(15, 0);
        std::cout << "\n";
    }
    static void PrintWarn(const char* printString)
    {
        SetColor(11, 0);
        std::cout << "WARN: " << printString;
        SetColor(15, 0);
        std::cout << "\n";
    }
    static void Destroy()
    {
        PrintNormal("RENDERER IS COMPLETELY TERMINATED");
        PrintNormal("PRESS ESC TO CONTINUE...");
        while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
        {
            Sleep(10);
        }
        FreeConsole();
    }

  private:
    static void SetColor(int textColor, int backgroundColor)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, (backgroundColor << 4) | textColor);
    }
};
#endif
#ifndef NDEBUG
#define PRINT_NORMAL(x) Console::PrintNormal(x)
#define PRINT_ERROR(x) Console::PrintError(x)
#define PRINT_WARN(x) Console::PrintWarn(x)
#define PRINT_DIAG(x) Console::PrintDiagnosis(x)
#define PRINT_LOG(x) Console::PrintLog(x)
#else
#define PRINT_NORMAL(x)
#define PRINT_ERROR(x)
#define PRINT_WARN(x)
#define PRINT_DIAG(x)
#define PRINT_LOG(x)
#endif
