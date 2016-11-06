
### === Game DLL D3XP === ###
file(GLOB D3XP_INCLUDES d3xp/*.h)
file(GLOB D3XP_SOURCES d3xp/*.cpp)
source_group("" FILES ${D3XP_INCLUDES})
source_group("" FILES ${D3XP_SOURCES})

file(GLOB D3XP_AI_INCLUDES d3xp/ai/*.h)
file(GLOB D3XP_AI_SOURCES d3xp/ai/*.cpp)
source_group("ai" FILES ${D3XP_AI_INCLUDES})
source_group("ai" FILES ${D3XP_AI_SOURCES})

file(GLOB D3XP_ANIM_INCLUDES d3xp/anim/*.h)
file(GLOB D3XP_ANIM_SOURCES d3xp/anim/*.cpp)
source_group("anim" FILES ${D3XP_ANIM_INCLUDES})
source_group("anim" FILES ${D3XP_ANIM_SOURCES})

file(GLOB D3XP_GSYS_INCLUDES d3xp/gamesys/*.h)
file(GLOB D3XP_GSYS_SOURCES d3xp/gamesys/*.cpp)
source_group("gamesys" FILES ${D3XP_GSYS_INCLUDES})
source_group("gamesys" FILES ${D3XP_GSYS_SOURCES})

file(GLOB D3XP_PHYS_INCLUDES d3xp/physics/*.h)
file(GLOB D3XP_PHYS_SOURCES d3xp/physics/*.cpp)
source_group("physics" FILES ${D3XP_PHYS_INCLUDES})
source_group("physics" FILES ${D3XP_PHYS_SOURCES})

file(GLOB D3XP_SCRI_INCLUDES d3xp/script/*.h)
file(GLOB D3XP_SCRI_SOURCES d3xp/script/*.cpp)
source_group("script" FILES ${D3XP_SCRI_INCLUDES})
source_group("script" FILES ${D3XP_SCRI_SOURCES})

set(src_d3xp
	${D3XP_SOURCES}
	${D3XP_AI_SOURCES}
	${D3XP_ANIM_SOURCES}
	${D3XP_GSYS_SOURCES}
	${D3XP_PHYS_SOURCES}
	${D3XP_SCRI_SOURCES}
)

set (inc_d3xp
	${D3XP_INCLUDES}
	${D3XP_AI_INCLUDES}
	${D3XP_ANIM_INCLUDES}
	${D3XP_GSYS_INCLUDES}
	${D3XP_PHYS_INCLUDES}
	${D3XP_SCRI_INCLUDES}
)

add_library(d3xp SHARED ${src_d3xp} ${inc_d3xp})
	set_target_properties(d3xp PROPERTIES PREFIX "")
	set_target_properties(d3xp PROPERTIES COMPILE_DEFINITIONS "GAME_DLL;_D3XP;CTF")
	set_target_properties(d3xp PROPERTIES COMPILE_FLAGS "-I${CMAKE_SOURCE_DIR}/d3xp")
	set_target_properties(d3xp PROPERTIES LINK_FLAGS "${ldflags}")
	set_target_properties(d3xp PROPERTIES INSTALL_NAME_DIR "@executable_path")
	target_link_libraries(d3xp idlib)

	if(NOT APPLE AND NOT WIN32)
		install(TARGETS d3xp
				RUNTIME DESTINATION "${bindir}"
				LIBRARY DESTINATION "${libdir}"
				ARCHIVE DESTINATION "${libdir}"
		)
	endif()