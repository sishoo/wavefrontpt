@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

clang-format main.c -i

set VULKAN_SDK=C:\VulkanSDK\1.3.296.0
set includes=/I%VULKAN_SDK%\Include
set links=/link /LIBPATH:%VULKAN_SDK%\Lib vulkan-1.lib SDL2main.lib SDL2.lib


clang main.c -o main.exe -I%VULKAN_SDK%\Include -L%VULKAN_SDK%\Lib -lvulkan-1 -lsdl2main -lsdl2 -ggdb -O0 -Wall -ferror-limit=0



