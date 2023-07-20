IncludeDir = {}
IncludeDir["Glad"] = "%{wks.location}/ACG_Project/vendor/Glad/include"
IncludeDir["glm"] = "%{wks.location}/ACG_Project/vendor/glm"
IncludeDir["ImGui"]     = "%{wks.location}/ACG_Project/vendor/imgui"
IncludeDir["GLFW"] = "%{wks.location}/ACG_Project/vendor/GLFW/include"
IncludeDir["stb_image"] = "%{wks.location}/ACG_Project/vendor/stb_image"
IncludeDir["objloader"] = "%{wks.location}/ACG_Project/vendor/tinyobjloader"

--VULKAN_SDK = os.getenv("VULKAN_SDK")
--[[LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"--]]