# CMakeList.txt : CMake project for Darklight, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include( PrecompiledHeader.cmake )
project(Darklight)

set(CMAKE_CXX_FLAGS "")

#Check to see if we are wanting a SDK project or a full engine project
Message("${CMAKE_CURRENT_SOURCE_DIR}/full_sdk_license.txt")#

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../)
set(CMAKE_VERBOSE_MAKEFILE ON)

# idLib files
set(src_idlib
	./engine/idlib/precompiled.cpp
	./engine/idlib/precompiled.h
	./engine/idlib/Base64.cpp
	./engine/idlib/BitMsg.h
	./engine/idlib/Base64.h
	./engine/idlib/BitMsg.cpp
	./engine/idlib/bv
	./engine/idlib/CmdArgs.cpp
	./engine/idlib/CmdArgs.h
	./engine/idlib/containers
	./engine/idlib/Dict.cpp
	./engine/idlib/Dict.h
	./engine/idlib/geometry
	./engine/idlib/hashing
	./engine/idlib/Heap.cpp
	./engine/idlib/Heap.h
	./engine/idlib/LangDict.cpp
	./engine/idlib/LangDict.h
	./engine/idlib/Lexer.cpp
	./engine/idlib/Lexer.h
	./engine/idlib/Lib.cpp
	./engine/idlib/Lib.h
	./engine/idlib/MapFile.cpp
	./engine/idlib/MapFile.h
	./engine/idlib/math
	./engine/idlib/Parser.cpp
	./engine/idlib/Parser.h
	./engine/idlib/Str.cpp
	./engine/idlib/Str.h
	./engine/idlib/Swap.h
	./engine/idlib/Timer.cpp
	./engine/idlib/Timer.h
	./engine/idlib/Token.cpp
	./engine/idlib/Token.h
	./engine/idlib/bv/Bounds.cpp
	./engine/idlib/bv/Bounds.h
	./engine/idlib/bv/Box.cpp
	./engine/idlib/bv/Box.h
	./engine/idlib/bv/Frustum.cpp
	./engine/idlib/bv/Frustum.h
	./engine/idlib/bv/Sphere.cpp
	./engine/idlib/bv/Sphere.h
	./engine/idlib/containers/Array.h
	./engine/idlib/containers/BinSearch.h
	./engine/idlib/containers/BTree.h
	./engine/idlib/containers/HashIndex.cpp
	./engine/idlib/containers/HashIndex.h
	./engine/idlib/containers/HashTable.h
	./engine/idlib/containers/Hierarchy.h
	./engine/idlib/containers/LinkList.h
	./engine/idlib/containers/List.h
	./engine/idlib/containers/ListSTL.h
	./engine/idlib/containers/PlaneSet.h
	./engine/idlib/containers/Queue.h
	./engine/idlib/containers/Sort.h
	./engine/idlib/containers/Stack.h
	./engine/idlib/containers/StaticList.h
	./engine/idlib/containers/StrList.h
	./engine/idlib/containers/StrPool.h
	./engine/idlib/containers/VectorSet.h
	./engine/idlib/geometry/DrawVert.cpp
	./engine/idlib/geometry/DrawVert.h
	./engine/idlib/geometry/JointTransform.cpp
	./engine/idlib/geometry/JointTransform.h
	./engine/idlib/geometry/Surface.cpp
	./engine/idlib/geometry/Surface.h
	./engine/idlib/geometry/Surface_Patch.cpp
	./engine/idlib/geometry/Surface_Patch.h
	./engine/idlib/geometry/Surface_Polytope.cpp
	./engine/idlib/geometry/Surface_Polytope.h
	./engine/idlib/geometry/Surface_SweptSpline.cpp
	./engine/idlib/geometry/Surface_SweptSpline.h
	./engine/idlib/geometry/TraceModel.cpp
	./engine/idlib/geometry/TraceModel.h
	./engine/idlib/geometry/Winding.cpp
	./engine/idlib/geometry/Winding.h
	./engine/idlib/geometry/Winding2D.cpp
	./engine/idlib/geometry/Winding2D.h
	./engine/idlib/hashing/CRC32.cpp
	./engine/idlib/hashing/CRC32.h
	./engine/idlib/hashing/MD4.cpp
	./engine/idlib/hashing/MD4.h
	./engine/idlib/hashing/MD5.cpp
	./engine/idlib/hashing/MD5.h
	./engine/idlib/math/Angles.cpp
	./engine/idlib/math/Angles.h
	./engine/idlib/math/Complex.cpp
	./engine/idlib/math/Complex.h
	./engine/idlib/math/Curve.h
	./engine/idlib/math/Extrapolate.h
	./engine/idlib/math/Float16.h
	./engine/idlib/math/Interpolate.h
	./engine/idlib/math/Lcp.cpp
	./engine/idlib/math/Lcp.h
	./engine/idlib/math/Math.cpp
	./engine/idlib/math/Math.h
	./engine/idlib/math/Matrix.cpp
	./engine/idlib/math/Matrix.h
	./engine/idlib/math/Ode.cpp
	./engine/idlib/math/Ode.h
	./engine/idlib/math/Plane.cpp
	./engine/idlib/math/Plane.h
	./engine/idlib/math/Pluecker.cpp
	./engine/idlib/math/Pluecker.h
	./engine/idlib/math/Polynomial.cpp
	./engine/idlib/math/Polynomial.h
	./engine/idlib/math/Quat.cpp
	./engine/idlib/math/Quat.h
	./engine/idlib/math/Random.h
	./engine/idlib/math/Rotation.cpp
	./engine/idlib/math/Rotation.h
	./engine/idlib/math/Simd.cpp
	./engine/idlib/math/Simd.h
	./engine/idlib/math/Simd_Generic.cpp
	./engine/idlib/math/Simd_Generic.h
	./engine/idlib/math/Vector.cpp
	./engine/idlib/math/Vector.h	
)

set(src_cgame 
	./Darklight/cgame/Client_frame.cpp
	./Darklight/cgame/Client_Local.cpp
	./Darklight/cgame/Client_Local.h
	./Darklight/cgame/Client_Packet.cpp
	./Darklight/cgame/Client_PlayerView.h
	./Darklight/cgame/Client_PlayerView.cpp
	./Darklight/cgame/Client_Effects.cpp
	./Darklight/cgame/Client_Effects.h
	./Darklight/cgame/entities/ClientEntity.cpp
	./Darklight/cgame/entities/ClientEntity.h
	./Darklight/cgame/entities/ClientModel.cpp
	./Darklight/cgame/entities/ClientModel.h
	./Darklight/cgame/entities/ClientMoveable.cpp
	./Darklight/cgame/entities/ClientMoveable.h	
)

set(src_game
	./Darklight/game/bot/bot.cpp
	./Darklight/game/bot/bot_input.cpp
	./Darklight/game/bot/bot_char.cpp
	./Darklight/game/bot/bot_char.h
	./Darklight/game/bot/bot_goal.cpp
	./Darklight/game/bot/bot_goal.h
	./Darklight/game/bot/bot_weights.cpp
	./Darklight/game/bot/bot_weights.h
	./Darklight/game/bot/bot_weapons.cpp
	./Darklight/game/bot/bot_weapons.h
	./Darklight/game/bot/bot.h
	./Darklight/game/bot/botai/BotAI.cpp
	./Darklight/game/bot/botai/BotAI_Seek_NBG.cpp
	./Darklight/game/bot/botai/BotAI_Battle_NBG.cpp
	./Darklight/game/bot/botai/BotAI_Battle_Fight.cpp
	./Darklight/game/bot/botai/BotAI_Battle_Chase.cpp
	./Darklight/game/bot/botai/BotAI_Battle_Retreat.cpp
	./Darklight/game/bot/botai/BotAI_respawn.cpp
	./Darklight/game/bot/botai/BotAI_SeekLTG.cpp
	./Darklight/game/bot/botai/BotAI.h
	./Darklight/game/ReflectionProbe.cpp
	./Darklight/game/ReflectionProbe.h
	./Darklight/game/Emitter.cpp
	./Darklight/game/Emitter.h
	./Darklight/game/game_fx.cpp
	./Darklight/game/game_navigation.cpp
	./Darklight/game/game_precompiled.cpp
	./Darklight/game/game_precompiled.h
	./Darklight/game/Actor.cpp
	./Darklight/game/Actor.h
	./Darklight/game/AF.cpp
	./Darklight/game/AF.h
	./Darklight/game/AFEntity.cpp
	./Darklight/game/AFEntity.h
	./Darklight/game/anim
	./Darklight/game/Camera.cpp
	./Darklight/game/Camera.h
	./Darklight/game/Entity.cpp
	./Darklight/game/Entity.h
	./Darklight/game/Game.def
	./Darklight/game/GameEdit.cpp
	./Darklight/game/GameEdit.h
	./Darklight/game/gamesys
	./Darklight/game/Game_local.cpp
	./Darklight/game/Game_local.h
	./Darklight/game/Game_network.cpp
	./Darklight/game/Game_packet.cpp
	./Darklight/game/Game_parallel.cpp
	./Darklight/game/Game_render.cpp
	./Darklight/game/IK.cpp
	./Darklight/game/IK.h
	./Darklight/game/Item.cpp
	./Darklight/game/Item.h
	./Darklight/game/Light.cpp
	./Darklight/game/Light.h
	./Darklight/game/Misc.cpp
	./Darklight/game/Misc.h
	./Darklight/game/Moveable.cpp
	./Darklight/game/Moveable.h
	./Darklight/game/Mover.cpp
	./Darklight/game/Mover.h
	./Darklight/game/MultiplayerGame.cpp
	./Darklight/game/MultiplayerGame.h
	./Darklight/game/physics
	./Darklight/game/Player.cpp
	./Darklight/game/Player_states.cpp
	./Darklight/game/Player.h
	./Darklight/game/PlayerIcon.cpp
	./Darklight/game/PlayerIcon.h
	./Darklight/game/Projectile.cpp
	./Darklight/game/Projectile.h
	./Darklight/game/PortalSky.cpp
	./Darklight/game/PortalSky.h
	./Darklight/game/Pvs.cpp
	./Darklight/game/Pvs.h
	./Darklight/game/script
	./Darklight/game/Sound.cpp
	./Darklight/game/Sound.h
	./Darklight/game/Target.cpp
	./Darklight/game/Target.h
	./Darklight/game/Trigger.cpp
	./Darklight/game/Trigger.h
	./Darklight/game/Weapon.cpp
	./Darklight/game/Weapon.h
	./Darklight/game/weapons
	./Darklight/game/WorldSpawn.cpp
	./Darklight/game/WorldSpawn.h
	./Darklight/game/anim/Anim.cpp
	./Darklight/game/anim/Anim.h
	./Darklight/game/anim/Anim_Blend.cpp
	./Darklight/game/anim/Anim_Import.cpp
	./Darklight/game/anim/Anim_Testmodel.cpp
	./Darklight/game/anim/Anim_Testmodel.h
	./Darklight/game/gamesys/Class.cpp
	./Darklight/game/gamesys/Class.h
	./Darklight/game/gamesys/DebugGraph.cpp
	./Darklight/game/gamesys/DebugGraph.h
	./Darklight/game/gamesys/Event.cpp
	./Darklight/game/gamesys/Event.h
	./Darklight/game/gamesys/NoGameTypeInfo.h
	./Darklight/game/gamesys/SaveGame.cpp
	./Darklight/game/gamesys/SaveGame.h
	./Darklight/game/gamesys/SysCmds.cpp
	./Darklight/game/gamesys/SysCmds.h
	./Darklight/game/gamesys/SysCvar.cpp
	./Darklight/game/gamesys/SysCvar.h
	./Darklight/game/gamesys/State.cpp
	./Darklight/game/gamesys/State.h
	./Darklight/game/physics/Clip.cpp
	./Darklight/game/physics/Clip.h
	./Darklight/game/physics/Force.cpp
	./Darklight/game/physics/Force.h
	./Darklight/game/physics/Force_Constant.cpp
	./Darklight/game/physics/Force_Constant.h
	./Darklight/game/physics/Force_Drag.cpp
	./Darklight/game/physics/Force_Drag.h
	./Darklight/game/physics/Force_Field.cpp
	./Darklight/game/physics/Force_Field.h
	./Darklight/game/physics/Force_Spring.cpp
	./Darklight/game/physics/Force_Spring.h
	./Darklight/game/physics/Physics.cpp
	./Darklight/game/physics/Physics.h
	./Darklight/game/physics/Physics_Actor.cpp
	./Darklight/game/physics/Physics_Actor.h
	./Darklight/game/physics/Physics_AF.cpp
	./Darklight/game/physics/Physics_AF.h
	./Darklight/game/physics/Physics_Base.cpp
	./Darklight/game/physics/Physics_Base.h
	./Darklight/game/physics/Physics_Monster.cpp
	./Darklight/game/physics/Physics_Monster.h
	./Darklight/game/physics/Physics_Parametric.cpp
	./Darklight/game/physics/Physics_Parametric.h
	./Darklight/game/physics/Physics_Player.cpp
	./Darklight/game/physics/Physics_Player.h
	./Darklight/game/physics/Physics_RigidBody.cpp
	./Darklight/game/physics/Physics_RigidBody.h
	./Darklight/game/physics/Physics_Static.cpp
	./Darklight/game/physics/Physics_Static.h
	./Darklight/game/physics/Physics_StaticMulti.cpp
	./Darklight/game/physics/Physics_StaticMulti.h
	./Darklight/game/physics/Push.cpp
	./Darklight/game/physics/Push.h
	./Darklight/game/script/Script_Compiler.cpp
	./Darklight/game/script/Script_Compiler.h
	./Darklight/game/script/Script_Interpreter.cpp
	./Darklight/game/script/Script_Interpreter.h
	./Darklight/game/script/Script_Program.cpp
	./Darklight/game/script/Script_Program.h
	./Darklight/game/script/Script_Thread.cpp
	./Darklight/game/script/Script_Thread.h
	./Darklight/game/weapons/Weapon_bfg.cpp
	./Darklight/game/weapons/Weapon_bfg.h
	./Darklight/game/weapons/Weapon_chaingun.cpp
	./Darklight/game/weapons/Weapon_chaingun.h
	./Darklight/game/weapons/Weapon_fist.cpp
	./Darklight/game/weapons/Weapon_fist.h
	./Darklight/game/weapons/Weapon_machinegun.cpp
	./Darklight/game/weapons/Weapon_machinegun.h
	./Darklight/game/weapons/Weapon_pistol.cpp
	./Darklight/game/weapons/Weapon_pistol.h
	./Darklight/game/weapons/Weapon_plasmagun.cpp
	./Darklight/game/weapons/Weapon_plasmagun.h
	./Darklight/game/weapons/Weapon_rocketlauncher.cpp
	./Darklight/game/weapons/Weapon_rocketlauncher.h
	./Darklight/game/weapons/Weapon_shotgun.cpp
	./Darklight/game/weapons/Weapon_shotgun.h
	./Engine/sys/sys_game.h
)


set(src_launcher
	./Darklight/launcher/launcher_main.cpp
	./engine/sys/win32/rc/doom.rc
	./engine/sys/win32/rc/doom_resource.h
	./engine/sys/win32/rc/res/BEVEL.BMP
	./engine/sys/win32/rc/res/BITMAP2.BMP
	./engine/sys/win32/rc/res/BMP00001.BMP
	./engine/sys/win32/rc/res/bmp00002.bmp
	./engine/sys/win32/rc/res/bmp00003.bmp
	./engine/sys/win32/rc/res/bmp00004.bmp
	./engine/sys/win32/rc/res/bmp00005.bmp
	./engine/sys/win32/rc/res/BMP0002.BMP
	./engine/sys/win32/rc/res/cchsb.bmp
	./engine/sys/win32/rc/res/ccrgb.bmp
	./engine/sys/win32/rc/res/dbg_back.bmp
	./engine/sys/win32/rc/res/dbg_breakpoint.ico
	./engine/sys/win32/rc/res/dbg_current.ico
	./engine/sys/win32/rc/res/dbg_currentline.ico
	./engine/sys/win32/rc/res/dbg_empty.ico
	./engine/sys/win32/rc/res/dbg_open.bmp
	./engine/sys/win32/rc/res/dbg_toolbar.bmp
	./engine/sys/win32/rc/res/DEFTEX.WAL
	./engine/sys/win32/rc/res/doom.ico
	./engine/sys/win32/rc/res/ENDCAP.BMP
	./engine/sys/win32/rc/res/fpoint.cur
	./engine/sys/win32/rc/res/fxeditor.ico
	./engine/sys/win32/rc/res/fxed_link.ico
	./engine/sys/win32/rc/res/fxed_toolbar.bmp
	./engine/sys/win32/rc/res/GetString.htm
	./engine/sys/win32/rc/res/guied.ico
	./engine/sys/win32/rc/res/guied_collapse.ico
	./engine/sys/win32/rc/res/guied_expand.ico
	./engine/sys/win32/rc/res/guied_hand.cur
	./engine/sys/win32/rc/res/guied_nav_visible.ico
	./engine/sys/win32/rc/res/guied_nav_visibledisabled.ico
	./engine/sys/win32/rc/res/guied_scripts.ico
	./engine/sys/win32/rc/res/guied_scripts_white.ico
	./engine/sys/win32/rc/res/guied_viewer_toolbar.bmp
	./engine/sys/win32/rc/res/IBEVEL.BMP
	./engine/sys/win32/rc/res/icon2.ico
	./engine/sys/win32/rc/res/IENDCAP.BMP
	./engine/sys/win32/rc/res/logo_sm3dfx.bmp
	./engine/sys/win32/rc/res/matedtree.bmp
	./engine/sys/win32/rc/res/MaterialEditor.ico
	./engine/sys/win32/rc/res/MEFileToolbar.bmp
	./engine/sys/win32/rc/res/MEtoolbar.bmp
	./engine/sys/win32/rc/res/me_disabled_icon.ico
	./engine/sys/win32/rc/res/me_enabled.ico
	./engine/sys/win32/rc/res/me_off_icon.ico
	./engine/sys/win32/rc/res/me_on_icon.ico
	./engine/sys/win32/rc/res/PropTree.rc2
	./engine/sys/win32/rc/res/Q.BMP
	./engine/sys/win32/rc/res/qe3.ico
	./engine/sys/win32/rc/res/Radiant.ico
	./engine/sys/win32/rc/res/RADIANT3.GIF
	./engine/sys/win32/rc/res/RadiantDoc.ico
	./engine/sys/win32/rc/res/shaderbar.bmp
	./engine/sys/win32/rc/res/shaderdoc.ico
	./engine/sys/win32/rc/res/shaderframe.ico
	./engine/sys/win32/rc/res/spliter.cur
	./engine/sys/win32/rc/res/Toolbar.bmp
	./engine/sys/win32/rc/res/TOOLBAR1.BMP
	./engine/sys/win32/rc/res/TOOLBAR2.BMP
	./engine/sys/win32/rc/res/VIEWDEFA.BMP
	./engine/sys/win32/rc/res/VIEWOPPO.BMP
)

# idLib project
add_definitions(-D_AFXDLL -D_WINDOWS -D_CRT_SECURE_NO_DEPRECATE)
add_library(idLib STATIC ${src_idlib})
add_precompiled_header( idLib precompiled.h SOURCE_CXX ./engine/idlib/precompiled.cpp )
target_compile_definitions(idLib PRIVATE NAME=$<TARGET_FILE_NAME:idLib>)
set_target_properties(idLib PROPERTIES LINK_FLAGS "/PDB:\"idlib.pdb\"")

# Game Project
add_library(game MODULE  ${src_game} ${src_cgame} )
target_compile_definitions(game PRIVATE GAME_DLL=1)
target_link_libraries(game idLib)
add_precompiled_header( game game_precompiled.h  SOURCE_CXX ./Darklight/game/game_precompiled.cpp )
set_target_properties(game PROPERTIES OUTPUT_NAME "gamex64" LINK_FLAGS "/PDB:\"Game.pdb\"")

get_filename_component(DIR_ONE_ABOVE ../ ABSOLUTE)

add_custom_target(
    GameCustomTarget
    COMMAND ${DIR_ONE_ABOVE}/editor/BuildTool.exe
)
add_dependencies(game GameCustomTarget)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/full_sdk_license.txt") 	
	Message("Including Tools...")
	include( Tools.cmake )

	Message("Including Engine...")
	include( DarkEngine.cmake )
else()
	Message("Generated SDK projects...")
endif()

# Launcher Project
add_executable(Launcher ${src_launcher})
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/full_sdk_license.txt") 	
	target_link_libraries(Launcher DoomDLL)
endif()
set_target_properties(Launcher PROPERTIES OUTPUT_NAME "Darklight" LINK_FLAGS "/STACK:16777216,16777216 /SUBSYSTEM:WINDOWS /PDB:\"Darklight.pdb\"")
