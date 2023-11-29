#include "headers.hpp"


// pasted scanner, i'm so silly

bool compareByte(const char* pData, const char* bMask, const char* szMask)
{
    for (; *szMask; ++szMask, ++pData, ++bMask)
        if (*szMask == 'x' && *pData != *bMask)
            return false;
    return (*szMask) == NULL;
}

DWORD64 PatternScanEx(const char* name, DWORD64 dwAddress, DWORD64 dwLen, const char* bMask, const char* szMask) {
    bool debug = true;
    DWORD64 retVal = 0x0;
    DWORD length = (DWORD)strlen(szMask);
    for (DWORD i = 0; i < dwLen - length; i++)
    {
        if (compareByte((const char*)(dwAddress + i), bMask, szMask))
        {
            retVal = ((DWORD64)(dwAddress + i));
            if (debug)
                printf("name: %s, address: %llX\n", name, retVal - base);
            return retVal;
        }
    }
    printf("couldn't find %s\n", name);
    return 0ui64;
}

DWORD64 resolveRelativeAddress(DWORD64 instr, DWORD offset, DWORD instrSize) {
    return instr == 0ui64 ? 0ui64 : (instr + instrSize + *(int*)(instr + offset));
}

namespace Offsets {
	uintptr_t GetXAssetPool;
	uintptr_t StringTable_GetColumnValueForRow;
	uintptr_t DB_ReadRawFile;
	uintptr_t DB_GetString;

	void Scan() {
        base = (uintptr_t)(GetModuleHandle(0));
        GetXAssetPool = PatternScanEx("xassetpool", base + 0x2900000, 0x4200000, "\xE8\x00\x00\x00\x00\x4C\x89\x7C\x24\x00\x44\x8B\xCE", "x????xxxx?xxx");
        GetXAssetPool = resolveRelativeAddress(GetXAssetPool, 1, 5);
        StringTable_GetColumnValueForRow = PatternScanEx("stringtable", base + 0x1800000, 0x3400000, "\xE8\x00\x00\x00\x00\x80\x38\x00\x74\x0D", "x????xxxxx");
        StringTable_GetColumnValueForRow = resolveRelativeAddress(StringTable_GetColumnValueForRow, 1, 5);
        DB_ReadRawFile = PatternScanEx("readrawfile", base + 0x3200000, 0x5800000, "\xE8\x00\x00\x00\x00\x48\x85\xC0\x75\x34\x45\x84\xF6", "x????xxxxxxxx");
        DB_ReadRawFile = resolveRelativeAddress(DB_ReadRawFile, 1, 5);
        DB_GetString = PatternScanEx("getstring", base + 0x1000000, 0x3400000, "\xE8\x00\x00\x00\x00\x48\x89\x06\x33\xC0", "x????xxxxx");
        DB_GetString = resolveRelativeAddress(DB_GetString, 1, 5);
	}
}