#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "structs.hpp"
#include "functions.hpp"

extern uintptr_t base;

namespace Offsets {
	extern uintptr_t GetXAssetPool;
	extern uintptr_t StringTable_GetColumnValueForRow;
	extern uintptr_t DB_ReadRawFile;
	extern uintptr_t DB_GetString;
	void Scan();
}