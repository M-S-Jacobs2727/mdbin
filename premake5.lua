-- mdbin

workspace "MDBIN"
    configurations { "debug-static", "release-static", "debug-shared", "release-shared" }

project "MDBIN"
    language "C++"
    location "build"

    files { "src/*.h", "src/*.cpp" }

    filter { "configurations:debug-static" }
        kind "StaticLib"
        defines {"DEBUG"}
        symbols "On"
    
    filter { "configurations:release-static" }
        kind "StaticLib"
        defines {"NDEBUG"}
        optimize "Speed"
        
    filter { "configurations:debug-shared" }
        kind "SharedLib"
        defines {"DEBUG"}
        symbols "On"
    
    filter { "configurations:release-shared" }
        kind "SharedLib"
        defines {"NDEBUG"}
        optimize "Speed"

    filter "action:gmake2"
        buildoptions {"-std=c++17"}
