#pragma once
#include "headers.hpp"

enum XAssetType {
	ASSET_TYPE_LOCALIZE = 0x3B,
	ASSET_TYPE_RAWFILE = 0x44,
	ASSET_TYPE_STRINGTABLE = 0x47,
	ASSET_TYPE_LUA_FILE = 0x4C,
};

struct DB_AssetPool {
	void* m_entries;
	void* m_freeHead;
	unsigned int m_poolSize;
	unsigned int m_elementSize;
	unsigned int m_loadedPoolSize;
	char __padding[0x86];
};

// Assets

struct RawFile {
	uintptr_t hash;
	char unk[0x4];
	int compressedLen;
	int len;
	const char* buffer;
};

struct StringTable {
	uintptr_t hash;
	int columnCount;
	int rowCount;
	char __padding[0x18];
};

struct LuaFile
{
	uintptr_t hash;
	int len;
	const char* buffer;
};

struct LocalizeEntry
{
	uintptr_t hash;
	char* value;
};