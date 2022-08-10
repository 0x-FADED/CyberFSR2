#include "hooking.h"
void hooks()
{
	auto location = hook::get_module_pattern<uint8_t>(L"HorizonZeroDawn.exe", "48 8B D9 83 78 78 01", 7);
	hook::nop(location, 6);
}