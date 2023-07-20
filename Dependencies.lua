-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Mint/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Mint/vendor/Glad/include"
IncludeDir["glm"] =  "%{wks.location}/Mint/vendor/glm"
IncludeDir["ImGui"]     = "%{wks.location}/Mint/vendor/imgui"
IncludeDir["stb_image"] = "%{wks.location}/Mint/vendor/stb_image"
IncludeDir["objloader"] = "%{wks.location}/Mint/vendor/tinyobjloader"
IncludeDir["react3d"]   = "%{wks.location}/Mint/vendor/react3d/include"
IncludeDir["entt"]      = "%{wks.location}/Mint/vendor/entt"
IncludeDir["mono"]      = "%{wks.location}/Mint/vendor/mono/include"
IncludeDir["filewatch"] = "%{wks.location}/Mint/vendor/filewatch"
IncludeDir["ImGuizmo"] = "%{wks.location}/Mint/vendor/ImGuizmo"
IncludeDir["yaml_cpp"] = "%{wks.location}/Mint/vendor/yaml-cpp/include"



LibraryDir = {}
LibraryDir["mono"] = "%{wks.location}/Mint/vendor/mono/lib/%{cfg.buildcfg}"

Library = {}
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"