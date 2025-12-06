# Copyright 2025 - David Brown <d.brown@bigdavedev.com>
# SPDX-License-Identifier: MIT

add_executable (flappy_bird)

target_sources (
	flappy_bird
	
	PRIVATE
	main.cpp
)

set_target_properties (
	flappy_bird

	PROPERTIES
	CXX_STANDARD     23
	WIN32_EXECUTABLE $<PLATFORM_ID:Windows>
	VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_link_libraries (
	flappy_bird

	PRIVATE
	FlappyBird::CompilerFlags
	SDL3::SDL3
	SDL3_image::SDL3_image
	SDL3_ttf::SDL3_ttf
	SDL3_mixer::SDL3_mixer
	glm::glm-header-only
	EnTT::EnTT
)

if (FLAPPY_BIRD_ENABLE_TESTING)
	add_subdirectory (tests)
endif ()
