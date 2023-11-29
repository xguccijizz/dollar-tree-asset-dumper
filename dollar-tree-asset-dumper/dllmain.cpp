#include "headers.hpp"

uintptr_t base = 0;

int main() {
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);
    Offsets::Scan();
    if (!Offsets::GetXAssetPool) {
        printf("Asset Pool Offset not found, can't proceed\n");
        return 1;
    }
    if (Offsets::DB_GetString) XAssetDumper::DumpLocalize();
    if (Offsets::StringTable_GetColumnValueForRow) XAssetDumper::DumpStringTable();
    if (Offsets::DB_ReadRawFile) XAssetDumper::DumpRawFile();
    //XAssetDumper::DumpLuaFile();
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
        main();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

