project "Mint-Editor"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        staticruntime "on"
            
        targetdir("%{wks.location}/bin/".. outputdir .."/%{prj.name}")
        objdir("%{wks.location}/bin-int/".. outputdir .."/%{prj.name}")
            
        files
        {
            "src/**.h",
            "src/**.cpp"
        }
        
        defines
        {
            "MT_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS"
        } 

        includedirs
        {
            "%{wks.location}/Mint/vendor/spdlog/include",
            "%{wks.location}/Mint/src",
            "%{wks.location}/Mint/vendor",
            "%{IncludeDir.entt}",
            "%{IncludeDir.react3d}",
            "%{IncludeDir.ImGuizmo}",
            "%{IncludeDir.glm}"
        }
    
        links{ "Mint" }
        linkoptions { "-IGNORE:4217" }
        ignoredefaultlibraries { "msvcrtd" }
    
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
