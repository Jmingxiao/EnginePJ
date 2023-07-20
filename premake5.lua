include "Dependency.lua"
workspace "ACG_Project"
	architecture "x64"
	startproject "ACG_Project"
    
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
    flags{ "MultiProcessorCompile"}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
    include "ACG_Project/vendor/Glad"
    include "ACG_Project/vendor/GLFW"
    include "ACG_Project/vendor/imgui"
group ""


project "ACG_Project"
    location "ACG_Project"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"


    targetdir("%{wks.location}/bin/".. outputdir .."/%{prj.name}")
    objdir("%{wks.location}/bin-int/".. outputdir .."/%{prj.name}")

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cc",
        "%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/tinyobjloader/**.h",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
    }
    
    
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.glm}", 
        "%{IncludeDir.ImGui}",
		"%{IncludeDir.stb_image}",
        "%{IncludeDir.objloader}",
    }

    defines
    {
        --"DISABLE_VCPKG"
    } 

    links
    {   
        "Glad",
        "GLFW",
        "ImGui",
        "opengl32.lib",
    }


  

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "VL_DEBUG"
        runtime "Debug"
        symbols "on"
        

    filter "configurations:Release"
        defines "VL_RELEASE"
        runtime "Release"
        optimize "on"
    
    filter "configurations:Dist"
        defines "VL_DIST"
        runtime "Release"
        optimize "on"

