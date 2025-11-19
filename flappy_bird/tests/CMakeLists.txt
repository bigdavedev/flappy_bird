# Copyright 2025 - David Brown <d.brown@bigdavedev.com>
# SPDX-License-Identifier: MIT

add_executable (flappy_bird_tests)

target_sources (
	flappy_bird_tests

	PRIVATE
	tests.cpp
)

target_link_libraries (
	flappy_bird_tests

	PRIVATE
	gtest
	gtest_main
)

add_test (
	NAME FlappyBirdTests
	COMMAND flappy_bird_tests
)
