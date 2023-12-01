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

	const uint64_t HashAsset(const char* data)
	{
		uint64_t result = 0x47F5817A5EF961BA;

		for (size_t i = 0; i < strlen(data); i++)
		{
			uint64_t value = tolower(data[i]);

			if (value == '\\')
				value = '/';

			result = 0x100000001B3 * (value ^ result);
		}

		return result & 0x7FFFFFFFFFFFFFFF;
	}
	
	DB_AssetPool* GetXAssetPool(XAssetType type) {
		auto func = reinterpret_cast<DB_AssetPool*(*)(XAssetType)>(Offsets::GetXAssetPool);
		return func(type);
	}

	char* StringTable_GetColumnValueForRow(StringTable* table, int row, int column) {
		auto func = reinterpret_cast<char* (*)(StringTable*, int, int)>(Offsets::StringTable_GetColumnValueForRow); // E8 ?? ?? ?? ?? 80 38 00 74 0D
		return func(table, row, column);
	}

	char* DB_ReadRawFile(const char* filename, char* buf, int size) {
		auto func = reinterpret_cast<char* (*)(const char*, char*, int)>(Offsets::DB_ReadRawFile); // E8 ? ? ? ? 48 85 C0 75 34 45 84 F6
		return func(filename, buf, size);
	}

	// not in the pdb, guessing name
	char* DB_GetString(char* value) {
		auto func = reinterpret_cast<char* (*)(char*)>(Offsets::DB_GetString); // E8 ? ? ? ? 48 89 06 33 C0
		return func(value);
	}

	// thanks stackoverflow
	std::string cleanstr(const std::string& s) {
		std::ostringstream o;
		for (auto c = s.cbegin(); c != s.cend(); c++) {
			switch (*c) {
			case '"': o << "\\\""; break;
			case '\\': o << "\\\\"; break;
			case '\b': o << "\\b"; break;
			case '\f': o << "\\f"; break;
			case '\n': o << "\\n"; break;
			case '\r': o << "\\r"; break;
			case '\t': o << "\\t"; break;
			default:
				if ('\x00' <= *c && *c <= '\x1f') {
					o << "\\u"
						<< std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(*c);
				}
				else {
					o << *c;
				}
			}
		}
		return o.str();
	}

	std::unordered_map<__int64, std::string> localizeHashes;

	void IsLocalizedString(std::string str) {
		if (str.length() && strstr(str.c_str(), "/") && !strstr(str.c_str(), ".")) {
			localizeHashes.insert({ HashAsset(str.c_str()), str });
		}
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
					std::string e = cell;
					IsLocalizedString(e); // grabbing unhashed localize names
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
			
		}
		printf("Dumped StringTables\n");
	}

	void DumpLocalize() {
		char buf[512];
		DB_AssetPool* pool = GetXAssetPool(ASSET_TYPE_LOCALIZE);
		auto assets = (LocalizeEntry*)(pool->m_entries);
		std::filesystem::create_directories(path + "Localize");
		std::ofstream file;
		file.open(path + "Localize\\localize.json");
		file << "{\n";
		for (int i = 0; i < pool->m_loadedPoolSize; ++i) {
			auto header = &assets[i];
			if (!header->hash || !header->value) continue;

			auto keyExists = localizeHashes.find(header->hash);
			if (keyExists == localizeHashes.end()) {
				sprintf_s(buf, "%llX", header->hash);
			}
			else {
				sprintf_s(buf, "%s", keyExists->second.c_str());
			}

			file << "    \"" << buf << "\": \"" << cleanstr(DB_GetString(header->value)) << "\"";

			if (i + 1 != pool->m_loadedPoolSize) {
				file << ",\n";
			}
		}
		file << "\n}";
		file.close();
		localizeHashes.clear();
		printf("Dumped Localize\n");
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