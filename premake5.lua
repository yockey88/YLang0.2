workspace "YLang0.2"
    startproject "YLang0.2"
    architecture "x64"

    configurations {
        "Debug" ,
        "Release"
    }

    tdir = "bin/%{cfg.buildcfg}/%{prj.name}"
    odir = "bin_obj/%{cfg.buildcfg}/%{prj.name}"

    -- External Dependencies
    externals = {}
    externals["spdlog"] = "external/spdlog-1.11.0"
    
    include "external/spdlog-1.11.0"

    ------------------------
    -- Parser Development --
    ------------------------
    project "YLang0.2"
        location "YLang0.2"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        staticruntime "on"

        targetdir(tdir)
        objdir(odir)

        files {
            "%{prj.name}/**.cpp",
            "%{prj.name}/**.hpp" ,
            "%{prj.name}/include/**.hpp"
        }

        externalincludedirs {
            "%{prj.name}/include" ,

        }

        filter { "system:windows" , "configurations:*" }
            systemversion "latest"
            defines {
                "YLC_PLATFORM_WINDOWS"
            }
        
        filter { "system:macosx" , "configurations:*" }
            systemversion "latest"
            xcodebuildsettings {
                ["MACOSX_DEPLOYMENT_TARGET"] = "10.15",
                ["UseModernBuildSystem"] = "NO"
            }
            defines {
                "YLC_PLATFORM_MACOS"
            }

        filter { "system:linux" , "configurations:*" }
            defines {
                "YLC_PLATFORM_LINUX"
            }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"
            defines { 
                "YLC_DEBUG"
            }

        filter "configurations:Release"
            runtime "Release"
            symbols "off"
            optimize "on"
            defines { 
                "YLC_RELEASE"
            }

    project "ByteCode"
        location "ByteCode"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        staticruntime "on"

        targetdir(tdir)
        objdir(odir)

        files {
            "%{prj.name}/**.cpp",
            "%{prj.name}/**.hpp" ,
            "%{prj.name}/include/**.hpp"
        }

        includedirs {
            "YLang0.2/include" ,
            "ByteCode/include" ,
            "%{externals.spdlog}/include"
        }

        links {
            "spdlog"
        }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            runtime "Release"
            symbols "off"
            optimize "on"