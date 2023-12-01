#include "headers.hpp"

uintptr_t base = 0;

int main(HMODULE handle) {
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);
    Offsets::Scan();
    if (!Offsets::GetXAssetPool) {
        printf("Asset Pool Offset not found, can't proceed\n");
        return 1;
    }
   
    if (Offsets::StringTable_GetColumnValueForRow) XAssetDumper::DumpStringTable();
    if (Offsets::DB_GetString) XAssetDumper::DumpLocalize();
    //if (Offsets::DB_ReadRawFile) XAssetDumper::DumpRawFile();
    //XAssetDumper::DumpLuaFile();
    FreeLibraryAndExitThread(handle, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        main(hModule);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

