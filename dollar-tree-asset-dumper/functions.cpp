#include "functions.hpp"

namespace XAssetDumper {

	// basic internal raw xasset dumper
	// 
	// for luafiles and rawfiles would advise using a proper zlib library for decompressing
	// 
	// assets will dump on injection

	// Export path in games directory ( Call of Duty\_retail_\Assets )
	std::string path = "Assets\\";

	// Functions

	DB_AssetPool* GetXAssetPool(XAssetType type) {
		auto pool = (DB_AssetPool*)(0xC0A3E50_g); // E8 ? ? ? ? 4C 89 7C 24 ? 44 8B CE
		return &pool[type];
	}

	char* StringTable_GetColumnValueForRow(StringTable* table, int row, int column) {
		auto func = reinterpret_cast<char* (*)(StringTable*, int, int)>(0x41748D0_g); // E8 ?? ?? ?? ?? 80 38 00 74 0D
		return func(table, row, column);
	}

	char* DB_ReadRawFile(const char* filename, char* buf, int size) {
		auto func = reinterpret_cast<char* (*)(const char*, char*, int)>(0x37AD8B0_g); // E8 ? ? ? ? 48 85 C0 75 34 45 84 F6
		return func(filename, buf, size);
	}

	// Assets

	void DumpStringTable() {
		char buf[256];
		DB_AssetPool* pool = GetXAssetPool(ASSET_TYPE_STRINGTABLE); 
		auto assets = (StringTable*)(pool->m_entries);
		std::filesystem::create_directories(path + "StringTable");
		for (int i = 0; i < pool->m_loadedPoolSize; ++i) {
			auto header = &assets[i];
			std::ofstream file;

			sprintf_s(buf, "StringTable\\%llX.csv", header->hash);

			file.open(path + buf);

			int idx = 0;
			for (int x = 0; x < header->rowCount; ++x) {
				for (int y = 0; y < header->columnCount; ++y)
				{
					char* cell = StringTable_GetColumnValueForRow(header, x, y);
					file << cell;
					if (y != (header->columnCount - 1))
					{
						file << ",";
					}
					idx++;
				}
				file << std::endl;
			}
			file.close();
			//printf("exported csv - %llX.csv\n", header->hash);
		}
	}

	void DumpLuaFile() {
		char buf[256];
		DB_AssetPool* pool = GetXAssetPool(ASSET_TYPE_LUA_FILE);
		auto assets = (LuaFile*)(pool->m_entries);
		std::filesystem::create_directories(path + "LuaFile");
		for (int i = 0; i < pool->m_loadedPoolSize; ++i) {
			auto header = &assets[i];
			
			sprintf_s(buf, "LuaFile\\%llX.lua", header->hash);

			std::ofstream file;
			file.open(path + buf);
			// Add ZLib Decompression
			file.write(header->buffer, header->len);

			file.close();
			//printf("exported luafile - %llX.lua\n", header->hash);
		}
	}

	void DumpRawFile() {
		char namebuf[256];
		char namehash[256];
		char outbuf[0x40000];
		DB_AssetPool* pool = GetXAssetPool(ASSET_TYPE_RAWFILE);
		auto assets = (RawFile*)(pool->m_entries);
		std::filesystem::create_directories(path + "RawFile");
		
		for (int i = 0; i < pool->m_loadedPoolSize; ++i) {
			auto header = &assets[i];

			// printf("starting - %llX\n", header->hash);
			
			// crashes on higher size items
			if (header->compressedLen > 60000) continue;

			sprintf_s(namebuf, "RawFile\\%llX.rawfile", header->hash);
			sprintf_s(namehash, "#x4%llX", header->hash);

			DB_ReadRawFile(namehash, outbuf, 0x40000);
			std::ofstream file;
			file.open(path + namebuf);
			file.write(outbuf, header->len);
			file.close();
			
			//printf("exported rawfile - %llX.rawfile\n", header->hash);
		}
	}
}

size_t operator"" _g(const size_t val)
{
	return base + (val);
}