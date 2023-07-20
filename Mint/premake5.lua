project "Mint"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"


    targetdir("%{wks.location}/bin/".. outputdir .."/%{prj.name}")
    objdir("%{wks.location}/bin-int/".. outputdir .."/%{prj.name}")

    pchheader "pch.h"
    pchsource "src/pch.cpp"


    files
    {
        "src/**.h",
        "src/**.cpp",
        "vendor/entt/**.hpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
        "vendor/tinyobjloader/**.h",
        "vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
    }
    
    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "MT_PLATFORM_WINDOWS", 
        "MT_BUILD_DLL",
        "GLFW_INCLUDE_NONE"
    } 

    includedirs
    {
        "src",
        "vendor/spdlog/include",
        "%{IncludeDir.ImGui}",
		"%{IncludeDir.stb_image}",
        "%{IncludeDir.objloader}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.react3d}",
        "%{IncludeDir.mono}",
        "%{IncludeDir.filewatch}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.glm}",
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "react3d",
        "yaml-cpp",
        "opengl32.lib",
        "%{Library.mono}",
    }

    filter "files:vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "MT_DEBUG"
        runtime "Debug"
        symbols "on"
        

    filter "configurations:Release"
        defines "MT_RELEASE"
        runtime "Release"
        optimize "on"
    
    filter "configurations:Dist"
        defines "MT_DIST"
        runtime "Release"
        optimize "on"
