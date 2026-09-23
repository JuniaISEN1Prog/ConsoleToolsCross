cmake_minimum_required(VERSION 3.10)

# Intégration vcpkg pour la compilation Windows/MSYS64.
# CMAKE_TOOLCHAIN_FILE doit être défini avant le premier appel à project().
if(WIN32)
  set(CMAKE_TOOLCHAIN_FILE "C:/msys64/mingw64/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "Chemin vers le toolchain vcpkg")
  set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "Triplet vcpkg")
endif()


project(ConsoleTools C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Définir les sources selon la plateforme
if(WIN32)
    set(CONSOLE_SRC ConsoleTools.c)
    add_definitions(-DPLATFORM_WINDOWS)
else()
    set(CONSOLE_SRC ConsoleToolsLinux.c)
    add_definitions(-DPLATFORM_LINUX)
endif()



# Sources du projet
set(SOURCES
    demo.c
    ${CONSOLE_SRC}
)


# Créer l'exécutable
add_executable(demo ${SOURCES})

# Options de compilation
target_compile_options(demo PRIVATE
    -Wall
    -Wextra
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3>
)

# Installation
install(TARGETS demo DESTINATION bin)



# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()