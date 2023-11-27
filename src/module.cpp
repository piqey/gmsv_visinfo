#include "GarrysMod/Lua/Interface.h"
#include "GarrysMod/FactoryLoader.hpp"
#include "eiface.h"

const SourceSDK::FactoryLoader engine_loader("engine", false, IS_SERVERSIDE, "bin/");
// const SourceSDK::ModuleLoader server_loader("server");

IVEngineServer* engine_server = nullptr;

//
// CLUSTERS
//

LUA_FUNCTION_STATIC(GetClusterForOrigin)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::Vector);
	LUA->PushNumber((double)engine_server->GetClusterForOrigin(LUA->GetVector(1)));

	return 1;
}

LUA_FUNCTION_STATIC(GetClusterCount)
{
	LUA->PushNumber((double)engine_server->GetClusterCount());

	return 1;
}

//
// AREAS
//

LUA_FUNCTION_STATIC(GetArea)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::Vector);
	LUA->PushNumber((double)engine_server->GetArea(LUA->GetVector(1)));

	return 1;
}

LUA_FUNCTION_STATIC(CheckAreasConnected)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);
	LUA->CheckType(2, GarrysMod::Lua::Type::Number);

	LUA->PushNumber((double)engine_server->CheckAreasConnected((int)LUA->GetNumber(1), (int)LUA->GetNumber(2)));

	return 1;
}

GMOD_MODULE_OPEN()
{
	engine_server = engine_loader.GetInterface<IVEngineServer>(INTERFACEVERSION_VENGINESERVER);

	if (engine_server == nullptr)
		LUA->ThrowError("Failed to resolve server-side engine interface!");

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);

	LUA->PushCFunction(GetClusterForOrigin);
	LUA->SetField(-2, "GetClusterForOrigin");

	LUA->PushCFunction(GetClusterCount);
	LUA->SetField(-2, "GetClusterCount");

	LUA->PushCFunction(GetArea);
	LUA->SetField(-2, "GetArea");

	LUA->PushCFunction(CheckAreasConnected);
	LUA->SetField(-2, "CheckAreasConnected");

	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}
