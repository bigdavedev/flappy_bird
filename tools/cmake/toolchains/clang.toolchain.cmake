# Copyright 2025 - David Brown <d.brown@bigdavedev.com>
# SPDX-License-Identifier: MIT
set (CMAKE_C_COMPILER clang)
set (CMAKE_CXX_COMPILER clang++)

set (
	FLAPPY_BIRD_COMPILER_FLAGS_LIST
	-Weverything
	-Wno-c++98-compat-pedantic
	-Wno-c++20-compat
	-Wno-padded
	-Wno-exit-time-destructors
	-Wno-missing-prototypes
	-Wno-weak-vtables
	-Wno-global-constructors
	-Wno-switch-default
	-Wno-ctad-maybe-unsupported
)
