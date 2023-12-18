#include "PVS.hpp"

namespace VisInfo::Types
{
	// C++ type

	PVSData::PVSData(int size, byte* buffer) :
		size(size), total(engine_server->GetClusterCount()), buffer(buffer)
	{
		if (buffer == nullptr)
			throw std::runtime_error("PVS byte buffer is a null pointer!");

		for (int i = 0; i < total; i++)
			if (buffer[i / 8] & (1 << (i % 8)))
				visible++;
	}

	PVSData::~PVSData()
	{
		delete[] buffer;
	}

	bool PVSData::CheckOrigin(Vector origin) const
	{
		return engine_server->CheckOriginInPVS(origin, buffer, size);
	}

	bool PVSData::CheckBox(Vector mins, Vector maxs) const
	{
		return engine_server->CheckBoxInPVS(mins, maxs, buffer, size);
	}

	bool PVSData::ContainsCluster(int cluster) const
	{
		if (buffer == nullptr)
			throw std::runtime_error("PVS byte buffer is a null pointer!");

		if (cluster < 0 || cluster >= total)
			throw std::out_of_range("Cluster ID out of range!");

		return (buffer[cluster / 8] & (1 << (cluster % 8))) != 0;
	}

	// Lua UserType stuff

	LUA_FUNCTION_STATIC(index)
	{
		LUA->CheckType(1, PVSData::meta);

		if (LUA->IsType(2, GarrysMod::Lua::Type::Number)) // Return boolean visibility when passed a cluster ID
		{
			LUA->GetMetaTable(1); // Get the PVS metatable
			LUA->GetField(-1, "ContainsCluster"); // Push the ContainsCluster method onto the stack

			LUA->Remove(-2); // Remove the metatable from the stack, keeping the function

			LUA->Push(1); // Push the PVS userdata from the original function call
			LUA->Push(2); // Push the second argument from the original function call

			LUA->Call(2, 1); // Call the function; 2 arguments, 1 result
		}
		else // Return stuff normally
		{
			LUA->GetMetaTable(1);
			LUA->Push(2);
			LUA->RawGet(-2);

			if (LUA->IsType(-1, GarrysMod::Lua::Type::NIL))
			{
				LUA->GetFEnv(1);
				LUA->Push(2);
				LUA->RawGet(-2);
			}
		}

		return 1;
	}

	LUA_FUNCTION_STATIC(len) // TODO(?): Should this return the total amount visible or the total amount...?
	{
		LUA->CheckType(1, PVSData::meta);
		LUA->PushNumber((double)LUA->GetUserType<PVSData>(1, PVSData::meta)->visible /* total */);

		return 1;
	}

	LUA_FUNCTION_STATIC(tostring)
	{
		LUA->CheckType(1, PVSData::meta);

		PVSData* pvs = LUA->GetUserType<PVSData>(1, PVSData::meta);
		LUA->PushFormattedString("[%s] %f total clusters, %f visible", PVSData::typeName, (double)pvs->total, (double)pvs->visible);

		return 1;
	}

	LUA_FUNCTION_STATIC(CheckOrigin)
	{
		LUA->CheckType(1, PVSData::meta);
		LUA->CheckType(2, GarrysMod::Lua::Type::Vector);

		LUA->PushBool(LUA->GetUserType<PVSData>(1, PVSData::meta)->CheckOrigin(LUA->GetVector(2)));

		return 1;
	}

	LUA_FUNCTION_STATIC(CheckBox)
	{
		LUA->CheckType(1, PVSData::meta);
		LUA->CheckType(2, GarrysMod::Lua::Type::Vector);
		LUA->CheckType(3, GarrysMod::Lua::Type::Vector);

		LUA->PushBool(LUA->GetUserType<PVSData>(1, PVSData::meta)->CheckBox(LUA->GetVector(2), LUA->GetVector(3)));

		return 1;
	}

	LUA_FUNCTION_STATIC(ContainsCluster)
	{
		LUA->CheckType(1, PVSData::meta);
		LUA->CheckNumber(2);

		PVSData* pvs = LUA->GetUserType<PVSData>(1, PVSData::meta);

		try { LUA->PushBool(pvs->ContainsCluster((int)LUA->GetNumber(2))); }
		catch (const std::runtime_error& e) { LUA->ThrowError(e.what()); }
		catch (const std::out_of_range& e) { LUA->FormattedError("%s Use a number between %f and %f.", e.what(), 0.0, (double)(pvs->total - 1)); }

		return 1;
	}

	LUA_FUNCTION_STATIC(GetTotalClusters)
	{
		LUA->CheckType(1, PVSData::meta);
		LUA->PushNumber(LUA->GetUserType<PVSData>(1, PVSData::meta)->total);

		return 1;
	}

	const char* PVSData::typeName = "PVS";
	int PVSData::meta = 0;

	void PVSData::Initialize(GarrysMod::Lua::ILuaBase* L)
	{
		meta = L->CreateMetaTable(typeName);

		L->PushCFunction(index);
		L->SetField(-2, "__index");

		L->PushCFunction(len);
		L->SetField(-2, "__len");

		L->PushCFunction(tostring);
		L->SetField(-2, "__tostring");

		L->PushCFunction(VisInfo::Types::CheckOrigin);
		L->SetField(-2, "CheckOrigin");

		L->PushCFunction(VisInfo::Types::CheckBox);
		L->SetField(-2, "CheckBox");

		L->PushCFunction(VisInfo::Types::ContainsCluster);
		L->SetField(-2, "ContainsCluster");

		L->PushCFunction(GetTotalClusters);
		L->SetField(-2, "GetTotalClusters");

		L->Pop();
	}

	void PVSData::Deinitialize(GarrysMod::Lua::ILuaBase* L)
	{
		L->PushNil();
		L->SetField(GarrysMod::Lua::INDEX_REGISTRY, PVSData::typeName);
	}
}
