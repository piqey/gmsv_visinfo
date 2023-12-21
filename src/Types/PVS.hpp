#include <stdexcept>

#include "GarrysMod/Lua/Interface.h"
#include "GarrysMod/FactoryLoader.hpp"
#include "eiface.h"
#include "engine/ivdebugoverlay.h"

extern IVEngineServer* engine_server;
extern IVDebugOverlay* debug_overlay;

const QAngle qZero; // I don't know why vec3_angle is unresolved

// extern inline void LuaPrint(GarrysMod::Lua::ILuaBase* L, char* msg);
// extern inline void LuaPrint(GarrysMod::Lua::ILuaBase* L, char* msg, int num);

namespace VisInfo::Types
{
	struct PVSData
	{
		// C++ stuff

		static PVSData FromCluster(int cluster);

		const int size = 0;
		const int total = 0;
		const byte* buffer = nullptr;

		int visible = 0;

		PVSData(int size, byte* buffer);
		PVSData(PVSData&& pvs) noexcept;

		~PVSData();

		bool CheckOrigin(Vector origin) const;
		bool CheckBox(Vector mins, Vector maxs) const;
		bool ContainsCluster(int cluster) const;

		void DebugOverlay(int r, int g, int b, int a, float duration) const;

		// Lua UserType stuff

		static const char* typeName;
		static int meta;

		static void Initialize(GarrysMod::Lua::ILuaBase* L);
		static void Deinitialize(GarrysMod::Lua::ILuaBase* L);
	};
}
