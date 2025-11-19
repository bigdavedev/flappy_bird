# Copyright 2025 - David Brown <d.brown@bigdavedev.com>
# SPDX-License-Identifier: MIT
include (FetchContent)

function (install_ninja)
	set (NINJA_REPOSITORY https://github.com/ninja-build/ninja/releases/download)
	set (NINJA_VERSION "v1.12.1")
	string (TOLOWER "${CMAKE_HOST_SYSTEM_NAME}" host)
	FetchContent_Declare (
		ninja.Windows
		URL ${NINJA_REPOSITORY}/${NINJA_VERSION}/ninja-win.zip
		DOWNLOAD_NO_PROGRESS TRUE
	)
	FetchContent_Declare (
		ninja.Darwin
		URL ${NINJA_REPOSITORY}/${NINJA_VERSION}/ninja-mac.zip
		DOWNLOAD_NO_PROGRESS TRUE
	)
	FetchContent_Declare (
		ninja.Linux
		URL ${NINJA_REPOSITORY}/${NINJA_VERSION}/ninja-linux.zip
		DOWNLOAD_NO_PROGRESS TRUE
	)
	FetchContent_MakeAvailable (ninja.${CMAKE_HOST_SYSTEM_NAME})
	find_program (
		CMAKE_MAKE_PROGRAM
		NAMES ninja
		REQUIRED
		PATHS "${ninja.${host}_SOURCE_DIR}"
	)
endfunction (install_ninja)

install_ninja ()
