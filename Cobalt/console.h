#pragma once

#include <fstream>
#include <inttypes.h>
#include <Windows.h>
#include <psapi.h>
#include <stdio.h>
#include <iostream>
#include <string>

template<class T>
struct TArray
{
	T* Data;
	int Count;
	int Max;
};

struct FString : public TArray<wchar_t>
{
	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		const auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

void* Result;

typedef FString* (__fastcall* fGetEngineVersion)
(
	void* Result
	);

static fGetEngineVersion GetEngineVersion;

std::string text;

class Util
{
private:
	static bool MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask)
	{
		for (PBYTE value = reinterpret_cast<PBYTE>(pBuffer); *lpMask; ++lpPattern, ++lpMask, ++value)
		{
			if (*lpMask == 'x' && *reinterpret_cast<LPCBYTE>(lpPattern) != *value)
				return false;
		}
		return true;
	}
public:
	static double EngineVersion()
	{
		std::string EngineVersion = text.substr(0, 4);

		return std::stod(EngineVersion);
	}

	static double FortniteVersion()
	{
		std::string FortniteVersion = text.substr(34, 4);

		return std::stod(FortniteVersion);
	}

	static uintptr_t FindPattern(PVOID pBase, DWORD dwSize, LPCSTR lpPattern, LPCSTR lpMask)
	{
		dwSize -= static_cast<DWORD>(strlen(lpMask));
		for (unsigned long index = 0; index < dwSize; ++index)
		{
			PBYTE pAddress = reinterpret_cast<PBYTE>(pBase) + index;
			if (MaskCompare(pAddress, lpPattern, lpMask))
				return reinterpret_cast<uintptr_t>(pAddress);
		}
		return NULL;
	}

	static uintptr_t FindPattern(LPCSTR lpPattern, LPCSTR lpMask)
	{
		MODULEINFO info = { 0 };
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));
		return FindPattern(info.lpBaseOfDll, info.SizeOfImage, lpPattern, lpMask);
	}

	static uintptr_t BaseAddress()
	{
		return reinterpret_cast<uintptr_t>(GetModuleHandle(0));
	}
};

struct UObject;

struct UClass;

struct UConsole;

struct FString;

struct UGameViewportClient
{
	char unknown1[0x40];
	UConsole* ViewportConsole;
};

struct UEngine420
{
	char unknown1[0xF8];
	UClass* ConsoleClass;
	char unknown2[0x620];
	UGameViewportClient* GameViewportClient;
};

struct UEngine421
{
	char unknown1[0xF8];
	UClass* ConsoleClass;
	char unknown2[0x621];
	UGameViewportClient* GameViewportClient;
};

struct UEngine4212
{
	char unknown1[0xF8];
	UClass* ConsoleClass;
	char unknown2[0x641];
	UGameViewportClient* GameViewportClient;
};

struct UEngine422
{
	char unknown1[0xF8];
	UClass* ConsoleClass;
	char unknown2[0x648];
	UGameViewportClient* GameViewportClient;
};

struct UEngine423
{
	char unknown1[0xF8];
	UClass* ConsoleClass;
	char unknown2[0x650];
	UGameViewportClient* GameViewportClient;
};

struct UEngine424
{
	char unknown1[0xF8];
	UClass* ConsoleClass;
	char unknown2[0x689];
	UGameViewportClient* GameViewportClient;
};

struct UEngine425
{
	char unknown1[0xF8];
	UClass* ConsoleClass;
	char unknown2[0x6C0];
	UGameViewportClient* GameViewportClient;
};

struct UEngine426
{
	char unknown1[0xF8];
	UClass* ConsoleClass;
	char unknown2[0x688];
	UGameViewportClient* GameViewportClient;
};

struct UEngine530
{
	char unknown1[0xF8];
	UClass* ConsoleClass;
	char unknown2[0x642];
	UGameViewportClient* GameViewportClient;
};

typedef UObject* (__fastcall* fStaticConstructObject_Internal)
(
	UClass* Class,
	UObject* InOuter,
	void* Name,
	int SetFlags,
	unsigned int InternalSetFlags,
	UObject* Template,
	bool  bCopyTransientsFromClassDefaults,
	void* InstanceGraph,
	bool  bAssumeTemplateIsArchetype
	);
static fStaticConstructObject_Internal StaticConstructObject_Internal;

static UConsole* ConstructConsole(UClass* ConsoleClass, UObject* outer)
{
	return reinterpret_cast<UConsole*>(StaticConstructObject_Internal(ConsoleClass, outer, nullptr, 0, 0, nullptr, false, nullptr, false));
}

static void EnableConsole(double EngineVersion, double FortniteVersion)
{
	static uintptr_t SCOIOffset = 0;
	unsigned long long SCOIAddress = 0;

	static uintptr_t GEngineOffset = 0;
	unsigned long long GEngineAddress = 0;

	switch ((int)(EngineVersion * 100))
	{
	default:
		printf("Unsupported engine version\n");
		break;

	case 420:
		SCOIOffset = Util::FindPattern("\xE8\x00\x00\x00\x00\x41\x8D\x4D\x60", "x????xxxx");
		SCOIAddress = SCOIOffset + 5 + *reinterpret_cast<int32_t*>(SCOIOffset + 1);
		StaticConstructObject_Internal = reinterpret_cast<fStaticConstructObject_Internal>(SCOIAddress);

		if (FortniteVersion < 3.5)
		{
			UEngine420* GEngine;
			GEngineOffset = Util::FindPattern("\x48\x8B\xD3\xE8\x00\x00\x00\x00\x48\x8B\xC8\x48\x89\x05\x00\x00\x00\x00\xE8", "xxxx????xxxxxx????x");
			GEngineAddress = GEngineOffset + 18 + *reinterpret_cast<int32_t*>(GEngineOffset + 14);
			GEngine = *reinterpret_cast<UEngine420**>(GEngineAddress);

			GEngine->GameViewportClient->ViewportConsole = ConstructConsole(GEngine->ConsoleClass, reinterpret_cast<UObject*>(GEngine->GameViewportClient));
		}
		else
		{
			UEngine421* GEngine;
			GEngineOffset = Util::FindPattern("\x48\x8B\xD9\x48\x8B\xF2\x48\x8B\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xF8\x48\x85\xC0", "xxxxxxxxx????x????xxxxxx");
			GEngineAddress = GEngineOffset + 13 + *reinterpret_cast<int32_t*>(GEngineOffset + 9);
			GEngine = *reinterpret_cast<UEngine421**>(GEngineAddress);

			GEngine->GameViewportClient->ViewportConsole = ConstructConsole(GEngine->ConsoleClass, reinterpret_cast<UObject*>(GEngine->GameViewportClient));
		}
		break;

	case 421:
		SCOIOffset = Util::FindPattern("\xE8\x00\x00\x00\x00\x41\x8D\x4D\x60", "x????xxxx");
		SCOIAddress = SCOIOffset + 5 + *reinterpret_cast<int32_t*>(SCOIOffset + 1);
		StaticConstructObject_Internal = reinterpret_cast<fStaticConstructObject_Internal>(SCOIAddress);

		if (FortniteVersion < 5.3)
		{
			UEngine421* GEngine;
			GEngineOffset = Util::FindPattern("\x48\x8B\xD1\x49\x89\x00\x00\x48\x8B\x0D\x00\x00\x00\x00\x00\x00\x00\x49\x89", "xxxxx??xxx???????xx");
			GEngineAddress = GEngineOffset + 14 + *reinterpret_cast<int32_t*>(GEngineOffset + 10);
			GEngine = *reinterpret_cast<UEngine421**>(GEngineAddress);

			GEngine->GameViewportClient->ViewportConsole = ConstructConsole(GEngine->ConsoleClass, reinterpret_cast<UObject*>(GEngine->GameViewportClient));
		}
		else if (FortniteVersion == 5.3)
		{
			UEngine530* GEngine = *reinterpret_cast<UEngine530**>(Util::BaseAddress() + 0x5485C28);
			GEngine->GameViewportClient->ViewportConsole = ConstructConsole(GEngine->ConsoleClass, reinterpret_cast<UObject*>(GEngine->GameViewportClient));
		}
		else
		{
			UEngine4212* GEngine;
			auto GEngineOffset = Util::FindPattern("\x49\x8B\xCC\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x48\x89\x05\x00\x00\x00\x00\x48\x85\xC9", "xxx????x????xx???xxx????xxx");
			auto GEngineAddress = GEngineOffset + 24 + *reinterpret_cast<int32_t*>(GEngineOffset + 20);
			GEngine = *reinterpret_cast<UEngine4212**>(GEngineAddress);

			GEngine->GameViewportClient->ViewportConsole = ConstructConsole(GEngine->ConsoleClass, reinterpret_cast<UObject*>(GEngine->GameViewportClient));
		}
		break;

	case 422:
		UEngine422 * GEngine;
		GEngineOffset = Util::FindPattern("\x48\x8B\xD3\x00\x00\x00\x00\x00\x48\x8B\x4C\x24\x40\x48\x89\x05\x00\x00\x00\x00\x48\x85\xC9", "xxx?????xxxxxxxx????xxx");
		GEngineAddress = GEngineOffset + 20 + *reinterpret_cast<int32_t*>(GEngineOffset + 16);
		GEngine = *reinterpret_cast<UEngine422**>(GEngineAddress);

		SCOIOffset = Util::FindPattern("\xE8\x00\x00\x00\x00\x44\x8D\x4D\x01", "x????xxxx");
		SCOIAddress = SCOIOffset + 5 + *reinterpret_cast<int32_t*>(SCOIOffset + 1);
		StaticConstructObject_Internal = reinterpret_cast<fStaticConstructObject_Internal>(SCOIAddress);

		GEngine->GameViewportClient->ViewportConsole = ConstructConsole(GEngine->ConsoleClass, reinterpret_cast<UObject*>(GEngine->GameViewportClient));
		break;

	case 423:
		UEngine423 * GEngine423;
		GEngineOffset = Util::FindPattern("\x48\x8B\xD3\x00\x00\x00\x00\x00\x48\x8B\x4C\x24\x40\x48\x89\x05\x00\x00\x00\x00\x48\x85\xC9", "xxx?????xxxxxxxx????xxx");
		GEngineAddress = GEngineOffset + 20 + *reinterpret_cast<int32_t*>(GEngineOffset + 16);
		GEngine423 = *reinterpret_cast<UEngine423**>(GEngineAddress);

		SCOIOffset = Util::FindPattern("\xE8\x00\x00\x00\x00\x44\x8D\x4D\x01", "x????xxxx");
		SCOIAddress = SCOIOffset + 5 + *reinterpret_cast<int32_t*>(SCOIOffset + 1);
		StaticConstructObject_Internal = reinterpret_cast<fStaticConstructObject_Internal>(SCOIAddress);

		GEngine423->GameViewportClient->ViewportConsole = ConstructConsole(GEngine423->ConsoleClass, reinterpret_cast<UObject*>(GEngine423->GameViewportClient));
		break;

	case 424:
		UEngine424 * GEngine424;
		GEngineOffset = Util::FindPattern("\x40\x57\x48\x81\x00\x00\x00\x00\x00\x48\x8B\x00\x41\xB8\x00\x00\x00\x00\x48\x8B\xD1\x48\x8B\x0D\x00\x00\x00\x00\x00\x00\x00\x00\x00\x48\x85\xC0\x0F\x00\x00\x00\x00\x00\x48\x8B\xC8", "xxxx?????xx?xx????xxxxxx?????????xxxx?????xxx");
		GEngineAddress = GEngineOffset + 28 + *reinterpret_cast<int32_t*>(GEngineOffset + 24);
		GEngine424 = *reinterpret_cast<UEngine424**>(GEngineAddress);

		SCOIAddress = Util::FindPattern("\x4C\x89\x44\x24\x18\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\x44\x8B\xA5\x00\x00\x00\x00", "xxxxxxxxxxxxxxxxxxxxx????xxx????xxx????xxxxxx????xxx????");
		StaticConstructObject_Internal = reinterpret_cast<fStaticConstructObject_Internal>(SCOIAddress);

		GEngine424->GameViewportClient->ViewportConsole = ConstructConsole(GEngine424->ConsoleClass, reinterpret_cast<UObject*>(GEngine424->GameViewportClient));
		break;

	case 425:
		UEngine425 * GEngine425;
		GEngineOffset = Util::FindPattern("\x48\x8B\xD3\x00\x00\x00\x00\x00\x48\x8B\x4C\x24\x40\x48\x89\x05\x00\x00\x00\x00\x48\x85\xC9", "xxx?????xxxxxxxx????xxx");
		GEngineAddress = GEngineOffset + 20 + *reinterpret_cast<int32_t*>(GEngineOffset + 16);
		GEngine425 = *reinterpret_cast<UEngine425**>(GEngineAddress);

		SCOIOffset = Util::FindPattern("\xE8\x00\x00\x00\x00\x8D\x57\x17\x48", "x????xxxx");
		SCOIAddress = SCOIOffset + 5 + *reinterpret_cast<int32_t*>(SCOIOffset + 1);
		StaticConstructObject_Internal = reinterpret_cast<fStaticConstructObject_Internal>(SCOIAddress);

		GEngine425->GameViewportClient->ViewportConsole = ConstructConsole(GEngine425->ConsoleClass, reinterpret_cast<UObject*>(GEngine425->GameViewportClient));
		break;

	case 426:
		SCOIAddress = Util::FindPattern("\x4C\x89\x44\x24\x18\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4", "xxxxxxxxxxxxxxxxxxxxx????xxx????xxx????xxx");
		StaticConstructObject_Internal = reinterpret_cast<fStaticConstructObject_Internal>(SCOIAddress);

		if (FortniteVersion < 14)
		{
			UEngine426* GEngine;
			GEngineOffset = Util::FindPattern("\x48\x8B\xD3\x00\x00\x00\x00\x00\x48\x8B\x4C\x24\x40\x48\x89\x05\x00\x00\x00\x00\x48\x85\xC9", "xxx?????xxxxxxxx????xxx");
			GEngineAddress = GEngineOffset + 20 + *reinterpret_cast<int32_t*>(GEngineOffset + 16);
			GEngine = *reinterpret_cast<UEngine426**>(GEngineAddress);

			GEngine->GameViewportClient->ViewportConsole = ConstructConsole(GEngine->ConsoleClass, reinterpret_cast<UObject*>(GEngine->GameViewportClient));
		}
		else
		{
			UEngine426* GEngine;
			auto GEngineOffset = Util::FindPattern("\x48\x89\x74\x24\x20\xE8\x00\x00\x00\x00\x48\x8B\x4C\x24\x40\x48\x89\x05", "xxxxxx????xxxxxxxx");
			auto GEngineAddress = GEngineOffset + 22 + *reinterpret_cast<int32_t*>(GEngineOffset + 18);
			GEngine = *reinterpret_cast<UEngine426**>(GEngineAddress);

			GEngine->GameViewportClient->ViewportConsole = ConstructConsole(GEngine->ConsoleClass, reinterpret_cast<UObject*>(GEngine->GameViewportClient));
		}
		break;
	}
}

DWORD WINAPI Unlockconsole(LPVOID param) {
	Sleep(15000); // works on most shit depends on loading time of the game
	uintptr_t GetEngineVersionAddress = Util::FindPattern("\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\xE8\x00\x00\x00\x00\x48\x8B\xC8\x41\xB8\x04\x00\x00\x00\x48\x8B\xD3", "xxxxxxxxxx????xxxxxxxxxxxx");
	GetEngineVersion = reinterpret_cast<fGetEngineVersion>(GetEngineVersionAddress);
	text = GetEngineVersion(&Result)->ToString().c_str();

	double EngineVersion = Util::EngineVersion();
	double FortniteVersion = Util::FortniteVersion();
	EnableConsole(EngineVersion, FortniteVersion);
	//printf("Console enabled\n\n");

	return NULL;
}