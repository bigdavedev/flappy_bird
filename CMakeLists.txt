# Copyright 2025 - David Brown <d.brown@bigdavedev.com>
# SPDX-License-Identifier: MIT
cmake_minimum_required (VERSION 3.31)

list (PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake)

include (InstallBuildTools)

project (
	FlappyBird
	VERSION 0.1.0
	LANGUAGES C CXX
	DESCRIPTION "Flappy Bird"
	HOMEPAGE_URL "https://github.com/bigdavedev/flappy_bird.git"
)

set_directory_properties (PROPERTY VS_STARTUP_PROJECT flappy_bird)

option (FLAPPY_BIRD_BUILD_DOCS "Generate documentation" OFF)
option (FLAPPY_BIRD_ENABLE_TESTING "Enable running tests" ON)
option (FLAPPY_BIRD_ENABLE_PACKAGING "Enable packaging" ON)

include (StageConfig)
include (CompilerFlagsTarget)

if (FLAPPY_BIRD_ENABLE_TESTING)
	enable_testing ()
endif ()

if (FLAPPY_BIRD_BUILD_DOCS)
	add_subdirectory (docs)
endif ()

add_subdirectory (external)

if (FLAPPY_BIRD_ENABLE_CLANG_TIDY)
	set (CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif ()

add_subdirectory (flappy_bird)

if (FLAPPY_BIRD_ENABLE_PACKAGING)
	set (CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
	include (InstallRequiredSystemLibraries)

	install (TARGETS flappy_bird DESTINATION .)
	install (FILES $<TARGET_RUNTIME_DLLS:flappy_bird> DESTINATION .)

	set (CPACK_COMPONENTS_ALL Unspecified)

	install (DIRECTORY data DESTINATION .)

	set (CPACK_PACKAGE_CONTACT "David Brown <d.brown@bigdavedev.com>")
	set (CPACK_PACKAGE_EXECUTABLES "flappy_bird" "FlappyBird")

	if (LINUX)
		include (package/Linux)
	elseif (WIN32)
		include (package/Windows)
	endif ()

	include (CPack)
endif ()
