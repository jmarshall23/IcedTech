# DarkEngine.cmake
#

set(src_engine
	# Collision System
	./engine/cm/CollisionModel.cpp
	./engine/cm/CollisionModel_trace.cpp
	./engine/cm/CollisionModel.h
	./engine/cm/CollisionModel_rotate.cpp
	./engine/cm/CollisionModel_contents.cpp
	./engine/cm/CollisionModel_debug.cpp
	./engine/cm/CollisionModel_files.cpp
	./engine/cm/CollisionModel_local.h
	./engine/cm/CollisionModel_contacts.cpp
	./engine/cm/CollisionModel_translate.cpp
	./engine/cm/CollisionModel_load.cpp

	# Framework
	./engine/framework/BuildDefines.h
	./engine/framework/BuildVersion.h
	./engine/framework/CmdSystem.cpp
	./engine/framework/CmdSystem.h
	./engine/framework/Common.cpp
	./engine/framework/Common_loadscreen.cpp
	./engine/framework/Common_network.cpp
	./engine/framework/Common_network.h
	./engine/framework/Common.h
	./engine/framework/Compressor.cpp
	./engine/framework/Compressor.h
	./engine/framework/Console.cpp
	./engine/framework/Console.h
	./engine/framework/ConsoleHistory.cpp
	./engine/framework/ConsoleHistory.h
	./engine/framework/CVarSystem.cpp
	./engine/framework/CVarSystem.h
	./engine/framework/DeclAF.cpp
	./engine/framework/DeclAF.h
	./engine/framework/DeclEntityDef.cpp
	./engine/framework/DeclEntityDef.h
	./engine/framework/DeclManager.cpp
	./engine/framework/DeclManager.h
	./engine/framework/DeclPDA.cpp
	./engine/framework/DeclPDA.h
	./engine/framework/DeclSkin.cpp
	./engine/framework/DeclSkin.h
	./engine/framework/DeclTable.cpp
	./engine/framework/DeclTable.h
	./engine/framework/DemoChecksum.h
	./engine/framework/DemoFile.cpp
	./engine/framework/DemoFile.h
	./engine/framework/EditField.cpp
	./engine/framework/EditField.h
	./engine/framework/EventLoop.cpp
	./engine/framework/EventLoop.h
	./engine/framework/File.cpp
	./engine/framework/File.h
	./engine/framework/FileSystem.cpp
	./engine/framework/FileSystem.h
	./engine/framework/KeyInput.cpp
	./engine/framework/KeyInput.h
	./engine/framework/Licensee.h
	./engine/framework/ParallelJobList.cpp
	./engine/framework/ParallelJobList.h
	./engine/framework/ParallelJobListLocal.h
	./engine/framework/ParallelJobList_JobHeaders.h
	./engine/framework/Session.cpp
	./engine/framework/Session.h
	./engine/framework/Session_local.h
	./engine/framework/Session_menu.cpp
	./engine/framework/Thread.cpp
	./engine/framework/Thread.h
	./engine/framework/Unzip.cpp
	./engine/framework/Unzip.h
	./engine/framework/UsercmdGen.cpp
	./engine/framework/UsercmdGen.h
	
	#NavMesh
	./engine/navigation/Nav_File.cpp
	./engine/navigation/Nav_public.h
	./engine/navigation/Nav_Manager.cpp
	./engine/navigation/Nav_local.h

	#Model
	./engine/model/Model.cpp
	./engine/model/Model.h
	./engine/model/ModelDecal.cpp
	./engine/model/ModelDecal.h
	./engine/model/ModelManager.cpp
	./engine/model/ModelManager.h
	./engine/model/ModelOverlay.cpp
	./engine/model/ModelOverlay.h
	./engine/model/Model_ase.cpp
	./engine/model/Model_ase.h
	./engine/model/Model_mdr.cpp
	./engine/model/Model_mdr.h
	./engine/model/Model_beam.cpp
	./engine/model/Model_ColladaHelper.h
	./engine/model/Model_ColladaParser.cpp
	./engine/model/Model_ColladaParser.h
	./engine/model/Model_local.h
	./engine/model/Model_lwo.cpp
	./engine/model/Model_lwo.h
	./engine/model/Model_ma.cpp
	./engine/model/Model_ma.h
	./engine/model/Model_md3.cpp
	./engine/model/Model_md3.h
	./engine/model/Model_md5.cpp
	./engine/model/Model_fx.cpp
	./engine/model/Model_obj.cpp
	./engine/model/Model_sprite.cpp
	./engine/model/Model_terrain.cpp

	#Renderer
	./engine/renderer/BinaryImage.cpp
	./engine/renderer/BinaryImage.h
	./engine/renderer/BinaryImageData.h
	./engine/renderer/BufferObject.cpp
	./engine/renderer/BufferObject.h
	./engine/renderer/Cinematic.cpp
	./engine/renderer/Cinematic.h
	./engine/renderer/Color
	./engine/renderer/render/render.cpp
	./engine/renderer/render/render_common.cpp
	./engine/renderer/render/render_depthfill.cpp
	./engine/renderer/render/render_feedback.cpp
	./engine/renderer/render/render_interaction.cpp
	./engine/renderer/render/render_occlusion.cpp
	./engine/renderer/render/render_particles.cpp
	./engine/renderer/render/render_shadow.cpp
	./engine/renderer/render/render_skin.cpp
	./engine/renderer/DXT
	./engine/renderer/GLMatrix.cpp
	./engine/renderer/GLMatrix.h
	./engine/renderer/GpuTimer.cpp
	./engine/renderer/GpuTimer.h
	./engine/renderer/GraphicsContext.h
	./engine/renderer/GuiModel.cpp
	./engine/renderer/GuiModel.h
	./engine/renderer/Image.h
	./engine/renderer/ImageManager.cpp
	./engine/renderer/ImageOpts.h
	./engine/renderer/Image_ies.cpp
	./engine/renderer/Image_files.cpp
	./engine/renderer/Image_GL.cpp
	./engine/renderer/Image_intrinsic.cpp
	./engine/renderer/Image_load.cpp
	./engine/renderer/Image_process.cpp
	./engine/renderer/Image_program.cpp
	./engine/renderer/Interaction.cpp
	./engine/renderer/Interaction.h
	./engine/renderer/Material.cpp
	./engine/renderer/Material.h
	./engine/renderer/MegaTexture.cpp
	./engine/renderer/MegaTexture.h
	./engine/renderer/MegaTextureBuild.cpp
	./engine/renderer/MegaTextureFile.cpp
	./engine/renderer/RenderEntity.cpp
	./engine/renderer/RenderMatrix.cpp
	./engine/renderer/RenderMatrix.h
	./engine/renderer/RenderProgs.cpp
	./engine/renderer/RenderProgs.h
	./engine/renderer/RenderProgs_GLSL.cpp
	./engine/renderer/RenderShadows.cpp
	./engine/renderer/RenderSystem.cpp
	./engine/renderer/RenderSystem.h
	./engine/renderer/RenderSystem_init.cpp
	./engine/renderer/RenderTexture.cpp
	./engine/renderer/RenderTexture.h
	./engine/renderer/RenderWorld.cpp
	./engine/renderer/RenderWorld.h
	./engine/renderer/RenderWorld_demo.cpp
	./engine/renderer/RenderWorld_load.cpp
	./engine/renderer/RenderWorld_local.h
	./engine/renderer/simplex.h
	./engine/renderer/tr_backend.cpp
	./engine/renderer/tr_deform.cpp
	./engine/renderer/tr_font.cpp
	./engine/renderer/tr_guisurf.cpp
	./engine/renderer/tr_light.cpp
	./engine/renderer/tr_lightrun.cpp
	./engine/renderer/tr_local.h
	./engine/renderer/tr_main.cpp
	./engine/renderer/tr_orderIndexes.cpp
	./engine/renderer/tr_polytope.cpp
	./engine/renderer/tr_rendertools.cpp
	./engine/renderer/tr_subview.cpp
	./engine/renderer/tr_trace.cpp
	./engine/renderer/tr_trisurf.cpp
	./engine/renderer/VertexCache.cpp
	./engine/renderer/VertexCache.h
	./engine/renderer/VirtualTexture.h
	./engine/renderer/VirtualTextureFeedbackJob.cpp
	./engine/renderer/VirtualTextureSystem.cpp
	./engine/renderer/VirtualTextureTranscode.cpp
	./engine/renderer/Color/ColorSpace.cpp
	./engine/renderer/Color/ColorSpace.h
	./engine/renderer/DXT/DXTCodec.h
	./engine/renderer/DXT/DXTDecoder.cpp
	./engine/renderer/DXT/DXTEncoder.cpp

	# Sound System
	./engine/sound/snd_emitter.cpp
	./engine/sound/snd_local.h
	./engine/sound/snd_shader.cpp
	./engine/sound/snd_system.cpp
	./engine/sound/snd_world.cpp
	./engine/sound/sound.h
	./engine/sound/SoundVoice.cpp
	./engine/sound/SoundVoice.h
	./engine/sound/WaveFile.cpp
	./engine/sound/WaveFile.h
	./engine/sound/OpenAL
	./engine/sound/OpenAL/AL_SoundHardware.cpp
	./engine/sound/OpenAL/AL_SoundHardware.h
	./engine/sound/OpenAL/AL_SoundSample.cpp
	./engine/sound/OpenAL/AL_SoundSample.h
	./engine/sound/OpenAL/AL_SoundVoice.cpp
	./engine/sound/OpenAL/AL_SoundVoice.h

	# System
	./engine/sys/sys_intrinsics.h
	./engine/sys/sys_local.cpp
	./engine/sys/sys_local.h
	./engine/sys/sys_public.h
	./engine/sys/sys_threading.h
	./engine/sys/win32/eax.h
	./engine/sys/win32/eaxguid.lib
	./engine/sys/win32/rc
	./engine/sys/win32/sdk
	./engine/sys/win32/win_cpu.cpp
	./engine/sys/win32/win_glimp.cpp
	./engine/sys/win32/win_input.cpp
	./engine/sys/win32/win_local.h
	./engine/sys/win32/win_main.cpp
	./engine/sys/win32/win_qgl.cpp
	./engine/sys/win32/win_shared.cpp
	./engine/sys/win32/win_snd.cpp
	./engine/sys/win32/win_syscon.cpp
	./engine/sys/win32/win_taskkeyhook.cpp
	./engine/sys/win32/win_thread.cpp
	./engine/sys/win32/win_wndproc.cpp
	./engine/sys/win32/rc/CreateResourceIDs.cpp

	./engine/ui/BindWindow.cpp
	./engine/ui/BindWindow.h
	./engine/ui/ChoiceWindow.cpp
	./engine/ui/ChoiceWindow.h
	./engine/ui/DeviceContext.cpp
	./engine/ui/DeviceContext.h
	./engine/ui/EditWindow.cpp
	./engine/ui/EditWindow.h
	./engine/ui/FieldWindow.cpp
	./engine/ui/FieldWindow.h
	./engine/ui/GameBearShootWindow.cpp
	./engine/ui/GameBearShootWindow.h
	./engine/ui/GameBustOutWindow.cpp
	./engine/ui/GameBustOutWindow.h
	./engine/ui/GameSSDWindow.cpp
	./engine/ui/GameSSDWindow.h
	./engine/ui/GuiScript.cpp
	./engine/ui/GuiScript.h
	./engine/ui/ListGUI.cpp
	./engine/ui/ListGUI.h
	./engine/ui/ListGUILocal.h
	./engine/ui/ListWindow.cpp
	./engine/ui/ListWindow.h
	./engine/ui/MarkerWindow.cpp
	./engine/ui/MarkerWindow.h
	./engine/ui/Rectangle.h
	./engine/ui/RegExp.cpp
	./engine/ui/RegExp.h
	./engine/ui/RegExp_old.h
	./engine/ui/RenderWindow.cpp
	./engine/ui/RenderWindow.h
	./engine/ui/SimpleWindow.cpp
	./engine/ui/SimpleWindow.h
	./engine/ui/SliderWindow.cpp
	./engine/ui/SliderWindow.h
	./engine/ui/UserInterface.cpp
	./engine/ui/UserInterface.h
	./engine/ui/UserInterfaceLocal.h
	./engine/ui/Window.cpp
	./engine/ui/Window.h
	./engine/ui/Winvar.cpp
	./engine/ui/Winvar.h
)

set(src_launcher
	./engine/sys/win32/win_launcher.cpp
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

set(src_external
	./engine/external/zlib/adler32.c
	./engine/external/zlib/compress.c
	./engine/external/zlib/crc32.c
	./engine/external/zlib/deflate.c
	./engine/external/zlib/gzclose.c
	./engine/external/zlib/gzlib.c
	./engine/external/zlib/gzread.c
	./engine/external/zlib/gzwrite.c
	./engine/external/zlib/infback.c
	./engine/external/zlib/inffast.c
	./engine/external/zlib/inflate.c
	./engine/external/zlib/inftrees.c
	./engine/external/zlib/trees.c
	./engine/external/zlib/uncompr.c
	./engine/external/zlib/zutil.c
	./engine/external/zlib/crc32.h
	./engine/external/zlib/deflate.h	
	./engine/external/zlib/gzguts.h
	./engine/external/zlib/inffast.h
	./engine/external/zlib/inffixed.h
	./engine/external/zlib/inflate.h
	./engine/external/zlib/inftrees.h
	./engine/external/zlib/trees.h
	./engine/external/zlib/zconf.h
	./engine/external/zlib/zconf.h.in
	./engine/external/zlib/zlib.h
	./engine/external/zlib/zutil.h
	./engine/external/zlib/minizip/ioapi.h
	./engine/external/zlib/minizip/unzip.h
	./engine/external/zlib/minizip/zip.h

	./engine/external/glew/glew.c
	./engine/external/glew/glew.h
	./engine/external/glew/qgllib.h
	./engine/external/glew/wglew.h

	./engine/external/enet/callbacks.c
	./engine/external/enet/callbacks.h
	./engine/external/enet/compress.c
	./engine/external/enet/enet.h
	./engine/external/enet/host.c
	./engine/external/enet/list.c
	./engine/external/enet/list.h
	./engine/external/enet/packet.c
	./engine/external/enet/peer.c
	./engine/external/enet/protocol.c
	./engine/external/enet/protocol.h
	./engine/external/enet/time.h
	./engine/external/enet/types.h
	./engine/external/enet/utility.h
	./engine/external/enet/win32.c
	./engine/external/enet/win32.h

	./engine/external/png/png.c
	./engine/external/png/pngerror.c
	./engine/external/png/pnggccrd.c
	./engine/external/png/pngget.c
	./engine/external/png/pngmem.c
	./engine/external/png/pngpread.c
	./engine/external/png/pngread.c
	./engine/external/png/pngrio.c
	./engine/external/png/pngrtran.c
	./engine/external/png/pngrutil.c
	./engine/external/png/pngset.c
	./engine/external/png/pngtrans.c
	./engine/external/png/pngvcrd.c
	./engine/external/png/pngwio.c
	./engine/external/png/pngwrite.c
	./engine/external/png/pngwtran.c
	./engine/external/png/pngwutil.c
	./engine/external/png/png.h
	./engine/external/png/pngconf.h

	./engine/external/iesloader/ies_loader.cpp
	./engine/external/iesloader/ies_loader.h


	./engine/external/recast/Include/Recast.h
	./engine/external/recast/Include/RecastAlloc.h
	./engine/external/recast/Include/RecastAssert.h
	./engine/external/recast/Source/Recast.cpp
	./engine/external/recast/Source/RecastAlloc.cpp
	./engine/external/recast/Source/RecastArea.cpp
	./engine/external/recast/Source/RecastAssert.cpp
	./engine/external/recast/Source/RecastContour.cpp
	./engine/external/recast/Source/RecastFilter.cpp
	./engine/external/recast/Source/RecastLayers.cpp
	./engine/external/recast/Source/RecastMesh.cpp
	./engine/external/recast/Source/RecastMeshDetail.cpp
	./engine/external/recast/Source/RecastRasterization.cpp
	./engine/external/recast/Source/RecastRegion.cpp

	./engine/external/detour/Include/DetourAlloc.h
	./engine/external/detour/Include/DetourAssert.h
	./engine/external/detour/Include/DetourCommon.h
	./engine/external/detour/Include/DetourMath.h
	./engine/external/detour/Include/DetourNavMesh.h
	./engine/external/detour/Include/DetourNavMeshBuilder.h
	./engine/external/detour/Include/DetourNavMeshQuery.h
	./engine/external/detour/Include/DetourNode.h
	./engine/external/detour/Include/DetourStatus.h
	./engine/external/detour/Source/DetourAlloc.cpp
	./engine/external/detour/Source/DetourAssert.cpp
	./engine/external/detour/Source/DetourCommon.cpp
	./engine/external/detour/Source/DetourNavMesh.cpp
	./engine/external/detour/Source/DetourNavMeshBuilder.cpp
	./engine/external/detour/Source/DetourNavMeshQuery.cpp
	./engine/external/detour/Source/DetourNode.cpp



	./engine/external/irrxml/src/CXMLReaderImpl.h
	./engine/external/irrxml/src/fast_atof.h
	./engine/external/irrxml/src/heapsort.h
	./engine/external/irrxml/src/irrArray.h
	./engine/external/irrxml/src/irrString.h
	./engine/external/irrxml/src/irrTypes.h
	./engine/external/irrxml/src/irrXML.cpp
	./engine/external/irrxml/src/irrXML.h

	./engine/external/imgui/imconfig.h
	./engine/external/imgui/imgui.cpp
	./engine/external/imgui/imgui.h
	./engine/external/imgui/imgui_draw.cpp
	./engine/external/imgui/imgui_internal.h
	./engine/external/imgui/imgui_widgets.cpp
	./engine/external/imgui/imstb_rectpack.h
	./engine/external/imgui/imstb_textedit.h
	./engine/external/imgui/imstb_truetype.h
	./engine/external/imgui/examples/imgui_impl_opengl3.cpp
	./engine/external/imgui/examples/imgui_impl_opengl3.h
	./engine/external/imgui/examples/imgui_impl_win32.cpp
	./engine/external/imgui/examples/imgui_impl_win32.h
)

set(FFmpeg_LIBRARIES
				"${CMAKE_SOURCE_DIR}/engine/external/ffmpeg-win64/lib/avcodec.lib"
				"${CMAKE_SOURCE_DIR}/engine/external/ffmpeg-win64/lib/avformat.lib"
				"${CMAKE_SOURCE_DIR}/engine/external/ffmpeg-win64/lib/avutil.lib"
				"${CMAKE_SOURCE_DIR}/engine/external/ffmpeg-win64/lib/swscale.lib")

# Engine Directories
include_directories(./engine/external/dxsdk/Include)
include_directories(./engine/external/zlib)
include_directories(./engine/external/png)
include_directories(./engine/external/irrxml/src)

# External Static Library
add_library(External STATIC ${src_external})
set_target_properties(External PROPERTIES LINK_FLAGS "/PDB:\"External.pdb\"")
target_include_directories(External PRIVATE ./engine/external/Recast/include ./engine/external/detour/Include)

# DoomDLL Project
add_definitions(-D__DOOM_DLL__)
add_library(DoomDLL SHARED  ${src_engine} ./engine/framework/Engine_precompiled.cpp )
target_link_libraries(DoomDLL idLib External Tools "opengl32.lib" "dxguid.lib" "glu32.lib" "dinput8.lib" "winmm.lib" "wsock32.lib" "dbghelp.lib" "iphlpapi.lib" "${CMAKE_SOURCE_DIR}/engine/external/openal/out/build/x64-Release/OpenAL32.lib" ${FFmpeg_LIBRARIES})
add_precompiled_header( DoomDLL Engine_precompiled.h  SOURCE_CXX ./engine/framework/Engine_precompiled.cpp )
set_target_properties(DoomDLL PROPERTIES OUTPUT_NAME "DoomDLL" LINK_FLAGS "/STACK:16777216,16777216 /PDB:\"DoomDLL.pdb\" /DEF:${CMAKE_CURRENT_SOURCE_DIR}/exports.def")
target_include_directories(DoomDLL PRIVATE ./engine/external/Recast/include ./engine/external/detour/Include ./engine/external/openal/include ./engine/external/ffmpeg-win64/include)

# Launcher Project
add_executable(Launcher ${src_launcher})
target_link_libraries(Launcher DoomDLL)
set_target_properties(Launcher PROPERTIES OUTPUT_NAME "Darklight" LINK_FLAGS "/STACK:16777216,16777216 /SUBSYSTEM:WINDOWS /PDB:\"Darklight.pdb\"")

# Dedicated Server
add_executable(DoomDedicatedServer ${src_engine} ./engine/framework/Dedicated_precompiled.cpp)
target_compile_definitions(DoomDedicatedServer PRIVATE ID_DEDICATED ID_ENGINE_EXECUTABLE)
add_precompiled_header( DoomDedicatedServer Engine_precompiled.h  SOURCE_CXX ./engine/framework/Dedicated_precompiled.cpp )
target_link_libraries(DoomDedicatedServer idLib External "opengl32.lib" "dxguid.lib" "glu32.lib" "dinput8.lib" "winmm.lib" "wsock32.lib" "dbghelp.lib" "iphlpapi.lib" "${CMAKE_SOURCE_DIR}/engine/external/openal/out/build/x64-Release/OpenAL32.lib"  ${FFmpeg_LIBRARIES})
target_include_directories(DoomDedicatedServer PRIVATE ./engine/external/Recast/include ./engine/external/detour/Include ./engine/external/openal/include ./engine/external/ffmpeg-win64/include)
set_target_properties(DoomDedicatedServer PROPERTIES OUTPUT_NAME "DarklightDedicated" LINK_FLAGS "/STACK:16777216,16777216 /SUBSYSTEM:WINDOWS /PDB:\"DarklightDedicated.pdb\"")
add_dependencies(DoomDedicatedServer DoomDLL )