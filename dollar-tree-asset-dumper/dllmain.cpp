#include "headers.hpp"

uintptr_t base = 0;

void main() {
    base = (uintptr_t)(GetModuleHandle(0));
    XAssetDumper::DumpLocalize();
    XAssetDumper::DumpStringTable();
    XAssetDumper::DumpRawFile();
    //XAssetDumper::DumpLuaFile();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        main();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

