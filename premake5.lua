workspace "CPUPipeline"
	startproject "CPUPipeline"

	configurations { "Debug", "Release" }
	platforms { "x64", "Win32"}

	filter { "platforms:Win32" }
		system "Windows"
		architecture "x32"

	filter { "platforms:x64" }
		system "Windows"
		architecture "x64"

	flags
	{
		"MultiProcessorCompile"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["GLFW"] = "vendor/glfw/include"
IncludeDir["glm"] = "vendor/glm"

group "Dependencies"
	include "vendor/Glad"
	include "vendor/glfw"

group ""

project "CPUPipeline"
	location "CPUPipeline"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")
	
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}
	
	includedirs
	{
		"%{prj.name}/src",

		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}"
	}
	
	links 
	{ 
		"GLFW",
		"Glad",
		"opengl32.lib"
	}
	
	filter "architecture:x86"
		libdirs {
			"./vendor/glew/lib/Release/win32"
		}
	
	filter "architecture:x64"
		libdirs {
			"./vendor/glew/lib/Release/x64"
		}
	
	filter "system:windows"

		defines
		{
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		-- defines ""
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		-- defines ""
		runtime "Release"
		optimize "on"