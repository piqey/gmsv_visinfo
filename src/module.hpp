#include "GarrysMod/Lua/Interface.h"
#include "GarrysMod/FactoryLoader.hpp"
#include "eiface.h"

static inline void LuaPrint(GarrysMod::Lua::ILuaBase* L, char* msg)
{
	L->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); // Fetch the global table
	L->GetField(-1, "print"); // Fetch the print function
	L->PushString(msg); // Push the string argument
	L->Call(1, 0); // Call the function with 1 argument and no return value
	L->Pop(); // Pop global table
}

static inline void LuaPrint(GarrysMod::Lua::ILuaBase* L, char* msg, int num)
{
	L->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); // Fetch the global table
	L->GetField(-1, "print"); // Fetch the print function
	L->PushString(msg); // Push the string argument
	L->PushNumber((double)num);
	L->Call(2, 0); // Call the function with 1 argument and no return value
	L->Pop(); // Pop global table
}

const SourceSDK::FactoryLoader engine_loader("engine");
IVEngineServer* engine_server = nullptr;
