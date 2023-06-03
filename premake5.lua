-- premake5.lua
workspace "UltimatumGame"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "UltimatumGame"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "UltimatumGame"