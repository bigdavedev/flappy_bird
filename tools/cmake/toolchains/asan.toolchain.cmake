# Copyright 2025 - David Brown <d.brown@bigdavedev.com>
# SPDX-License-Identifier: MIT
set (CMAKE_C_COMPILER clang)
set (CMAKE_CXX_COMPILER clang++)

set (
	FLAPPY_BIRD_COMPILER_FLAGS_LIST
	-fsanitize=address
	-fno-omit-frame-pointer
	-O1
	-g
	-fno-sanitize-recover
)

set (
	FLAPPY_BIRD_LINKER_FLAGS_INIT
	-fsanitize=address
)
