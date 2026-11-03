cmake_minimum_required(VERSION 3.22)
project(ffmenu CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(dobby
  GIT_REPOSITORY https://github.com/jmpews/Dobby.git
  GIT_TAG        master)
FetchContent_MakeAvailable(dobby)

add_library(kittymemory STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/external/KittyMemory/MemoryPatch.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/external/KittyMemory/MemoryBackup.cpp)

add_library(modmenu SHARED
  Main.cpp FeatureModule.cpp Canvas.cpp)

target_include_directories(modmenu PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/external/KittyMemory)

target_link_libraries(modmenu dobby kittymemory android log)
target_compile_options(modmenu PRIVATE -fvisibility=hidden -O2)
set_target_properties(modmenu PROPERTIES OUTPUT_NAME "ModMenu")
