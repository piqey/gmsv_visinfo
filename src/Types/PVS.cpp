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

	void PVSData::DebugOverlay(int r, int g, int b, int a, float duration) const
	{
		bbox_t* bboxes = new bbox_t[total];

		engine_server->GetAllClusterBounds(bboxes, total);

		for (int i = 0; i < total; i++)
			if (buffer[i / 8] & (1 << (i % 8)))
			{
				#pragma warning(suppress: 6385)
				bbox_t& bbox = bboxes[i];
				Vector center = 0.5f * (bbox.mins + bbox.maxs);

				debug_overlay->AddBoxOverlay(center, bbox.mins - center, bbox.maxs - center, qZero, r, g, b, a, duration);
			}
		
		delete[] bboxes;
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
		LUA->PushFormattedString("[%s] %f/%f clusters visible", PVSData::typeName, (double)pvs->visible, (double)pvs->total);

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
		int cluster = (int)LUA->CheckNumber(2);

		PVSData* pvs = LUA->GetUserType<PVSData>(1, PVSData::meta);

		try { LUA->PushBool(pvs->ContainsCluster(cluster)); }
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

	LUA_FUNCTION_STATIC(DebugOverlay)
	{
		LUA->CheckType(1, PVSData::meta);
		LUA->CheckType(2, GarrysMod::Lua::Type::Table);
		float duration = (float)LUA->CheckNumber(3);

		LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); // Push _G onto the stack
		LUA->GetField(-1, "IsColor"); // Push the IsColor function from _G onto the stack

		LUA->Remove(-2); // Remove _G from the stack, keeping IsColor

		LUA->Push(2); // Push the color table onto the stack

		LUA->Call(1, 1); // Call IsColor with 1 argument, 1 result

		if (!LUA->GetBool(-1))
			LUA->ThrowError("Second argument must be a valid Color object!");

		LUA->Pop(1); // Pop the IsColor bool result off the stack

		LUA->Push(2); // Push the color table onto the stack

		LUA->GetField(-1, "r");
		int r = (int)LUA->GetNumber(-1);
		LUA->Pop(1);

		LUA->GetField(-1, "g");
		int g = (int)LUA->GetNumber(-1);
		LUA->Pop(1);

		LUA->GetField(-1, "b");
		int b = (int)LUA->GetNumber(-1);
		LUA->Pop(1);

		LUA->GetField(-1, "a");
		int a = (int)LUA->GetNumber(-1);
		LUA->Pop(2); // Also pop the color table off the stack

		LUA->GetUserType<PVSData>(1, PVSData::meta)->DebugOverlay(r, g, b, a, duration);

		return 0;
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

		L->PushCFunction(VisInfo::Types::DebugOverlay);
		L->SetField(-2, "DebugOverlay");

		L->Pop();
	}

	void PVSData::Deinitialize(GarrysMod::Lua::ILuaBase* L)
	{
		L->PushNil();
		L->SetField(GarrysMod::Lua::INDEX_REGISTRY, PVSData::typeName);
	}
}
