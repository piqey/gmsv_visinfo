#include <stdexcept>

#include "GarrysMod/Lua/Interface.h"
#include "GarrysMod/FactoryLoader.hpp"
#include "eiface.h"

extern const SourceSDK::FactoryLoader engine_loader;
extern IVEngineServer* engine_server;

// extern inline void LuaPrint(GarrysMod::Lua::ILuaBase* L, char* msg);
// extern inline void LuaPrint(GarrysMod::Lua::ILuaBase* L, char* msg, int num);

namespace VisInfo::Types
{
	const struct PVSData
	{
		// C++ stuff

		const int size = 0;
		const int total = 0;
		const byte* buffer = nullptr;

		int visible = 0;

		PVSData(int size, byte* buffer);
		~PVSData();

		bool CheckOrigin(Vector origin) const;
		bool CheckBox(Vector mins, Vector maxs) const;
		bool ContainsCluster(int cluster) const;

		// Lua UserType stuff

		static const char* typeName;
		static int meta;

		static void Initialize(GarrysMod::Lua::ILuaBase* L);
		static void Deinitialize(GarrysMod::Lua::ILuaBase* L);
	};
}
