project "gtest"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
    targetdir(tdir)
    objdir(odir)

    files {
        "googletest/include/**.h" ,
        "googletest/src/**.h" ,
        "googletest/src/**.cc" ,
        "googlemock/include/**.h" ,
        "googlemock/src/**.cc"
    }

    includedirs {
        "googletest/include" ,
        "googletest" ,
        "googlemock/include" ,
        "googlemock"
    }

    filter { "system:windows" }
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        symbols "off"
        optimize "on"