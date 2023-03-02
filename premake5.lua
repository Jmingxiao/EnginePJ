include "Dependencies.lua"

workspace "Mint"
    architecture "x64"
    startproject "Mint-Editor"
    configurations{ "Debug","Release","Dist"}
    flags{ "MultiProcessorCompile"}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    --include "Mint/vendor/GLFW"
    --include "Mint/vendor/Glad"
    include "Mint/vendor/imgui"
    include "Mint/vendor/react3d"
	include "Mint/vendor/yaml-cpp"
group ""


include "Mint"

include "Mint-Editor"


include "Sandbox"