#include <windows.h>
#include <iostream>
#include <string>
#include <shlobj.h>

void DownloadFile() {
    WCHAR documentsPath[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath) == S_OK) {
        wcscat_s(documentsPath, MAX_PATH, L"\\Downloads");

        if (GetFileAttributesW(documentsPath) == INVALID_FILE_ATTRIBUTES) {
            CreateDirectoryW(documentsPath, NULL);
        }

        wcscat_s(documentsPath, MAX_PATH, L"\\flameshot.dmg");

        std::wstring command = L"powershell -Command \"Invoke-WebRequest -Uri 'https://github.com/flameshot-org/flameshot/releases/download/v12.1.0/flameshot.dmg' -OutFile '" + std::wstring(documentsPath) + L"'\"";

        size_t commandSize = command.size() + 1; // +1 for the null terminator
        char* commandChar = new char[commandSize];

        size_t convertedChars = 0;
        errno_t err = wcstombs_s(&convertedChars, commandChar, commandSize, command.c_str(), commandSize - 1);
        if (err != 0) {
            std::wcout << L"❌ Failed to convert wide string to multi-byte string." << std::endl;
            delete[] commandChar;
            return;
        }

        system(commandChar);
        std::wcout << L"✅ Download started: flameshot.dmg will be saved to Documents\\Downloads." << std::endl;

        delete[] commandChar;
    }
    else {
        std::wcout << L"❌ Failed to get Documents directory." << std::endl;
    }
}

typedef int (WINAPI* MessageBoxW_t)(HWND, LPCWSTR, LPCWSTR, UINT);
MessageBoxW_t OriginalMessageBoxW = MessageBoxW;

int WINAPI HookedMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
    std::wcout << L"🚀 Hooked MessageBoxW! Original Text: " << lpText << std::endl;

    DownloadFile();
    return 0;
}

void InstallIATHook() {
    HMODULE hModule = GetModuleHandle(NULL); 
    if (!hModule) return;

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + dosHeader->e_lfanew);
    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)hModule +
        ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    while (importDesc->Name) {
        char* moduleName = (char*)((BYTE*)hModule + importDesc->Name);
        if (_stricmp(moduleName, "USER32.dll") == 0) {
            PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)((BYTE*)hModule + importDesc->FirstThunk);

            while (thunk->u1.Function) {
                FARPROC* funcAddr = (FARPROC*)&thunk->u1.Function;
                if (*funcAddr == (FARPROC)MessageBoxW) {
                    DWORD oldProtect;
                    VirtualProtect(funcAddr, sizeof(FARPROC), PAGE_EXECUTE_READWRITE, &oldProtect);
                    *funcAddr = (FARPROC)HookedMessageBoxW; 
                    VirtualProtect(funcAddr, sizeof(FARPROC), oldProtect, &oldProtect);
                    std::cout << "✅ Hook Installed!" << std::endl;
                    return;
                }
                thunk++;
            }
        }
        importDesc++;
    }
}

void UninstallIATHook() {
    HMODULE hModule = GetModuleHandle(NULL);
    if (!hModule) return;

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + dosHeader->e_lfanew);
    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)hModule +
        ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    while (importDesc->Name) {
        char* moduleName = (char*)((BYTE*)hModule + importDesc->Name);
        if (_stricmp(moduleName, "USER32.dll") == 0) {
            PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)((BYTE*)hModule + importDesc->FirstThunk);

            while (thunk->u1.Function) {
                FARPROC* funcAddr = (FARPROC*)&thunk->u1.Function;
                if (*funcAddr == (FARPROC)HookedMessageBoxW) {
                    DWORD oldProtect;
                    VirtualProtect(funcAddr, sizeof(FARPROC), PAGE_EXECUTE_READWRITE, &oldProtect);
                    *funcAddr = (FARPROC)OriginalMessageBoxW;
                    VirtualProtect(funcAddr, sizeof(FARPROC), oldProtect, &oldProtect);
                    std::cout << "✅ Hook Removed!" << std::endl;
                    return;
                }
                thunk++;
            }
        }
        importDesc++;
    }
}

int main() {
    std::cout << "🚀 Installing IAT Hook on MessageBoxW..." << std::endl;
    InstallIATHook();

    MessageBoxW(NULL, L"This is a test message.", L"Original", MB_OK);
    UninstallIATHook();
    return 0;
}
