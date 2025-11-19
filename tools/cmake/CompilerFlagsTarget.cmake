# Copyright 2025 - David Brown <d.brown@bigdavedev.com>
# SPDX-License-Identifier: MIT
macro (flappy_bird_create_compiler_flags_target)
	add_library (flappy_bird_compiler_flags INTERFACE)
	add_library (FlappyBird::CompilerFlags ALIAS flappy_bird_compiler_flags)

	foreach (flag IN LISTS FLAPPY_BIRD_COMPILER_FLAGS_LIST)
		target_compile_options (flappy_bird_compiler_flags INTERFACE ${flag})
	endforeach ()

	foreach (flag IN LISTS FLAPPY_BIRD_LINKER_FLAGS_INIT)
		target_link_options (flappy_bird_compiler_flags INTERFACE ${flag})
	endforeach ()


	target_compile_features (flappy_bird_compiler_flags INTERFACE cxx_std_23)
endmacro ()

macro (flappy_bird_try_enable_ipo)
	include (CheckIPOSupported)

	check_ipo_supported(RESULT result OUTPUT output)
	if (result)
		set (CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
		set (CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
		set (CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE)
	else ()
		message (WARNING "IPO is not supported: ${output}")
	endif ()
endmacro ()

flappy_bird_create_compiler_flags_target ()
