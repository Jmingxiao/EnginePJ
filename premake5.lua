workspace "Mint"
    architecture "x64"
    startproject "Mint-Editor"
    configurations{ "Debug","Release","Dist"}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


-- Include directories relative to root folder (solution directory)
IncludeDir = {}
--IncludeDir["GLFW"] = "Mint/vendor/GLFW/include"
--IncludeDir["Glad"] = "Mint/vendor/Glad/include"
--IncludeDir["glm"] = "Mint/vendor/glm"
IncludeDir["ImGui"] = "Mint/vendor/imgui"
IncludeDir["stb_image"] = "Mint/vendor/stb_image"
IncludeDir["objloader"] = "Mint/vendor/tinyobjloader"


group "Dependencies"
    --include "Mint/vendor/GLFW"
    --include "Mint/vendor/Glad"
    include "Mint/vendor/imgui"
group ""

project "Mint"
    location "Mint"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"



    targetdir("bin/".. outputdir .."/%{prj.name}")
    objdir("bin-int/".. outputdir .."/%{prj.name}")

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        --"%{prj.name}/vendor/glm/glm/**.hpp",
        --"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/tinyobjloader/**.h",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        --"%{IncludeDir.GLFW}",
        --"%{IncludeDir.Glad}",
        --"%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
		"%{IncludeDir.stb_image}",
        "%{IncludeDir.objloader}",
    }

    links
    {
        --"GLFW",
        --"Glad",
        "ImGui",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"


        defines
        {
            "MT_PLATFORM_WINDOWS", 
            "MT_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        } 

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


    project "Sandbox"
        location "Sandbox"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        staticruntime "on"
    
        targetdir("bin/".. outputdir .."/%{prj.name}")
        objdir("bin-int/".. outputdir .."/%{prj.name}")
        
        files
        {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp"
        }
        
        includedirs
        {
            "Mint/vendor/spdlog/include",
            "Mint/src",
            "Mint/vendor",
            --"%{IncludeDir.glm}"
        }
        
        links{ "Mint" }
        linkoptions { "-IGNORE:4217" }
        ignoredefaultlibraries { "msvcrtd" }
        
        filter "system:windows"
            systemversion "latest"
    
            defines
            {
                "MT_PLATFORM_WINDOWS" 
            } 
    
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
    
    


    project "Mint-Editor"
        location "Mint-Editor"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        staticruntime "on"
            
        targetdir("bin/".. outputdir .."/%{prj.name}")
        objdir("bin-int/".. outputdir .."/%{prj.name}")
            
        files
        {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp"
        }
    
        includedirs
        {
            "Mint/vendor/spdlog/include",
            "Mint/src",
            "Mint/vendor",
            --"%{IncludeDir.glm}"
        }
    
        links{ "Mint" }
        linkoptions { "-IGNORE:4217" }
        ignoredefaultlibraries { "msvcrtd" }
    
        filter "system:windows"
            systemversion "latest"
            defines
            {
                "MT_PLATFORM_WINDOWS" 
            } 
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



