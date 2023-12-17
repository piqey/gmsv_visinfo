#include "module.hpp"

// CLUSTERS

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

LUA_FUNCTION_STATIC(GetAllClusterBounds)
{
	int size = engine_server->GetClusterCount();
	bbox_t* bboxes = new bbox_t[size];

	engine_server->GetAllClusterBounds(bboxes, size);

	LUA->CreateTable();

	for (int i = 0; i < size; i++)
	{
		LUA->PushNumber((double)(i + 1));
		LUA->CreateTable();

		LUA->PushVector(bboxes[i].mins);
		LUA->SetField(-2, "mins");

		LUA->PushVector(bboxes[i].maxs);
		LUA->SetField(-2, "maxs");

		LUA->SetTable(-3);
	}

	delete[] bboxes;

	return 1;
}

// PVS

LUA_FUNCTION_STATIC(GetPVSForCluster)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::Number);

	int size = (int)ceil(engine_server->GetClusterCount() / 8.0f);
	byte* buffer = new byte[size];

	engine_server->GetPVSForCluster((int)LUA->GetNumber(1), size, buffer);
	// LuaPrint(LUA, "Got buffer size of", size);

	LUA->PushUserType(new PVSData(size, buffer), PVSData::meta);

	LUA->PushMetaTable(PVSData::meta);
	LUA->SetMetaTable(-2);

	LUA->CreateTable();
	LUA->SetFEnv(-2);

	return 1;
}

// AREAS

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

	PVSData::Initialize(LUA);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);

	// CLUSTERS

	LUA->PushCFunction(GetClusterForOrigin);
	LUA->SetField(-2, "GetClusterForOrigin");

	LUA->PushCFunction(GetClusterCount);
	LUA->SetField(-2, "GetClusterCount");

	LUA->PushCFunction(GetAllClusterBounds);
	LUA->SetField(-2, "GetAllClusterBounds");

	// PVS

	LUA->PushCFunction(GetPVSForCluster);
	LUA->SetField(-2, "GetPVSForCluster");

	// AREAS

	LUA->PushCFunction(GetArea);
	LUA->SetField(-2, "GetArea");

	LUA->PushCFunction(CheckAreasConnected);
	LUA->SetField(-2, "CheckAreasConnected");

	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE()
{
	PVSData::Deinitialize(LUA);

	return 0;
}
