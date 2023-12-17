---@diagnostic disable

PROJECT_GENERATOR_VERSION = 2

newoption({
	trigger = "gmcommon",
	description = "Sets the path to the garrysmod_common (https://github.com/danielga/garrysmod_common) directory",
    value = "../garrysmod_common"
})

local gmcommon = _OPTIONS.gmcommon or os.getenv("GARRYSMOD_COMMON") or "./garrysmod_common"
include(gmcommon)

CreateWorkspace({name = "visinfo", abi_compatible = false, path = "projects/" .. os.target() .. "/" .. _ACTION})
	configurations({"Debug", "Release"})
	platforms({"Static32", "Shared32", "Static64", "Shared64"})

	filter("platforms:Static32")
		kind("StaticLib")
		architecture("x32")

	filter("platforms:Static64")
		kind("StaticLib")
		architecture("x64")

	filter("platforms:Shared32")
		kind("SharedLib")
		architecture("x32")

	filter("platforms:Shared64")
		kind("SharedLib")
		architecture("x64")

	CreateProject({serverside = true, source_path = "src", manual_files = false})
		IncludeLuaShared()
		IncludeScanning()
		IncludeDetouring()
		IncludeSDKCommon()
		IncludeSDKTier0()
		IncludeSDKTier1()

		files({"src/Types/*.c", "src/Types/*.cpp"})
		files({"src/Types/*.h", "src/Types/*.hpp"})

	--[[
	CreateProject({serverside = false, source_path = "src", manual_files = true})
		IncludeLuaShared()
		IncludeScanning()
		IncludeDetouring()
		IncludeSDKCommon()
		IncludeSDKTier0()
		IncludeSDKTier1()

		filter("system:windows")
			files({"src/win32/*.cpp", "src/win32/*.hpp"})

		filter("system:linux or macosx")
			files({"src/posix/*.cpp", "src/posix/*.hpp"})
	--]]
