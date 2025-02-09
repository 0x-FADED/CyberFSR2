#pragma once

#include <Windows.h>
#include "..\external\Hooking.Patterns\Hooking.Patterns.h"

/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 *  Use of this source code is subject to the terms provided on the CitizenFX
 *  Collective web site, currently versioned (4) at the following web URL:
 *  https://fivem.net/terms
 */

namespace hook
{
	//taken most of this code is taken form cfx.re and modfified a little
	template<typename ValueType, typename AddressType>
	inline void put(AddressType address, ValueType value)
	{
		DWORD oldProtect;
		VirtualProtect((void*)address, sizeof(value), PAGE_EXECUTE_READWRITE, &oldProtect);

		std::memcpy((void*)address, &value, sizeof(value));

		VirtualProtect((void*)address, sizeof(value), oldProtect, &oldProtect);

		FlushInstructionCache(GetCurrentProcess(), (void*)address, sizeof(value));
	}

	template<typename AddressType>
	inline void nop(AddressType address, size_t length)
	{

		DWORD oldProtect;
		VirtualProtect((void*)address, length, PAGE_EXECUTE_READWRITE, &oldProtect);

		memset((void*)address, 0x90, length);

		VirtualProtect((void*)address, length, oldProtect, &oldProtect);
		FlushInstructionCache(GetCurrentProcess(), (void*)address, length);
	}

	template<typename T, typename TAddr>
	inline T get_address(TAddr address)
	{
		intptr_t target = *(int32_t*)(ptrdiff_t(address));
		target += (ptrdiff_t(address) + 4);

		return (T)target;
	}

	template<typename T>
	inline T get_call(T address)
	{
		intptr_t target = *(int32_t*)(uintptr_t(address) + 1);
		target += (uintptr_t(address) + 5);

		return (T)target;
	}

	//simple code to patch stuff
	template<typename T, size_t Bytes, typename AddressType>
	inline void patch(AddressType address, const T(&patch)[Bytes])
	{
		DWORD oldProtect;
		VirtualProtect(reinterpret_cast<void*>(address), std::size(patch), PAGE_EXECUTE_READWRITE, &oldProtect);

		std::memcpy(reinterpret_cast<void*>(address), patch, std::size(patch));

		VirtualProtect(reinterpret_cast<void*>(address), std::size(patch), oldProtect, &oldProtect);

		FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(address), std::size(patch));
	}

	//find patterns in a specific module
	template<typename T = void>
	inline auto get_module_pattern(const wchar_t* modulename, std::string_view pattern_string, ptrdiff_t offset = 0)
	{
		auto moduleHandle = GetModuleHandleW(modulename);

		if (moduleHandle != nullptr)
		{
			return pattern(moduleHandle, std::move(pattern_string)).get_first<T>(offset);
		}
	}

}