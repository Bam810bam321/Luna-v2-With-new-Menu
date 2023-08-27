// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <ShlObj.h>
#include <ShlObj_core.h>
#include "includes.hpp"
#include "utils\ctx.hpp"
#include "utils\recv.h"
#include "utils\imports.h"
//#include "utils\anti_debug.h"
#include "cheats/lagcompensation/local_animations.h"
#include "steam/steam_api.h"
#include "nSkinz\SkinChanger.h"
#include "cheats/menu.h"

PVOID base_address = nullptr;

__forceinline void crash(bool debug = false);
__forceinline void setup_render();
__forceinline void setup_netvars();
__forceinline void setup_skins();
__forceinline void setup_steam();
__forceinline void setup_hooks();

__forceinline void setup_steam()
{
	typedef uint32_t SteamPipeHandle;
	typedef uint32_t SteamUserHandle;

	SteamUserHandle hSteamUser = ((SteamUserHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamUser"))();
	SteamPipeHandle hSteamPipe = ((SteamPipeHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamPipe"))();

	SteamClient = ((ISteamClient * (__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamClient"))();
	SteamGameCoordinator = (ISteamGameCoordinator*)SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, "SteamGameCoordinator001");
	SteamUser = (ISteamUser*)SteamClient->GetISteamUser(hSteamUser, hSteamPipe, "SteamUser019");
	SteamFriends = SteamClient->GetISteamFriends(hSteamUser, hSteamPipe, "SteamFriends015");
	SteamUtils = SteamClient->GetISteamUtils(hSteamPipe, "SteamUtils009");
}

DWORD WINAPI main(PVOID base)
{
	/* wait for all modules to load */
	while (!GetModuleHandleA(crypt_str("serverbrowser.dll")))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	g_ctx.signatures =
	{
			crypt_str("A1 ? ? ? ? 50 8B 08 FF 51 0C"),
			crypt_str("B9 ?? ?? ?? ?? A1 ?? ?? ?? ?? FF 10 A1 ?? ?? ?? ?? B9"),
			crypt_str("0F 11 05 ?? ?? ?? ?? 83 C8 01"),
			crypt_str("8B 0D ?? ?? ?? ?? 8B 46 08 68"),
			crypt_str("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10"),
			crypt_str("8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7"),
			crypt_str("A1 ? ? ? ? 8B 0D ? ? ? ? 6A 00 68 ? ? ? ? C6"),
			crypt_str("80 3D ? ? ? ? ? 53 56 57 0F 85"),
			crypt_str("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C"),
			crypt_str("80 3D ? ? ? ? ? 74 06 B8"),
			crypt_str("55 8B EC 83 E4 F0 B8 D8"),
			crypt_str("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C"),
			crypt_str("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6"),
			crypt_str("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"),
			crypt_str("56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 23"),
			crypt_str("55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14 83 7F 60"),
			crypt_str("55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9"),
			crypt_str("57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"),
			crypt_str("55 8B EC 81 EC ? ? ? ? 53 8B D9 89 5D F8"),
			crypt_str("53 0F B7 1D ? ? ? ? 56"),
			crypt_str("8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6"),
			crypt_str("8B 35 ? ? ? ? FF 10 0F B7 C0")
	};

	g_ctx.indexes =
	{
		5,
		33,
		340,
		219,
		220,
		34,
		158,
		75,
		461,
		483,
		453,
		484,
		285,
		224,
		247,
		27,
		17,
		123
	};

	while (!IFH(GetModuleHandle)(crypt_str("serverbrowser.dll")))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	CreateDirectory(crypt_str("C:\\Medusa.uno\\"), NULL);
	CreateDirectory(crypt_str("C:\\Medusa.uno\\Configs\\"), NULL);
	CreateDirectory(crypt_str("C:\\Medusa.uno\\Scripts\\"), NULL);
	base_address = base;

	setup_sounds();

	setup_skins();

	setup_netvars();

	setup_render();

	cfg_manager->setup();

	c_lua::get().initialize();

	key_binds::get().initialize_key_binds();

	setup_hooks();
	Netvars::Netvars();

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	return EXIT_SUCCESS;
}

void crash_log(const char* text, ...)
{
	if (!text)
		return;

	char path[MAX_PATH];
	IFH(GetEnvironmentVariable)(crypt_str("USERPROFILE"), path, MAX_PATH);

	strcat(path, crypt_str("\\Desktop\\crash.log"));
	auto file = std::ofstream(path, std::ios::app);

	if (!file.is_open())
		return;

	va_list va_alist;
	char buffer[1024] = "\0";

	va_start(va_alist, text);
	_vsnprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), text, va_alist); //-V2007
	va_end(va_alist);

	file << buffer << std::endl;
	file.close();
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		IFH(DisableThreadLibraryCalls)(hModule);

		auto current_process = IFH(GetCurrentProcess)();
		auto priority_class = IFH(GetPriorityClass)(current_process);

		if (priority_class != HIGH_PRIORITY_CLASS && priority_class != REALTIME_PRIORITY_CLASS)
			IFH(SetPriorityClass)(current_process, HIGH_PRIORITY_CLASS);

		CreateThread(nullptr, 0, main, hModule, 0, nullptr); //-V718 //-V513
	}

	return TRUE;
}

extern "C" NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN OldValue);
extern "C" NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);

__forceinline void crash(bool debug)
{
	g_ctx.signatures.clear();
	g_ctx.indexes.clear();
	g_ctx.username.clear();

	if (debug)
	{
		BOOLEAN OldValue;
		RtlAdjustPrivilege(19, TRUE, FALSE, &OldValue);

		ULONG Response;
		NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, nullptr, 6, &Response);
	}

	MODULEINFO module_info;
	IFH(GetModuleInformation)(IFH(GetCurrentProcess)(), IFH(GetModuleHandle)(crypt_str("client.dll")), &module_info, sizeof(MODULEINFO));

	auto address = (DWORD)module_info.lpBaseOfDll;

	while (true) //-V776
	{
		*(DWORD*)(address) = 0;
		++address;
	}
}

vmthook* sv_cheats;
using GetBool_t = bool(__thiscall*)(void*);
bool __stdcall sv_cheatshk() {
	const auto return_address = reinterpret_cast<DWORD>(_ReturnAddress());
	static const auto cam_think = util::FindSignature("client.dll", "85 C0 75 30 38 87");

	if (return_address == cam_think)
		return true;

	return sv_cheats->get_func_address<GetBool_t>(13);
}
__forceinline void setup_render()
{
	static auto create_font = [](const char* name, int size, int weight, DWORD flags) -> vgui::HFont
	{
		g_ctx.last_font_name = name;

		auto font = m_surface()->FontCreate();
		m_surface()->SetFontGlyphSet(font, name, size, weight, 0, 0, flags);

		return font;
	};

	fonts[LOGS] = create_font(crypt_str("Lucida Console"), 10, FW_MEDIUM, FONTFLAG_DROPSHADOW);
	fonts[ESP] = create_font(crypt_str("Smallest Pixel-7"), 11, FW_MEDIUM, FONTFLAG_OUTLINE);
	fonts[NAME] = create_font(crypt_str("Verdana"), 12, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[SUBTABWEAPONS] = create_font(crypt_str("undefeated"), 13, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[KNIFES] = create_font(crypt_str("icomoon"), 13, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[GRENADES] = create_font(crypt_str("undefeated"), 20, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[INDICATORFONT] = create_font(crypt_str("Verdana"), 25, FW_HEAVY, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[DAMAGE_MARKER] = create_font(crypt_str("CrashNumberingGothic"), 15, FW_HEAVY, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);

	g_ctx.last_font_name.clear();
}

__forceinline void setup_netvars()
{
	netvars::get().tables.clear();
	auto client = m_client()->GetAllClasses();

	if (!client)
		return;

	while (client)
	{
		auto recvTable = client->m_pRecvTable;

		if (recvTable)
			netvars::get().tables.emplace(std::string(client->m_pNetworkName), recvTable);

		client = client->m_pNext;
	}
}

__forceinline void setup_skins()
{
	auto items = std::ifstream(crypt_str("csgo/scripts/items/items_game_cdn.txt"));
	auto gameItems = std::string(std::istreambuf_iterator <char> { items }, std::istreambuf_iterator <char> { });

	if (!items.is_open())
		return;

	items.close();
	memory.initialize();

	for (auto i = 0; i <= memory.itemSchema()->paintKits.lastElement; i++)
	{
		auto paintKit = memory.itemSchema()->paintKits.memory[i].value;

		if (paintKit->id == 9001)
			continue;

		auto itemName = m_localize()->FindSafe(paintKit->itemName.buffer + 1);
		auto itemNameLength = WideCharToMultiByte(CP_UTF8, 0, itemName, -1, nullptr, 0, nullptr, nullptr);

		if (std::string name(itemNameLength, 0); WideCharToMultiByte(CP_UTF8, 0, itemName, -1, &name[0], itemNameLength, nullptr, nullptr))
		{
			if (paintKit->id < 10000)
			{
				if (auto pos = gameItems.find('_' + std::string{ paintKit->name.buffer } + '='); pos != std::string::npos && gameItems.substr(pos + paintKit->name.length).find('_' + std::string{ paintKit->name.buffer } + '=') == std::string::npos)
				{
					if (auto weaponName = gameItems.rfind(crypt_str("weapon_"), pos); weaponName != std::string::npos)
					{
						name.back() = ' ';
						name += '(' + gameItems.substr(weaponName + 7, pos - weaponName - 7) + ')';
					}
				}
				SkinChanger::skinKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
			else
			{
				std::string_view gloveName{ paintKit->name.buffer };
				name.back() = ' ';
				name += '(' + std::string{ gloveName.substr(0, gloveName.find('_')) } + ')';
				SkinChanger::gloveKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
		}
	}

	std::sort(SkinChanger::skinKits.begin(), SkinChanger::skinKits.end());
	std::sort(SkinChanger::gloveKits.begin(), SkinChanger::gloveKits.end());
}


struct ModuleInfo
{
	void* base;
	std::size_t size;
};

[[nodiscard]] static auto generateBadCharTable(std::string_view pattern) noexcept
{
	assert(!pattern.empty());

	std::array<std::size_t, (std::numeric_limits<std::uint8_t>::max)() + 1> table;

	auto lastWildcard = pattern.rfind('?');
	if (lastWildcard == std::string_view::npos)
		lastWildcard = 0;

	const auto defaultShift = (std::max)(std::size_t(1), pattern.length() - 1 - lastWildcard);
	table.fill(defaultShift);

	for (auto i = lastWildcard; i < pattern.length() - 1; ++i)
		table[static_cast<std::uint8_t>(pattern[i])] = pattern.length() - 1 - i;

	return table;
}

static ModuleInfo getModuleInformation(const char* name) noexcept
{
	if (HMODULE handle = GetModuleHandleA(name))
	{
		if (MODULEINFO moduleInfo; GetModuleInformation(GetCurrentProcess(), handle, &moduleInfo, sizeof(moduleInfo)))
			return ModuleInfo{ moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage };
	}
	return {};
}

template <bool ReportNotFound = true>
static std::uintptr_t findPattern31(ModuleInfo moduleInfo, std::string_view pattern) noexcept
{
	static auto id = 0;
	++id;

	if (moduleInfo.base && moduleInfo.size)
	{
		const auto lastIdx = pattern.length() - 1;
		const auto badCharTable = generateBadCharTable(pattern);

		auto start = static_cast<const char*>(moduleInfo.base);
		const auto end = start + moduleInfo.size - pattern.length();

		while (start <= end) {
			int i = lastIdx;
			while (i >= 0 && (pattern[i] == '?' || start[i] == pattern[i]))
				--i;

			if (i < 0)
				return reinterpret_cast<std::uintptr_t>(start);

			start += badCharTable[static_cast<std::uint8_t>(start[lastIdx])];
		}
	}

	assert(false);
#ifdef _WIN32
	if constexpr (ReportNotFound)
		MessageBoxA(nullptr, ("Failed to find pattern #" + std::to_string(id) + '!').c_str(), "Osiris", MB_OK | MB_ICONWARNING);
#endif
	return 0;
}

template <bool ReportNotFound = true>
static std::uintptr_t findPattern(const char* moduleName, std::string_view pattern) noexcept
{
	return findPattern31<ReportNotFound>(getModuleInformation(moduleName), pattern);
}

std::uintptr_t newFunctionClientDLL;
std::uintptr_t newFunctionEngineDLL;
std::uintptr_t newFunctionStudioRenderDLL;
std::uintptr_t newFunctionMaterialSystemDLL;

void fix()
{
	newFunctionClientDLL = findPattern("client", "\x55\x8B\xEC\x56\x8B\xF1\x33\xC0\x57\x8B\x7D\x08");
	newFunctionEngineDLL = findPattern("engine", "\x55\x8B\xEC\x56\x8B\xF1\x33\xC0\x57\x8B\x7D\x08");
	newFunctionStudioRenderDLL = findPattern("studiorender", "\x55\x8B\xEC\x56\x8B\xF1\x33\xC0\x57\x8B\x7D\x08");
	newFunctionMaterialSystemDLL = findPattern("materialsystem", "\x55\x8B\xEC\x56\x8B\xF1\x33\xC0\x57\x8B\x7D\x08");
}

DWORD newFunctionClientDLL_hook;
DWORD newFunctionEngineDLL_hook;
DWORD newFunctionStudioRenderDLL_hook;
DWORD newFunctionMaterialSystemDLL_hook;

static char __fastcall newFunctionClientBypass(void* thisPointer, void* edx, const char* moduleName) noexcept
{
	return 1;
}

static char __fastcall newFunctionEngineBypass(void* thisPointer, void* edx, const char* moduleName) noexcept
{
	return 1;
}

static char __fastcall newFunctionStudioRenderBypass(void* thisPointer, void* edx, const char* moduleName) noexcept
{
	return 1;
}

static char __fastcall newFunctionMaterialSystemBypass(void* thisPointer, void* edx, const char* moduleName) noexcept
{
	return 1;
}

using eye_angle_fix = Vector * (__thiscall*)(void*);
inline eye_angle_fix _yes_fuck;


void* pBuildTransformationsEntity;
Vector vecBuildTransformationsAngles;
Vector* __fastcall hkGetEyeAngles(void* ecx, void* edx)
{
    static int* WantedReturnAddress = (int*)util::FindSignature("client.dll", "8B 55 0C 8B C8 E8 ? ? ? ? 83 C4 08 5E 8B E5");

    static auto oGetEyeAngles = hooks::player_hook->get_func_address <eye_angle_fix>(170);

    if (_ReturnAddress() != WantedReturnAddress)
        return oGetEyeAngles(ecx);

    if (!ecx || pBuildTransformationsEntity != ecx)
        return oGetEyeAngles(ecx);

    pBuildTransformationsEntity = nullptr;

    return &vecBuildTransformationsAngles;
}
using BuildTransformations = Vector * (__thiscall*)(void*, CStudioHdr* hdr, Vector* pos, Quaternion* q, matrix3x4_t* cameraTransform, int bonemask, byte* computed);
void __fastcall hkBuildTransformations(void* ecx, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion* q, matrix3x4_t* cameraTransform, int bonemask, byte* computed) {

    static auto oBuildTransformations = hooks::player_hook->get_func_address <BuildTransformations>(190);
    if (ecx && ((player_t*)ecx)->EntIndex() == m_engine()->GetLocalPlayer()) {

        pBuildTransformationsEntity = ecx;
        vecBuildTransformationsAngles = ((player_t*)ecx)->GetRenderAngles();
    }
    oBuildTransformations(ecx, hdr, pos, q, cameraTransform, bonemask, computed);
    pBuildTransformationsEntity = nullptr;
}



__forceinline void setup_hooks()
{
	fix();


	//playerhook fix by drip
    auto playerhooks = util::FindSignature("client.dll", ("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C")) + 0x47;
    hooks::player_hook = new vmthook(reinterpret_cast<DWORD**>(playerhooks));
    hooks::player_hook->hook_function(reinterpret_cast<uintptr_t>(hkBuildTransformations), 190);
    hooks::player_hook->hook_function(reinterpret_cast<uintptr_t>(hkGetEyeAngles), 170);
	
	newFunctionClientDLL_hook = (DWORD)DetourFunction((PBYTE)newFunctionClientDLL, (PBYTE)newFunctionClientBypass);
	newFunctionEngineDLL_hook = (DWORD)DetourFunction((PBYTE)newFunctionEngineDLL, (PBYTE)newFunctionEngineBypass);
	newFunctionStudioRenderDLL_hook = (DWORD)DetourFunction((PBYTE)newFunctionStudioRenderDLL, (PBYTE)newFunctionStudioRenderBypass);
	newFunctionMaterialSystemDLL_hook = (DWORD)DetourFunction((PBYTE)newFunctionMaterialSystemDLL, (PBYTE)newFunctionMaterialSystemBypass);

	const char* game_modules[]{ "client.dll", "engine.dll", "server.dll", "studiorender.dll", "materialsystem.dll", "shaderapidx9.dll", "vstdlib.dll", "vguimatsurface.dll" };
	long long patch = 0x69690004C201B0;

	for (auto current_module : game_modules)
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)util::FindSignature(current_module, "55 8B EC 56 8B F1 33 C0 57 8B 7D 08"), &patch, 7, 0);

	static auto getforeignfallbackfontname = (DWORD)(util::FindSignature(crypt_str("vguimatsurface.dll"), g_ctx.signatures.at(9).c_str()));
	hooks::original_getforeignfallbackfontname = (DWORD)DetourFunction((PBYTE)getforeignfallbackfontname, (PBYTE)hooks::hooked_getforeignfallbackfontname);

	static auto setupbones = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(10).c_str()));
	hooks::original_setupbones = (DWORD)DetourFunction((PBYTE)setupbones, (PBYTE)hooks::hooked_setupbones);

	static auto eye_angles = (DWORD)(util::FindSignature(crypt_str("client.dll"), "56 8B F1 85 F6 74 32"));
	_eye_angles = (eye_angles_valve)DetourFunction((PBYTE)eye_angles, (PBYTE)hkGetEyeAngles);

	static auto clmove = (DWORD)(util::FindSignature(crypt_str("engine.dll"), crypt_str("55 8B EC 81 EC 64 01 00 00 53 56 8A F9")));
	hooks::original_clmove = (DWORD)DetourFunction((PBYTE)clmove, (PBYTE)hooks::hooked_clmove);

	static auto doextrabonesprocessing = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(11).c_str()));
	hooks::original_doextrabonesprocessing = (DWORD)DetourFunction((PBYTE)doextrabonesprocessing, (PBYTE)hooks::hooked_doextrabonesprocessing);

	static auto standardblendingrules = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(12).c_str()));
	hooks::original_standardblendingrules = (DWORD)DetourFunction((PBYTE)standardblendingrules, (PBYTE)hooks::hooked_standardblendingrules);

	static auto updateclientsideanimation = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(13).c_str()));
	hooks::original_updateclientsideanimation = (DWORD)DetourFunction((PBYTE)updateclientsideanimation, (PBYTE)hooks::hooked_updateclientsideanimation);

	static auto physicssimulate = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(14).c_str()));
	hooks::original_physicssimulate = (DWORD)DetourFunction((PBYTE)physicssimulate, (PBYTE)hooks::hooked_physicssimulate);

	static auto modifyeyeposition = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(15).c_str()));
	hooks::original_modifyeyeposition = (DWORD)DetourFunction((PBYTE)modifyeyeposition, (PBYTE)hooks::hooked_modifyeyeposition);

	static auto calcviewmodelbob = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(16).c_str()));
	hooks::original_calcviewmodelbob = (DWORD)DetourFunction((PBYTE)calcviewmodelbob, (PBYTE)hooks::hooked_calcviewmodelbob);

	static auto shouldskipanimframe = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(17).c_str()));
	DetourFunction((PBYTE)shouldskipanimframe, (PBYTE)hooks::hooked_shouldskipanimframe);

	static auto checkfilecrcswithserver = (DWORD)(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(18).c_str()));
	DetourFunction((PBYTE)checkfilecrcswithserver, (PBYTE)hooks::hooked_checkfilecrcswithserver);

	static auto processinterpolatedlist = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(19).c_str()));
	hooks::original_processinterpolatedlist = (DWORD)DetourFunction((byte*)processinterpolatedlist, (byte*)hooks::processinterpolatedlist);

	hooks::client_hook = new vmthook(reinterpret_cast<DWORD**>(m_client()));
	hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_createmove_proxy), 22);
	hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_writeusercmddeltatobuffer), 24);
	hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_fsn), 37);

	hooks::clientstate_hook = new vmthook(reinterpret_cast<DWORD**>((CClientState*)(uint32_t(m_clientstate()) + 0x8)));
	hooks::clientstate_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_packetstart), 5);
	hooks::clientstate_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_packetend), 6);

	hooks::panel_hook = new vmthook(reinterpret_cast<DWORD**>(m_panel()));
	hooks::panel_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_painttraverse), 41);

	hooks::clientmode_hook = new vmthook(reinterpret_cast<DWORD**>(m_clientmode()));
	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_postscreeneffects), 44);
	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_overrideview), 18);
	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_drawfog), 17);

	hooks::inputinternal_hook = new vmthook(reinterpret_cast<DWORD**>(m_inputinternal()));
	hooks::inputinternal_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_setkeycodestate), 91);
	hooks::inputinternal_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_setmousecodestate), 92);

	hooks::engine_hook = new vmthook(reinterpret_cast<DWORD**>(m_engine()));
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_isconnected), 27);
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_getscreenaspectratio), 101);
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_ishltv), 93);

	hooks::renderview_hook = new vmthook(reinterpret_cast<DWORD**>(m_renderview()));
	hooks::renderview_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_sceneend), 9);

	hooks::materialsys_hook = new vmthook(reinterpret_cast<DWORD**>(m_materialsystem()));
	hooks::materialsys_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_beginframe), 42);
	hooks::materialsys_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_getmaterial), 84);

	hooks::modelrender_hook = new vmthook(reinterpret_cast<DWORD**>(m_modelrender()));
	hooks::modelrender_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_dme), 21);

	hooks::surface_hook = new vmthook(reinterpret_cast<DWORD**>(m_surface()));
	hooks::surface_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_lockcursor), 67);

	hooks::bspquery_hook = new vmthook(reinterpret_cast<DWORD**>(m_engine()->GetBSPTreeQuery()));
	hooks::bspquery_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_listleavesinbox), 6);

	hooks::prediction_hook = new vmthook(reinterpret_cast<DWORD**>(m_prediction()));
	hooks::prediction_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_runcommand), 19);

	auto sv_cheats_cvar = m_cvar()->FindVar("sv_cheats");
	sv_cheats = new vmthook(reinterpret_cast<DWORD**>(sv_cheats_cvar));
	sv_cheats->hook_function(reinterpret_cast<uintptr_t>(sv_cheatshk), 13);

	hooks::trace_hook = new vmthook(reinterpret_cast<DWORD**>(m_trace()));
	hooks::trace_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_clip_ray_collideable), 4);
	hooks::trace_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_trace_ray), 5);

	hooks::filesystem_hook = new vmthook(reinterpret_cast<DWORD**>(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(20).c_str()) + 0x2));
	hooks::filesystem_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_loosefileallowed), 128);

	while (!(INIT::Window = IFH(FindWindow)(crypt_str("Valve001"), nullptr)))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	INIT::OldWindow = (WNDPROC)IFH(SetWindowLongPtr)(INIT::Window, GWL_WNDPROC, (LONG_PTR)hooks::Hooked_WndProc);

	hooks::directx_hook = new vmthook(reinterpret_cast<DWORD**>(m_device()));
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::Hooked_EndScene_Reset), 16);
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_present), 17);
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::Hooked_EndScene), 42);

	hooks::hooked_events.RegisterSelf();

}
