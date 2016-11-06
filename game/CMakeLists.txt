### === Game DLL === ###
file(GLOB BASE_INCLUDES *.h)
file(GLOB BASE_SOURCES *.cpp)
source_group("" FILES ${BASE_INCLUDES})
source_group("" FILES ${BASE_SOURCES})

file(GLOB BASE_AI_INCLUDES ai/*.h)
file(GLOB BASE_AI_SOURCES ai/*.cpp)
source_group("ai" FILES ${BASE_AI_INCLUDES})
source_group("ai" FILES ${BASE_AI_SOURCES})

file(GLOB BASE_ANIM_INCLUDES anim/*.h)
file(GLOB BASE_ANIM_SOURCES anim/*.cpp)
source_group("anim" FILES ${BASE_ANIM_INCLUDES})
source_group("anim" FILES ${BASE_ANIM_SOURCES})

file(GLOB BASE_GSYS_INCLUDES gamesys/*.h)
file(GLOB BASE_GSYS_SOURCES gamesys/*.cpp)
source_group("gamesys" FILES ${BASE_GSYS_INCLUDES})
source_group("gamesys" FILES ${BASE_GSYS_SOURCES})

file(GLOB BASE_PHYS_INCLUDES physics/*.h)
file(GLOB BASE_PHYS_SOURCES physics/*.cpp)
source_group("physics" FILES ${BASE_PHYS_INCLUDES})
source_group("physics" FILES ${BASE_PHYS_SOURCES})

file(GLOB BASE_SCRI_INCLUDES script/*.h)
file(GLOB BASE_SCRI_SOURCES script/*.cpp)
source_group("script" FILES ${BASE_SCRI_INCLUDES})
source_group("script" FILES ${BASE_SCRI_SOURCES})

set(src_game
	${BASE_SOURCES}
	${BASE_AI_SOURCES}
	${BASE_ANIM_SOURCES}
	${BASE_GSYS_SOURCES}
	${BASE_PHYS_SOURCES}
	${BASE_SCRI_SOURCES}
)

set (inc_game
	${BASE_INCLUDES}
	${BASE_AI_INCLUDES}
	${BASE_ANIM_INCLUDES}
	${BASE_GSYS_INCLUDES}
	${BASE_PHYS_INCLUDES}
	${BASE_SCRI_INCLUDES}
)

set(src_game_list
	AF.cpp
	AFEntity.cpp
	Actor.cpp
	Camera.cpp
	Entity.cpp
	BrittleFracture.cpp
	Fx.cpp
	GameEdit.cpp
	Game_local.cpp
	Game_network.cpp
	Item.cpp
	IK.cpp
	Light.cpp
	Misc.cpp
	Mover.cpp
	Moveable.cpp
	MultiplayerGame.cpp
	Player.cpp
	PlayerIcon.cpp
	PlayerView.cpp
	Projectile.cpp
	Pvs.cpp
	SecurityCamera.cpp
	SmokeParticles.cpp
	Sound.cpp
	Target.cpp
	Trigger.cpp
	Weapon.cpp
	WorldSpawn.cpp
	ai/AAS.cpp
	ai/AAS_debug.cpp
	ai/AAS_pathing.cpp
	ai/AAS_routing.cpp
	ai/AI.cpp
	ai/AI_events.cpp
	ai/AI_pathing.cpp
	ai/AI_Vagary.cpp
	gamesys/DebugGraph.cpp
	gamesys/Class.cpp
	gamesys/Event.cpp
	gamesys/SaveGame.cpp
	gamesys/SysCmds.cpp
	gamesys/SysCvar.cpp
	gamesys/TypeInfo.cpp
	anim/Anim.cpp
	anim/Anim_Blend.cpp
	anim/Anim_Import.cpp
	anim/Anim_Testmodel.cpp
	script/Script_Compiler.cpp
	script/Script_Interpreter.cpp
	script/Script_Program.cpp
	script/Script_Thread.cpp
	physics/Clip.cpp
	physics/Force.cpp
	physics/Force_Constant.cpp
	physics/Force_Drag.cpp
	physics/Force_Field.cpp
	physics/Force_Spring.cpp
	physics/Physics.cpp
	physics/Physics_AF.cpp
	physics/Physics_Actor.cpp
	physics/Physics_Base.cpp
	physics/Physics_Monster.cpp
	physics/Physics_Parametric.cpp
	physics/Physics_Player.cpp
	physics/Physics_RigidBody.cpp
	physics/Physics_Static.cpp
	physics/Physics_StaticMulti.cpp
	physics/Push.cpp
)

include_directories(${CMAKE_SOURCE_DIR})
add_library(base SHARED ${src_game} ${inc_game})
	set_target_properties(base PROPERTIES PREFIX "")
	set_target_properties(base PROPERTIES COMPILE_DEFINITIONS "GAME_DLL")
	set_target_properties(base PROPERTIES COMPILE_FLAGS "-I${CMAKE_SOURCE_DIR}/game")
	set_target_properties(base PROPERTIES LINK_FLAGS "${ldflags}")
	set_target_properties(base PROPERTIES INSTALL_NAME_DIR "@executable_path")
	target_link_libraries(base idlib)

	if(NOT APPLE AND NOT WIN32)
		install(TARGETS base
				RUNTIME DESTINATION "${bindir}"
				LIBRARY DESTINATION "${libdir}"
				ARCHIVE DESTINATION "${libdir}"
		)
	endif()