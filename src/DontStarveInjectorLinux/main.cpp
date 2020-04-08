#include <unordered_set>
#include <unordered_map>
#include <list>
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <dlfcn.h>
#include <set>
#include <cassert>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/mman.h>
#include "../DontStarveInjector/xde.h"

#define PAGE_SIZE         sysconf(_SC_PAGESIZE)
#define PAGE_MASK         (~(PAGE_SIZE-1))
#define PAGE_ALIGN(addr)  (unsigned char *)(((uintptr_t)(addr) + PAGE_SIZE - 1) & PAGE_MASK)

const char* allFunctions[] = {
	
			"luaL_addlstring", "luaL_addstring", "luaL_addvalue", "luaL_argerror",
			"luaL_buffinit", "luaL_callmeta", "luaL_checkany", "luaL_findtable", "luaL_gsub",
			"luaL_loadbuffer", "luaL_loadfile", "luaL_loadstring", "luaL_newmetatable",
			"luaL_newstate", "luaL_openlib", "luaL_openlibs", "luaL_optinteger",
			"luaL_optlstring", "luaL_optnumber", "luaL_prepbuffer", "luaL_pushresult",
			"luaL_ref", "luaL_register", "luaL_unref", "luaL_where",
			 "lua_equal", "lua_getallocf", "lua_getupvalue", "luaL_checkudata",
			"lua_pushvfstring", "lua_setallocf", "lua_setupvalue", "internal_error",
	"luaL_checktype", "luaL_error", "luaL_typerror", "lua_atpanic",
	"lua_call", "lua_checkstack", "lua_close",
	"lua_pushlstring", "lua_dump", "lua_error", "lua_gc",
	"lua_getfenv", "lua_getfield", "lua_getinfo", "lua_getlocal",
	"lua_getmetatable", "lua_getstack", "lua_gettable", "lua_gettop",
	"lua_insert", "lua_iscfunction", "lua_isnumber", "lua_isstring",
	"lua_lessthan", "lua_load", "lua_newstate", "lua_newthread",
	"lua_newuserdata", "lua_next", "lua_objlen", "lua_pcall",
	"lua_pushboolean", "lua_pushcclosure", "lua_pushfstring", "lua_pushinteger",
	"lua_pushlightuserdata", "lua_createtable", "lua_pushnil", "lua_pushnumber",
	"lua_pushstring", "lua_pushthread", "lua_pushvalue", "lua_rawequal",
	"lua_rawget", "lua_rawgeti", "lua_rawset", "lua_rawseti",
	"lua_remove", "lua_replace", "lua_resume", "lua_setfenv",
	"lua_setfield", "lua_sethook", "lua_setlocal", "lua_setmetatable",
	"lua_settable", "lua_settop", "lua_toboolean", "lua_tocfunction",
	"lua_tointeger", "lua_tolstring", "lua_tonumber", "lua_topointer",
	"lua_tothread", "lua_touserdata", "lua_type", "lua_typename", "lua_concat",
	"lua_xmove", "lua_yield", "luaopen_base", "luaopen_debug",
	"luaopen_io", "luaopen_math", "luaopen_os", "luaopen_package",
	"luaopen_string", "luaopen_table",
};


class Bridge {
public:
	enum { PROC_ALIGN = 16, INSTR_SIZE = 64, INSTR_MATCH_COUNT = 12, MAX_SINGLE_INSTR_LENGTH = 24 };
	struct Entry {
		bool operator < (const Entry& rhs) const {
			return instr < rhs.instr;
		}

		uint8_t instr[INSTR_SIZE];
		int lengthHist[MAX_SINGLE_INSTR_LENGTH];
		size_t validLength;
		std::string name;
		uint8_t* address;
	};

	Entry ParseEntry(const std::string& name, uint8_t* address) {
		Entry entry;
		entry.name = name;
		entry.address = address;
		uint8_t* target = entry.instr;
		memset(target, 0x00, INSTR_SIZE);
		memset(entry.lengthHist, 0, sizeof(entry.lengthHist));
		xde_instr instr;
		size_t validLength = 0;
		bool edge = false;
		while (target < entry.instr + INSTR_SIZE) {
			xde_disasm(address, &instr);
			int len = instr.len;
			if (len == 0)
				break;
	
			if (instr.opcode == 0x68 || instr.opcode == 0xc7 || instr.addrsize == 4) {
				// read memory data
				int offset = instr.opcode == 0xe8 || instr.opcode == 0xe9 ? 1 : instr.opcode == 0x68 ? 1 : instr.opcode == 0xc7 && *(address + 1) == 0x44 && *(address + 2) == 0x24 ? 4 : 0;
				void* addr = instr.opcode == 0xe8 || instr.opcode == 0xe9 ? address + 5 + *(size_t*)(address + 1) : offset != 0 ? *(void**)(address + offset) : (void*)instr.addr_l[0];
				if (write(nullfd, addr, 4) >= 0)
				{
					if (offset == 0) {
						uint8_t temp[16];
						instr.addr_l[0] = *(long*)addr;
						xde_asm(temp, &instr);

						memcpy(target, temp, len + target > entry.instr + INSTR_SIZE ? entry.instr + INSTR_SIZE - target : len);
					} else {
						memcpy(target, address, len + target > entry.instr + INSTR_SIZE ? entry.instr + INSTR_SIZE - target : len);
						memcpy(target + offset, addr, 4);
					}
				} else {
					memcpy(target, address, len + target > entry.instr + INSTR_SIZE ? entry.instr + INSTR_SIZE - target : len);
				}
			} else {
				memcpy(target, address, len + target > entry.instr + INSTR_SIZE ? entry.instr + INSTR_SIZE - target : len);
			}
	
			address += len;
			target += len;
			if (!edge)
				validLength += len + target > entry.instr + INSTR_SIZE ? entry.instr + INSTR_SIZE - target : len;
			entry.lengthHist[len]++;
	
			if (*address == 0xC3) {
				edge = true;
			}
		}
	
		entry.validLength = validLength;
		return entry;
	}

	void GetEntries(void* instance, std::set<Entry>& entries) {
		for (size_t i = 0; i < sizeof(allFunctions) / sizeof(allFunctions[0]); i++) {
			const char* name = allFunctions[i];
			if (missingFuncs.count(name) != 0) continue;

			uint8_t* address = (uint8_t*)dlsym(instance, name);
			assert(address != nullptr);
			Entry entry = ParseEntry(name, address);
			if (entries.count(entry) == 0) {
				entries.insert(entry);
			}
		}
	}
	
	static int CommonLength(const uint8_t* x, int xlen, const uint8_t* y, int ylen) {
		int opt[INSTR_SIZE + 1][INSTR_SIZE + 1];
		memset(opt, 0, sizeof(opt));

		for (int i = 1; i <= xlen; i++) {
			for (int j = 1; j <= ylen; j++) {
				if (x[i - 1] == y[j - 1])
					opt[i][j] = opt[i - 1][j - 1] + 1;
				else
					opt[i][j] = opt[i - 1][j] > opt[i][j - 1] ? opt[i - 1][j] : opt[i][j - 1];
			}
		}

		return opt[xlen][ylen];
	}

	static void Hook(uint8_t* fromAddress, uint8_t* toAddress) {
		// inline hook
		unsigned char code[5] = { 0xe9, 0x00, 0x00, 0x00, 0x00 };
		*(long*)(code + 1) = (long)toAddress - (long)fromAddress - 5;

		mprotect(PAGE_ALIGN(fromAddress) - PAGE_SIZE, PAGE_SIZE * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
		memcpy(fromAddress, code, 5);
		mprotect(PAGE_ALIGN(fromAddress) - PAGE_SIZE, PAGE_SIZE * 2, PROT_READ | PROT_EXEC);
	}

	void RedirectLuaProviderEntries() {
		std::set<Entry> toEntries, entries;
		GetEntries(refer, entries);
		GetEntries(to, toEntries);
		/*
		uint8_t* h = (uint8_t*)0x8383C10; // index2addr, should not be called any more
		mprotect((void*)0x8383000, PAGE_SIZE * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
		*h = 0xcc;
		mprotect((void*)0x8383000, PAGE_SIZE * 2, PROT_READ | PROT_WRITE | PROT_EXEC);*/
		std::unordered_map<std::string, uint8_t*> mapAddress;
		for (std::set<Entry>::iterator it = toEntries.begin(); it != toEntries.end(); ++it) {
			mapAddress[(*it).name] = (*it).address;
			// printf("Function Refs (%s)\n", (*it).name.c_str());
		}

		std::unordered_set<uint8_t*> marked;
		std::unordered_map<void*, std::list<Entry> > mapEntries;
		uint8_t* addrMin = (uint8_t*)0xFFFFFFFF;
		uint8_t* addrMax = (uint8_t*)0x0;

		std::vector<std::pair<uint8_t*, uint8_t*> > hookList;

		const int BUFFER_SIZE = 4096;
		char buffer[BUFFER_SIZE];
		char path[256];
		sprintf(path, "/proc/%d/maps", getpid());

		uint8_t* start = (uint8_t*)0xFFFFFFFF;
		uint8_t* end = (uint8_t*)0x0;

		FILE* fp = fopen(path, "rb");
		if (fp != NULL) {
		    while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
				if (strstr(buffer, "dontstarve_") != nullptr) {
					uint8_t* low = nullptr, *high = nullptr;
					sscanf(buffer, "%p-%p", &low, &high);
					start = std::min(low, start);
					end = std::max(high, end);
				}
			}
		    fclose(fp);
		}

		printf("Address range %p - %p\n", start, end);
		if (start < end) {
			for (std::set<Entry>::iterator q = entries.begin(); q != entries.end(); ++q) {
				// bool isCheck = (*q).name == "luaopen_debug";
				uint8_t* address = mapAddress[(*q).name];
				uint8_t firstByte = *address;
				if (address != NULL && (*q).validLength > INSTR_MATCH_COUNT) {
					int maxCount = 0;
					int maxTotalCount = 0;
					uint8_t* best = NULL;
					for (uint8_t* p = start + 0x280000; p < end; p += PROC_ALIGN) {
						if (*(p - 1) != 0xC3 && *(p - 1) != 0x90) continue;
						if (marked.count(p)) continue;
						// skip lua_toboolean mess
						if (*p == 0x31 && *(p + 1) == 0xc0 && *(p + 2) == 0xeb) continue;

						// find nearest
						uint8_t* left = p;
						Entry entry = ParseEntry(q->name, left);

						int count = CommonLength(entry.instr, entry.validLength, (*q).instr, std::min((*q).validLength, entry.validLength));
						if (count > maxCount) {
							maxCount = count;
							maxTotalCount = maxCount;
							best = p;
						}
					}

					if (best != nullptr && maxTotalCount >= INSTR_MATCH_COUNT) {
						if (marked.count(best) != 0) {
							printf("ADDR %p Already registered. Please report this bug to me.\n", best);
						} else {
							//	Hook(best, address);
							marked.insert(best);
							hookList.push_back(std::make_pair(best, address));
						}
						// Hook(best, address);
						printf("[%p] (%0.2f%%) [%d/%d] - Hooked function (%p) to (%p) %s\n", best, (float)maxTotalCount * 100 / (*q).validLength, maxTotalCount, (*q).validLength, best, address, (*q).name.c_str());

						addrMax = best > addrMax ? best : addrMax;
						addrMin = best < addrMin ? best : addrMin;

					} else {
						printf("[%p] (%0.2f%%) [%d/%d] - Missing function (%p) to (%p) %s\n", best, (float)maxTotalCount * 100 / (*q).validLength, maxTotalCount, (*q).validLength, best, address, (*q).name.c_str());
						// Dump
						/*
						printf("Template: \n");
						DumpHex((*q).instr, INSTR_SIZE);
						printf("Target:   \n");
						DumpHex(best, INSTR_SIZE);
						*/
					}
				} else {
					printf("Entry: %s Missing!\n", (*q).name.c_str());
				}		
			}

/*
			mprotect(PAGE_ALIGN(addrMin) - PAGE_SIZE, PAGE_ALIGN(addrMax) - PAGE_ALIGN(addrMin) + PAGE_SIZE * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
			for (uint8_t* p = addrMin; p < addrMax; p += PROC_ALIGN) {
				if (*(p - 1) != 0xC3 && *(p - 1) != 0x90) continue;
				*p = 0xcc;
			}
			mprotect(PAGE_ALIGN(addrMin) - PAGE_SIZE, PAGE_ALIGN(addrMax) - PAGE_ALIGN(addrMin) + PAGE_SIZE * 2, PROT_READ | PROT_EXEC);*/

			for (std::vector<std::pair<uint8_t*, uint8_t*> >::const_iterator x = hookList.begin(); x != hookList.end(); ++x) {
				Hook((*x).first, (*x).second);
			}
		}
	}

    Bridge() {
		printf("DontStarveLuajit initialized!\n");
		from = dlopen(nullptr, RTLD_GLOBAL);
		to = dlopen("liblua51.so", RTLD_GLOBAL | RTLD_NOW);
		refer = dlopen("liblua51DS.so", RTLD_GLOBAL | RTLD_NOW);
		if (refer == nullptr || to == nullptr) {
			printf("Unable to load liblua51DS.so, `sudo apt-get install gcc:i386 lib32readline6` may help.\n");
			printf("If error still occurs, try to use `ldd liblua51DS.so` to find and install missing libs.\n");
		} else {
			printf("Main handle: %p\n", from);
			printf("Lua51 handle: %p\n", refer);
			printf("LuaJIT handle: %p\n", to);

			const char* funcs[] = {
				"luaL_loadfile", "luaL_newstate", "luaL_optinteger",
				"luaL_optlstring", "luaL_optnumber", 
			};

			for (size_t i = 0; i < sizeof(funcs) / sizeof(funcs[0]); i++) {
				missingFuncs.insert(funcs[i]);
			}

			nullfd = open("/dev/random", O_WRONLY);
			RedirectLuaProviderEntries();
			close(nullfd);
			dlclose(refer);
		}
    }

	std::unordered_set<std::string> missingFuncs;
	void* from;
	void* to;
	void* refer;
	int nullfd;
} theBridge;