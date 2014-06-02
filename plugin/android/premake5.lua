PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	removeplatforms { "ios", "osx64", "linux32", "linux64", "html5" }

	kind "StaticLib"
	language "C++"

	files {
		"src/**.hpp",
		"src/**.h",
		"src/**.cpp",
		"src/**.c"
	}

	includedirs {
		"include",
		minko.plugin.path("sdl") .. "/lib/sdl/include" -- hard coded but minko-plugin-sdl is always enabled here.
	}
