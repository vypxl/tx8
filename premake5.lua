workspace "tx8"
    toolset "clang"
    configurations { "debug", "release" }
    includedirs "include"

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "on"
    
    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "on"

project "tx8-core"
    kind "SharedLib"
    language "C"
    targetdir "out/%{cfg.buildcfg}"

    buildoptions { "-std=c11", "-Wall", "-Wextra", "-Werror" }
    
    links { "m" }

    files { "src/core/**.h", "src/core/**.c" }

project "tx8-core-test"
    kind "ConsoleApp"
    language "C"
    targetdir "out/test"

    buildoptions { "-std=c11", "-Wall", "-Wextra", "-Werror" }
    links { "tx8-core", "m" }

    files { "test/main.c" }

project "tx8-asm"
    kind "ConsoleApp"
    language "C"
    targetdir "out/%{cfg.buildcfg}"

    buildoptions { "-std=c11", "-Wall", "-Wextra", "-Wno-implicit-function-declaration", "-Wno-unused-function", "-Wno-unneeded-internal-declaration" }

    files { "src/asm/**.h", "src/asm/**.c" }
