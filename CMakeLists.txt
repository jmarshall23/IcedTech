cmake_minimum_required(VERSION 2.6)
project(IcedTech)
if(NOT COMMAND add_compile_options)
        function(add_compile_options)
                foreach(arg ${ARGN})
                        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${arg}" PARENT_SCOPE)
                        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${arg}" PARENT_SCOPE)
                endforeach()
        endfunction()
endif()

option(LINUX_RELEASE_BINS "Set RPATH to \$ORIGIN/libs/ for Linux binary releases" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/sys/cmake")

if(LINUX_RELEASE_BINS)
        message(STATUS "Setting RPATH to \$ORIGIN/libs/ so you can put dependencies in there")
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
        set(CMAKE_INSTALL_RPATH "$ORIGIN/libs")
        set(ldflags "${ldflags} -Wl,-z,origin") # not sure this is even needed, but shouldn't hurt
else()
        set(CMAKE_SKIP_RPATH ON CACHE BOOL "Skip RPATH" FORCE)
endif()
set(DOOM3BINARY "DOOM3-ICEDTECH")
include(CheckCXXCompilerFlag)
include(GNUInstallDirs OPTIONAL RESULT_VARIABLE GNUINSTALLDIRS)
option(CORE		"Build the core" ON)
option(ONATIVE          "Optimize for the host CPU" ON)
option(SDL2                     "Use SDL2 instead of SDL1.2" ON)
if(NOT CMAKE_SYSTEM_PROCESSOR)
        message(FATAL_ERROR "No target CPU architecture set")
endif()

if(NOT CMAKE_SYSTEM_NAME)
        message(FATAL_ERROR "No target OS set")
endif()

# target cpu
set(cpu ${CMAKE_SYSTEM_PROCESSOR})
if(cpu STREQUAL "powerpc")
        set(cpu "ppc")
elseif(cpu MATCHES "i.86")
        set(cpu "x86")
endif()

if(MSVC AND CMAKE_CL_64)
        set(cpu "amd64")
endif()


# build type
if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()
# libs
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})

set(CMAKE_REQUIRED_INCLUDES ${JPEG_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${JPEG_LIBRARY})

find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR})

if(NOT AROS)
	find_package(X11 REQUIRED)
	include_directories(${X11_INCLUDE_DIR})
endif()

if (SDL2)
	find_package(SDL2 REQUIRED)
	include_directories(${SDL2_INCLUDE_DIR})
	set(SDLx_LIBRARY ${SDL2_LIBRARY})
else()
	find_package(SDL REQUIRED)
	include_directories(${SDL_INCLUDE_DIR})
	set(SDLx_LIBRARY ${SDL_LIBRARY})
endif()

find_package(CURL QUIET)
if(CURL_FOUND)
	set(ID_ENABLE_CURL ON)
	include_directories(${CURL_INCLUDE_DIR})
else()
	message(STATUS "libcurl not found, server downloads won't be available")
	set(ID_ENABLE_CURL OFF)
	set(CURL_LIBRARY "")
endif()

# compiler specific flags
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-pipe)
	add_compile_options(-Wall)

	if(NOT CMAKE_CROSSCOMPILING AND ONATIVE)
		add_compile_options(-march=native)
	elseif(NOT APPLE AND cpu STREQUAL "x86")
		add_compile_options(-march=pentium3)
	endif()

	set(CMAKE_C_FLAGS_DEBUG "-g -ggdb -D_DEBUG -O0")
	set(CMAKE_C_FLAGS_DEBUGALL "-g -ggdb -D_DEBUG")
	set(CMAKE_C_FLAGS_PROFILE "-g -ggdb -D_DEBUG -O1 -fno-omit-frame-pointer")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -fno-unsafe-math-optimizations -fno-math-errno -fno-trapping-math -fomit-frame-pointer")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -ggdb -O2 -fno-unsafe-math-optimizations -fno-math-errno -fno-trapping-math -fno-omit-frame-pointer")
	set(CMAKE_C_FLAGS_MINSIZEREL "-Os -fno-unsafe-math-optimizations -fno-math-errno -fno-trapping-math -fomit-frame-pointer")

	set(CMAKE_CXX_FLAGS_DEBUGALL ${CMAKE_C_FLAGS_DEBUGALL})
	set(CMAKE_CXX_FLAGS_PROFILE ${CMAKE_C_FLAGS_PROFILE})

	add_compile_options(-fno-strict-aliasing)

	if(NOT AROS)
		CHECK_CXX_COMPILER_FLAG("-fvisibility=hidden" cxx_has_fvisibility)
		if(NOT cxx_has_fvisibility)
			message(FATAL_ERROR "Compiler does not support -fvisibility")
		endif()
		add_compile_options(-fvisibility=hidden)
	endif()

	# TODO fix these warnings
	add_compile_options(-Wno-sign-compare)
	add_compile_options(-Wno-switch)
	add_compile_options(-Wno-format-security)

	CHECK_CXX_COMPILER_FLAG("-Woverloaded-virtual" cxx_has_Woverload_virtual)
	if(cxx_has_Woverload_virtual)
		add_compile_options(-Woverloaded-virtual)
	endif()

	if(os STREQUAL "linux")
		set(sys_libs ${sys_libs} dl)
	endif()
elseif(MSVC)
	add_compile_options(/W4)
	add_compile_options(/wd4100) # unreferenced formal parameter
	add_compile_options(/wd4127) # conditional expression is constant
	add_compile_options(/wd4244) # possible loss of data
	add_compile_options(/wd4245) # signed/unsigned mismatch
	add_compile_options(/wd4267) # possible loss of data
	add_compile_options(/wd4714) # 'function' marked as __forceinline not inlined
	add_compile_options(/wd4996) # 'function': was declared deprecated
	add_compile_options(/wd4068) # unknown pragma
	add_compile_options(/wd4458) # declaration of 'variable' hides class member
	add_definitions(-D_ALLOW_KEYWORD_MACROS) # because of the "#define private public" and "#define protected public" in TypeInfo.cpp
	set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG /Od /Zi /MDd")
	set(CMAKE_C_FLAGS_RELEASE "/Ox /Oy /MD")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "/Ox /Oy /Zi /MD")
	set(CMAKE_C_FLAGS_MINSIZEREL "/Ox /Oy /Os /MD")
else()
	message(FATAL_ERROR "Unsupported compiler")
endif()

set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})
# disable assertions in release builds
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG ${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-DNDEBUG ${CMAKE_C_FLAGS_MINSIZEREL}")

# mingw and msvc

# fallback for cmake versions without GNUInstallDirs
if(GNUINSTALLDIRS MATCHES "NOTFOUND")
	set(CMAKE_INSTALL_BINDIR		"bin"
		CACHE PATH "user executables (bin)")
	set(CMAKE_INSTALL_LIBDIR		"lib${LIB_SUFFIX}"
		CACHE PATH "object code libraries (lib${LIB_SUFFIX})")
	set(CMAKE_INSTALL_DATAROOTDIR	"share"
		CACHE PATH "read-only architecture-independent data root (share)")
	set(CMAKE_INSTALL_DATADIR		"${CMAKE_INSTALL_DATAROOTDIR}"
		CACHE PATH "read-only architecture-independent data (DATAROOTDIR)")

	mark_as_advanced(CMAKE_INSTALL_BINDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_DATAROOTDIR CMAKE_INSTALL_DATADIR)
	foreach(dir BINDIR LIBDIR DATAROOTDIR DATADIR)
		if(NOT IS_ABSOLUTE ${CMAKE_INSTALL_${dir}})
			set(CMAKE_INSTALL_FULL_${dir} "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_${dir}}")
		else()
			set(CMAKE_INSTALL_FULL_${dir} "${CMAKE_INSTALL_${dir}}")
		endif()
	endforeach()
endif()

set(bindir		"${CMAKE_INSTALL_FULL_BINDIR}")
set(libdir		"${CMAKE_INSTALL_FULL_LIBDIR}/IcedTech")
set(datadir		"${CMAKE_INSTALL_FULL_DATADIR}/IcedTech")

configure_file(
	"${CMAKE_SOURCE_DIR}/config.h.in"
	"${CMAKE_BINARY_DIR}/config.h"
)

message(STATUS "Building ${CMAKE_BUILD_TYPE} for ${os}-${cpu}")

if(NOT APPLE AND NOT WIN32)
	message(STATUS "The install target will use the following directories:")
	message(STATUS "  Binary directory:  ${bindir}")
	message(STATUS "  Library directory: ${libdir}")
	message(STATUS "  Data directory:    ${datadir}")
endif()


set(src_renderer
code/renderer/qgllib/glew.c
code/renderer/qgllib/glew.h
code/renderer/qgllib/qgllib.h
code/renderer/qgllib/wglew.h
code/renderer/tr_turboshadow.cpp
code/renderer/tr_stencilshadow.cpp
code/renderer/VirtualTextureSystem.cpp
code/renderer/VirtualTextureFeedbackJob.cpp
code/renderer/MegaTextureFile.cpp
code/renderer/Model_sprite.cpp
code/renderer/BinaryImage.cpp
code/renderer/BufferObject.cpp
code/renderer/Model_md5.cpp
code/renderer/RenderWorld_load.cpp
code/renderer/Model_prt.cpp
code/renderer/tr_local.h
code/renderer/RenderWorld_local.h
code/renderer/tr_subview.cpp
code/renderer/RenderTexture.h
code/renderer/Model_lwo.cpp
code/renderer/RenderProgs_GLSL.cpp
code/renderer/tr_main.cpp
code/renderer/draw_interaction.cpp
code/renderer/Interaction.cpp
code/renderer/ModelOverlay.cpp
code/renderer/tr_orderIndexes.cpp
code/renderer/wglext.h
code/renderer/Model.h
code/renderer/GLMatrix.cpp
code/renderer/VirtualTextureTranscode.cpp
code/renderer/ModelDecal.h
code/renderer/RenderProgs.h
code/renderer/Cinematic.cpp
code/renderer/tr_lightrun.cpp
code/renderer/draw_occlusion.cpp
code/renderer/MegaTextureBuild.cpp
code/renderer/tr_rendertools.cpp
code/renderer/tr_trisurf.cpp
code/renderer/tr_deform.cpp
code/renderer/RenderWorld.h
code/renderer/Model_md3.cpp
code/renderer/RenderSystem.cpp
code/renderer/draw_feedback.cpp
code/renderer/ImageOpts.h
code/renderer/tr_light.cpp
code/renderer/BufferObject.h
code/renderer/Model_ase.cpp
code/renderer/Color/ColorSpace.h
code/renderer/Color/ColorSpace.cpp
code/renderer/tr_polytope.cpp
code/renderer/RenderEntity.cpp
code/renderer/tr_render.cpp
code/renderer/RenderWorld_demo.cpp
code/renderer/Material.h
code/renderer/MegaTexture.cpp
code/renderer/DXT/DXTDecoder.cpp
code/renderer/DXT/DXTCodec.h
code/renderer/DXT/DXTEncoder.cpp
code/renderer/ModelManager.h
code/renderer/Model_md3.h
code/renderer/Model_obj.cpp
code/renderer/Model_local.h
code/renderer/RenderSystem_init.cpp
code/renderer/RenderTexture.cpp
code/renderer/ModelDecal.cpp
code/renderer/simplex.h
code/renderer/draw_common.cpp
code/renderer/jpeg-6/jconfig.h
code/renderer/jpeg-6/jdmerge.c
code/renderer/jpeg-6/jmorecfg.h
code/renderer/jpeg-6/jfdctfst.c
code/renderer/jpeg-6/jdapimin.c
code/renderer/jpeg-6/jfdctflt.c
code/renderer/jpeg-6/jversion.h
code/renderer/jpeg-6/jidctfst.c
code/renderer/jpeg-6/jpeglib.h
code/renderer/jpeg-6/jchuff.c
code/renderer/jpeg-6/jcsample.c
code/renderer/jpeg-6/jcphuff.c
code/renderer/jpeg-6/jdpostct.c
code/renderer/jpeg-6/jdatadst.c
code/renderer/jpeg-6/jcomapi.c
code/renderer/jpeg-6/jdhuff.h
code/renderer/jpeg-6/jinclude.h
code/renderer/jpeg-6/jcmainct.c
code/renderer/jpeg-6/jdct.h
code/renderer/jpeg-6/jdapistd.c
code/renderer/jpeg-6/jcapimin.c
code/renderer/jpeg-6/jmemansi.c
code/renderer/jpeg-6/jfdctint.c
code/renderer/jpeg-6/jquant1.c
code/renderer/jpeg-6/jutils.c
code/renderer/jpeg-6/jpegtran.c
code/renderer/jpeg-6/jdmarker.c
code/renderer/jpeg-6/jcmaster.c
code/renderer/jpeg-6/jcdctmgr.c
code/renderer/jpeg-6/jmemname.c
code/renderer/jpeg-6/jmemdos.c
code/renderer/jpeg-6/jdphuff.c
code/renderer/jpeg-6/jcparam.c
code/renderer/jpeg-6/jdmaster.c
code/renderer/jpeg-6/jcapistd.c
code/renderer/jpeg-6/jchuff.h
code/renderer/jpeg-6/jdsample.c
code/renderer/jpeg-6/jmemmgr.c
code/renderer/jpeg-6/jdtrans.c
code/renderer/jpeg-6/jdcolor.c
code/renderer/jpeg-6/jdinput.c
code/renderer/jpeg-6/jidctint.c
code/renderer/jpeg-6/jdcoefct.c
code/renderer/jpeg-6/jquant2.c
code/renderer/jpeg-6/jccoefct.c
code/renderer/jpeg-6/jidctred.c
code/renderer/jpeg-6/jddctmgr.c
code/renderer/jpeg-6/jmemsys.h
code/renderer/jpeg-6/jccolor.c
code/renderer/jpeg-6/jpegint.h
code/renderer/jpeg-6/jdmainct.c
code/renderer/jpeg-6/jcprepct.c
code/renderer/jpeg-6/jcinit.c
code/renderer/jpeg-6/jdhuff.c
code/renderer/jpeg-6/jerror.h
code/renderer/jpeg-6/jerror.c
code/renderer/jpeg-6/jcmarker.c
code/renderer/jpeg-6/jctrans.c
code/renderer/jpeg-6/jdatasrc.c
code/renderer/jpeg-6/jload.c
code/renderer/jpeg-6/jmemnobs.c
code/renderer/jpeg-6/jidctflt.c
code/renderer/RenderMatrix.h
code/renderer/GraphicsContext.h
code/renderer/Model_lwo.h
code/renderer/Cinematic.h
code/renderer/Image_intrinsic.cpp
code/renderer/BinaryImageData.h
code/renderer/Material.cpp
code/renderer/Image_GL.cpp
code/renderer/ImageManager.cpp
code/renderer/tr_guisurf.cpp
code/renderer/tr_shadowbounds.cpp
code/renderer/tr_font.cpp
code/renderer/Image_process.cpp
code/renderer/MegaTexture.h
code/renderer/Model_ma.h
code/renderer/RenderSystem.h
code/renderer/RenderWorld.cpp
code/renderer/RenderProgs.cpp
code/renderer/tr_backend.cpp
code/renderer/Model_ase.h
code/renderer/ModelManager.cpp
code/renderer/tr_trace.cpp
code/renderer/GLMatrix.h
code/renderer/VirtualTexture.h
code/renderer/Interaction.h
code/renderer/Model_ma.cpp
code/renderer/VertexCache.h
code/renderer/Model_ColladaHelper.h
code/renderer/GuiModel.h
code/renderer/Image_load.cpp
code/renderer/GuiModel.cpp
code/renderer/Model_liquid.cpp
code/renderer/BinaryImage.h
code/renderer/RenderMatrix.cpp
code/renderer/Model_beam.cpp
code/renderer/Image.h
code/renderer/Model_ColladaParser.h
code/renderer/Image_files.cpp
code/renderer/Model_terrain.cpp
code/renderer/ModelOverlay.h
code/renderer/RenderWorld_portals.cpp
code/renderer/VertexCache.cpp
code/renderer/Image_program.cpp
code/renderer/Model.cpp
code/renderer/Model_ColladaParser.cpp
)
set(src_idlib

code/idlib/Lib.h
code/idlib/BitMsg.cpp
code/idlib/MapFile.cpp
code/idlib/containers
code/idlib/containers/PlaneSet.h
code/idlib/containers/HashIndex.h
code/idlib/containers/List.h
code/idlib/containers/StrList.h
code/idlib/containers/ListSTL.h
code/idlib/containers/HashIndex.cpp
code/idlib/containers/Stack.h
code/idlib/containers/BTree.h
code/idlib/containers/Hierarchy.h
code/idlib/containers/Array.h
code/idlib/containers/StrPool.h
code/idlib/containers/BinSearch.h
code/idlib/containers/Sort.h
code/idlib/containers/LinkList.h
code/idlib/containers/Queue.h
code/idlib/containers/StaticList.h
code/idlib/containers/VectorSet.h
code/idlib/containers/HashTable.h
code/idlib/Parser.cpp
code/idlib/Timer.h
code/idlib/precompiled.h
code/idlib/bv
code/idlib/bv/Bounds.cpp
code/idlib/bv/Sphere.cpp
code/idlib/bv/Box.cpp
code/idlib/bv/Sphere.h
code/idlib/bv/Bounds.h
code/idlib/bv/Frustum_gcc.cpp
code/idlib/bv/Frustum.h
code/idlib/bv/Frustum.cpp
code/idlib/bv/Box.h
code/idlib/Token.cpp
code/idlib/Parser.h
code/idlib/Heap.cpp
code/idlib/Swap.h
code/idlib/Str.h
code/idlib/Lexer.cpp
code/idlib/CmdArgs.cpp
code/idlib/Base64.cpp
code/idlib/Token.h
code/idlib/MapFile.h
code/idlib/precompiled.cpp
code/idlib/Dict.cpp
code/idlib/hashing
code/idlib/hashing/MD4.cpp
code/idlib/hashing/CRC32.cpp
code/idlib/hashing/MD5.cpp
code/idlib/hashing/MD4.h
code/idlib/hashing/MD5.h
code/idlib/hashing/CRC32.h
code/idlib/Timer.cpp
code/idlib/LangDict.h
code/idlib/CmdArgs.h
code/idlib/geometry
code/idlib/geometry/Surface_SweptSpline.cpp
code/idlib/geometry/TraceModel.cpp
code/idlib/geometry/TraceModel.h
code/idlib/geometry/Winding2D.cpp
code/idlib/geometry/Winding.cpp
code/idlib/geometry/Surface.cpp
code/idlib/geometry/Surface_Polytope.cpp
code/idlib/geometry/JointTransform.cpp
code/idlib/geometry/Surface_Patch.h
code/idlib/geometry/DrawVert.cpp
code/idlib/geometry/Surface_Patch.cpp
code/idlib/geometry/Surface_Polytope.h
code/idlib/geometry/DrawVert.h
code/idlib/geometry/Surface_SweptSpline.h
code/idlib/geometry/JointTransform.h
code/idlib/geometry/Winding2D.h
code/idlib/geometry/Surface.h
code/idlib/geometry/Winding.h
code/idlib/Lexer.h
code/idlib/Heap.h
code/idlib/math
code/idlib/math/Simd.h
code/idlib/math/Interpolate.h
code/idlib/math/Ode.cpp
code/idlib/math/Polynomial.h
code/idlib/math/Simd.cpp
code/idlib/math/Complex.h
code/idlib/math/Angles.h
code/idlib/math/Angles.cpp
code/idlib/math/Simd_SSE.h
code/idlib/math/Plane.h
code/idlib/math/Matrix.cpp
code/idlib/math/Simd_MMX.h
code/idlib/math/Pluecker.h
code/idlib/math/Quat.h
code/idlib/math/Float16.h
code/idlib/math/Simd_AltiVec.h
code/idlib/math/Simd_3DNow.cpp
code/idlib/math/Simd_SSE3.h
code/idlib/math/Simd_MMX.cpp
code/idlib/math/Random.h
code/idlib/math/Extrapolate.h
code/idlib/math/Simd_3DNow.h
code/idlib/math/Simd_SSE3.cpp
code/idlib/math/Matrix.h
code/idlib/math/Rotation.h
code/idlib/math/Math.cpp
code/idlib/math/Lcp.cpp
code/idlib/math/Vector.h
code/idlib/math/Complex.cpp
code/idlib/math/Quat.cpp
code/idlib/math/Polynomial.cpp
code/idlib/math/Curve.h
code/idlib/math/Simd_Generic.h
code/idlib/math/Ode.h
code/idlib/math/Lcp.h
code/idlib/math/Simd_SSE2.h
code/idlib/math/Math.h
code/idlib/math/Simd_SSE.cpp
code/idlib/math/Plane.cpp
code/idlib/math/Simd_Generic.cpp
code/idlib/math/Simd_SSE2.cpp
code/idlib/math/Simd_AltiVec.cpp
code/idlib/math/Vector.cpp
code/idlib/math/Rotation.cpp
code/idlib/math/Pluecker.cpp
code/idlib/LangDict.cpp
code/idlib/BitMsg.h
code/idlib/Lib.cpp
code/idlib/Str.cpp
code/idlib/Base64.h
code/idlib/Dict.h
code/idlib.vcxproj
code/idlib.vcxproj.filters


)
set(src_framework
code/framework/DeclPDA.h
code/framework/DeclParticle.cpp
code/framework/UsercmdGen.h
code/framework/ConsoleHistory.h
code/framework/Compressor.h
code/framework/Compressor.cpp
code/framework/DeclFX.cpp
code/framework/KeyInput.cpp
code/framework/File.h
code/framework/Session_local.h
code/framework/DeclAF.h
code/framework/FileSystem.h
code/framework/BuildVersion.h
code/framework/async/AsyncClient.cpp
code/framework/async/AsyncClient.h
code/framework/async/MsgChannel.h
code/framework/async/ServerScan.h
code/framework/async/AsyncServer.h
code/framework/async/NetworkSystem.h
code/framework/async/MsgChannel.cpp
code/framework/async/AsyncServer.cpp
code/framework/async/ServerScan.cpp
code/framework/async/AsyncNetwork.h
code/framework/async/AsyncNetwork.cpp
code/framework/async/NetworkSystem.cpp
code/framework/EditField.cpp
code/framework/Console.cpp
code/framework/CVarSystem.cpp
code/framework/DeclManager.cpp
code/framework/DeclTable.h
code/framework/DemoFile.h
code/framework/EventLoop.cpp
code/framework/Unzip.h
code/framework/DeclSkin.h
code/framework/DemoFile.cpp
code/framework/DeclFX.h
code/framework/Console.h
code/framework/DeclSkin.cpp
code/framework/BuildDefines.h
code/framework/UsercmdGen.cpp
code/framework/Session_menu.cpp
code/framework/DeclManager.h
code/framework/Licensee.h
code/framework/DeclPDA.cpp
code/framework/DemoChecksum.h
code/framework/Common.cpp
code/framework/Session.h
code/framework/DeclParticle.h
code/framework/CVarSystem.h
code/framework/CmdSystem.h
code/framework/Thread.cpp
code/framework/EventLoop.h
code/framework/EditField.h
code/framework/File.cpp
code/framework/DeclEntityDef.cpp
code/framework/Unzip.cpp
code/framework/DeclEntityDef.h
code/framework/Common.h
code/framework/KeyInput.h
code/framework/Session.cpp
code/framework/DeclTable.cpp
code/framework/CmdSystem.cpp
code/framework/FileSystem.cpp
code/framework/Thread.h
code/framework/ConsoleHistory.cpp
code/framework/DeclAF.cpp
)
set(src_cm
code/cm/CollisionModel_trace.cpp
code/cm/CollisionModel.h
code/cm/CollisionModel_rotate.cpp
code/cm/CollisionModel_contents.cpp
code/cm/CollisionModel_debug.cpp
code/cm/CollisionModel_files.cpp
code/cm/CollisionModel_local.h
code/cm/CollisionModel_contacts.cpp
code/cm/CollisionModel_translate.cpp
code/cm/CollisionModel_load.cpp
)
set(src_dmap
code/tools/compilers/dmap/dmap.cpp
code/tools/compilers/dmap/gldraw.cpp
code/tools/compilers/dmap/usurface.cpp
code/tools/compilers/dmap/optimize_gcc.cpp
code/tools/compilers/dmap/tritools.cpp
code/tools/compilers/dmap/ubrush.cpp
code/tools/compilers/dmap/optimize.cpp
code/tools/compilers/dmap/portals.cpp
code/tools/compilers/dmap/leakfile.cpp
code/tools/compilers/dmap/map.cpp
code/tools/compilers/dmap/glfile.cpp
code/tools/compilers/dmap/output.cpp
code/tools/compilers/dmap/facebsp.cpp
code/tools/compilers/dmap/dmap.h
code/tools/compilers/dmap/shadowopt3.cpp
code/tools/compilers/dmap/tritjunction.cpp
)
set(src_aas
code/tools/compilers/aas/AASCluster.h
code/tools/compilers/aas/AASBuild.cpp
code/tools/compilers/aas/AASBuild_file.cpp
code/tools/compilers/aas/AASFile_optimize.cpp
code/tools/compilers/aas/AASBuild_gravity.cpp
code/tools/compilers/aas/AASBuild_local.h
code/tools/compilers/aas/AASFile_local.h
code/tools/compilers/aas/AASReach.cpp
code/tools/compilers/aas/Brush.cpp
code/tools/compilers/aas/AASBuild_ledge.cpp
code/tools/compilers/aas/AASCluster.cpp
code/tools/compilers/aas/Brush.h
code/tools/compilers/aas/AASFileManager.cpp
code/tools/compilers/aas/BrushBSP.h
code/tools/compilers/aas/AASFile.h
code/tools/compilers/aas/BrushBSP.cpp
code/tools/compilers/aas/AASFileManager.h
code/tools/compilers/aas/AASBuild_merge.cpp
code/tools/compilers/aas/AASReach.h
code/tools/compilers/aas/AASFile_sample.cpp
code/tools/compilers/aas/AASFile.cpp
)
set(src_roq
code/tools/compilers/roqvq/roqParam.cpp
code/tools/compilers/roqvq/gdefs.h
code/tools/compilers/roqvq/roq.m
code/tools/compilers/roqvq/roqParam.h
code/tools/compilers/roqvq/roq.cpp
code/tools/compilers/roqvq/NSBitmapImageRep.cpp
code/tools/compilers/roqvq/quaddefs.h
code/tools/compilers/roqvq/codec.h
code/tools/compilers/roqvq/codec.cpp
code/tools/compilers/roqvq/roq.h
)
set(src_renderbump
code/tools/compilers/renderbump/renderbump.cpp
)
set(src_snd
code/sound/snd_local.h
code/sound/snd_shader.cpp
code/sound/snd_world.cpp
code/sound/snd_emitter.cpp
code/sound/snd_system.cpp
)
set(src_ui
code/ui/GameWindow.cpp
code/ui/UserInterface.cpp
code/ui/GuiScript.h
code/ui/RegExp.cpp
code/ui/SimpleWindow.h
code/ui/RenderWindow.cpp
code/ui/RegExp_old.h
code/ui/GameBearShootWindow.h
code/ui/ListWindow.h
code/ui/Winvar.h
code/ui/ChoiceWindow.h
code/ui/Winvar.cpp
code/ui/ListWindow.cpp
code/ui/GameBustOutWindow.cpp
code/ui/GameSSDWindow.h
code/ui/GameWindow.h
code/ui/EditWindow.cpp
code/ui/MarkerWindow.h
code/ui/EditWindow.h
code/ui/GameSSDWindow.cpp
code/ui/GuiScript.cpp
code/ui/SliderWindow.cpp
code/ui/UserInterfaceLocal.h
code/ui/RenderWindow.h
code/ui/RegExp.h
code/ui/DeviceContext.cpp
code/ui/MarkerWindow.cpp
code/ui/BindWindow.h
code/ui/ListGUI.h
code/ui/Window.h
code/ui/DeviceContext.h
code/ui/BindWindow.cpp
code/ui/Window.cpp
code/ui/ListGUI.cpp
code/ui/SimpleWindow.cpp
code/ui/GameBustOutWindow.h
code/ui/ListGUILocal.h
code/ui/SliderWindow.h
code/ui/FieldWindow.h
code/ui/UserInterface.h
code/ui/FieldWindow.cpp
code/ui/Rectangle.h
code/ui/ChoiceWindow.cpp
code/ui/GameBearShootWindow.cpp
)

set(src_tools
code/tools/materialeditor/MaterialDef.h
code/tools/materialeditor/ToggleListView.cpp
code/tools/materialeditor/MaterialTreeView.h
code/tools/materialeditor/MaterialDocManager.h
code/tools/materialeditor/MaterialPreviewView.cpp
code/tools/materialeditor/MaterialDef.cpp
code/tools/materialeditor/ToggleListView.h
code/tools/materialeditor/MaterialDoc.cpp
code/tools/materialeditor/MaterialTreeView.cpp
code/tools/materialeditor/MEOptions.cpp
code/tools/materialeditor/MaterialView.h
code/tools/materialeditor/StageView.h
code/tools/materialeditor/MaterialModifier.cpp
code/tools/materialeditor/MaterialPreviewView.h
code/tools/materialeditor/MEOptions.h
code/tools/materialeditor/MaterialPreviewPropView.cpp
code/tools/materialeditor/MaterialEditor.cpp
code/tools/materialeditor/ConsoleView.h
code/tools/materialeditor/StageView.cpp
code/tools/materialeditor/MaterialPropTreeView.cpp
code/tools/materialeditor/MaterialDoc.h
code/tools/materialeditor/MaterialEditor.h
code/tools/materialeditor/MaterialEditView.h
code/tools/materialeditor/FindDialog.h
code/tools/materialeditor/MaterialModifier.h
code/tools/materialeditor/MEMainFrame.cpp
code/tools/materialeditor/MaterialEditView.cpp
code/tools/materialeditor/FindDialog.cpp
code/tools/materialeditor/MaterialDocManager.cpp
code/tools/materialeditor/MaterialPreviewPropView.h
code/tools/materialeditor/ConsoleView.cpp
code/tools/materialeditor/MaterialPropTreeView.h
code/tools/materialeditor/MaterialView.cpp
code/tools/materialeditor/MEMainFrame.h
code/tools/particle/DialogParticleEditor.h
code/tools/particle/DialogParticleEditor.cpp
code/tools/af/DialogAFConstraintSlider.h
code/tools/af/DialogAFConstraintFixed.h
code/tools/af/DialogAFConstraintBallAndSocket.h
code/tools/af/DialogAFProperties.cpp
code/tools/af/DialogAFConstraint.cpp
code/tools/af/DialogAFBody.h
code/tools/af/DialogAFConstraintUniversal.cpp
code/tools/af/DialogAFName.cpp
code/tools/af/DialogAFName.h
code/tools/af/DialogAFConstraint.h
code/tools/af/DialogAFConstraintBallAndSocket.cpp
code/tools/af/DialogAFConstraintSlider.cpp
code/tools/af/DialogAFConstraintUniversal.h
code/tools/af/DialogAF.h
code/tools/af/DialogAFConstraintHinge.h
code/tools/af/DialogAFConstraintSpring.h
code/tools/af/DialogAFConstraintSpring.cpp
code/tools/af/DialogAFBody.cpp
code/tools/af/DialogAFConstraintHinge.cpp
code/tools/af/DialogAF.cpp
code/tools/af/DialogAFView.h
code/tools/af/DialogAFConstraintFixed.cpp
code/tools/af/DialogAFView.cpp
code/tools/af/DialogAFProperties.h
code/tools/radiant/EditorMap.h
code/tools/radiant/NewTexWnd.h
code/tools/radiant/WaveOpen.h
code/tools/radiant/LightDlg.cpp
code/tools/radiant/DialogTextures.h
code/tools/radiant/Undo.cpp
code/tools/radiant/SurfaceDlg.h
code/tools/radiant/CommandsDlg.cpp
code/tools/radiant/CapDialog.h
code/tools/radiant/QEDEFS.H
code/tools/radiant/DlgEvent.h
code/tools/radiant/RotateDlg.cpp
code/tools/radiant/CameraTargetDlg.cpp
code/tools/radiant/XYWnd.cpp
code/tools/radiant/CommentsDlg.h
code/tools/radiant/DialogThick.cpp
code/tools/radiant/EditViewDlg.h
code/tools/radiant/PrefsDlg.h
code/tools/radiant/SurfaceDlg.cpp
code/tools/radiant/splines.h
code/tools/radiant/ConsoleDlg.h
code/tools/radiant/EditorBrush.cpp
code/tools/radiant/TextureBar.cpp
code/tools/radiant/DialogThick.h
code/tools/radiant/Z.CPP
code/tools/radiant/GLWidget.cpp
code/tools/radiant/WIN_QE3.CPP
code/tools/radiant/SELECT.CPP
code/tools/radiant/DialogInfo.cpp
code/tools/radiant/GetString.h
code/tools/radiant/ECLASS.CPP
code/tools/radiant/EditorMap.cpp
code/tools/radiant/XYWnd.h
code/tools/radiant/ZClip.cpp
code/tools/radiant/Undo.h
code/tools/radiant/InspectorDialog.cpp
code/tools/radiant/WaitDlg.cpp
code/tools/radiant/RotateDlg.h
code/tools/radiant/CameraTargetDlg.h
code/tools/radiant/SELECT.H
code/tools/radiant/ScaleDialog.cpp
code/tools/radiant/EntityDlg.cpp
code/tools/radiant/LightDlg.h
code/tools/radiant/FindTextureDlg.cpp
code/tools/radiant/MRU.CPP
code/tools/radiant/EditorEntity.cpp
code/tools/radiant/EditViewDlg.cpp
code/tools/radiant/EditorBrushPrimit.cpp
code/tools/radiant/VERTSEL.CPP
code/tools/radiant/MainFrm.cpp
code/tools/radiant/DlgEvent.cpp
code/tools/radiant/PatchDialog.cpp
code/tools/radiant/FindTextureDlg.h
code/tools/radiant/WaveOpen.cpp
code/tools/radiant/GLWidget.h
code/tools/radiant/PreviewDlg.cpp
code/tools/radiant/MapInfo.h
code/tools/radiant/TabsDlg.cpp
code/tools/radiant/CurveDlg.h
code/tools/radiant/PatchDensityDlg.h
code/tools/radiant/MapInfo.cpp
code/tools/radiant/MediaPreviewDlg.h
code/tools/radiant/EntKeyFindReplace.h
code/tools/radiant/DRAG.CPP
code/tools/radiant/TextureBar.h
code/tools/radiant/CommandsDlg.h
code/tools/radiant/MediaPreviewDlg.cpp
code/tools/radiant/EntityDlg.h
code/tools/radiant/autocaulk.cpp
code/tools/radiant/DlgCamera.h
code/tools/radiant/PARSE.CPP
code/tools/radiant/ConsoleDlg.cpp
code/tools/radiant/ScaleDialog.h
code/tools/radiant/NewProjDlg.cpp
code/tools/radiant/PMESH.CPP
code/tools/radiant/InspectorDialog.h
code/tools/radiant/CamWnd.h
code/tools/radiant/autocaulk.h
code/tools/radiant/MainFrm.h
code/tools/radiant/splines.cpp
code/tools/radiant/NewTexWnd.cpp
code/tools/radiant/CurveDlg.cpp
code/tools/radiant/DlgCamera.cpp
code/tools/radiant/DialogTextures.cpp
code/tools/radiant/EditorEntity.h
code/tools/radiant/NewProjDlg.h
code/tools/radiant/CSG.CPP
code/tools/radiant/PropertyList.cpp
code/tools/radiant/PatchDensityDlg.cpp
code/tools/radiant/WIN_DLG.CPP
code/tools/radiant/DialogInfo.h
code/tools/radiant/Radiant.h
code/tools/radiant/Textures.h
code/tools/radiant/TabsDlg.h
code/tools/radiant/PrefsDlg.cpp
code/tools/radiant/cmdlib.h
code/tools/radiant/MRU.H
code/tools/radiant/TearoffContainerWindow.cpp
code/tools/radiant/GetString.cpp
code/tools/radiant/PreviewDlg.h
code/tools/radiant/ZClip.h
code/tools/radiant/PARSE.H
code/tools/radiant/QE3.H
code/tools/radiant/Z.H
code/tools/radiant/PMESH.H
code/tools/radiant/CamWnd.cpp
code/tools/radiant/WIN_QE3.RC2
code/tools/radiant/CapDialog.cpp
code/tools/radiant/PropertyList.h
code/tools/radiant/QE3.CPP
code/tools/radiant/EditorBrush.h
code/tools/radiant/EntKeyFindReplace.cpp
code/tools/radiant/PatchDialog.h
code/tools/radiant/CommentsDlg.cpp
code/tools/radiant/PointFile.cpp
code/tools/radiant/QERTYPES.H
code/tools/radiant/Radiant.cpp
code/tools/radiant/TearoffContainerWindow.h
code/tools/radiant/WaitDlg.h
code/tools/radiant/cmdlib.cpp
code/tools/compilers
code/tools/compilers/compiler_public.h
code/tools/compilers/megagen
code/tools/compilers/megagen/MegaProject.cpp
code/tools/compilers/megagen/MegaGen.cpp
code/tools/compilers/megagen/MegaGen.h
code/tools/compilers/dmap
code/tools/compilers/dmap/dmap.cpp
code/tools/compilers/dmap/gldraw.cpp
code/tools/compilers/dmap/usurface.cpp
code/tools/compilers/dmap/optimize_gcc.cpp
code/tools/compilers/dmap/tritools.cpp
code/tools/compilers/dmap/ubrush.cpp
code/tools/compilers/dmap/optimize.cpp
code/tools/compilers/dmap/portals.cpp
code/tools/compilers/dmap/leakfile.cpp
code/tools/compilers/dmap/map.cpp
code/tools/compilers/dmap/glfile.cpp
code/tools/compilers/dmap/output.cpp
code/tools/compilers/dmap/facebsp.cpp
code/tools/compilers/dmap/dmap.h
code/tools/compilers/dmap/shadowopt3.cpp
code/tools/compilers/dmap/tritjunction.cpp
code/tools/compilers/roqvq/roqParam.cpp
code/tools/compilers/roqvq/gdefs.h
code/tools/compilers/roqvq/roq.m
code/tools/compilers/roqvq/roqParam.h
code/tools/compilers/roqvq/roq.cpp
code/tools/compilers/roqvq/NSBitmapImageRep.cpp
code/tools/compilers/roqvq/quaddefs.h
code/tools/compilers/roqvq/codec.h
code/tools/compilers/roqvq/codec.cpp
code/tools/compilers/roqvq/roq.h
code/tools/compilers/aas/AASCluster.h
code/tools/compilers/aas/AASBuild.cpp
code/tools/compilers/aas/AASBuild_file.cpp
code/tools/compilers/aas/AASFile_optimize.cpp
code/tools/compilers/aas/AASBuild_gravity.cpp
code/tools/compilers/aas/AASBuild_local.h
code/tools/compilers/aas/AASFile_local.h
code/tools/compilers/aas/AASReach.cpp
code/tools/compilers/aas/Brush.cpp
code/tools/compilers/aas/AASBuild_ledge.cpp
code/tools/compilers/aas/AASCluster.cpp
code/tools/compilers/aas/Brush.h
code/tools/compilers/aas/AASFileManager.cpp
code/tools/compilers/aas/BrushBSP.h
code/tools/compilers/aas/AASFile.h
code/tools/compilers/aas/BrushBSP.cpp
code/tools/compilers/aas/AASFileManager.h
code/tools/compilers/aas/AASBuild_merge.cpp
code/tools/compilers/aas/AASReach.h
code/tools/compilers/aas/AASFile_sample.cpp
code/tools/compilers/aas/AASFile.cpp
code/tools/compilers/renderbump/renderbump.cpp
code/tools/compilers/megalight/MegaLight.cpp
code/tools/compilers/megalight/MegaLight_file.h
code/tools/decl/DialogEntityDefEditor.cpp
code/tools/decl/DialogDeclNew.cpp
code/tools/decl/DialogDeclNew.h
code/tools/decl/DialogDeclEditor.cpp
code/tools/decl/DialogEntityDefEditor.h
code/tools/decl/DialogDeclBrowser.h
code/tools/decl/DialogDeclEditor.h
code/tools/decl/DialogDeclBrowser.cpp
code/tools/edit_stub.cpp
code/tools/pda/DialogPDAEditor.h
code/tools/pda/DialogPDAEditor.cpp
code/tools/debugger/DebuggerBreakpoint.cpp
code/tools/debugger/DebuggerQuickWatchDlg.cpp
code/tools/debugger/debugger.cpp
code/tools/debugger/DebuggerServer.cpp
code/tools/debugger/DebuggerBreakpoint.h
code/tools/debugger/DebuggerWindow.cpp
code/tools/debugger/DebuggerFindDlg.h
code/tools/debugger/DebuggerClient.h
code/tools/debugger/DebuggerQuickWatchDlg.h
code/tools/debugger/DebuggerScript.h
code/tools/debugger/DebuggerMessages.h
code/tools/debugger/DebuggerServer.h
code/tools/debugger/DebuggerScript.cpp
code/tools/debugger/DebuggerClient.cpp
code/tools/debugger/DebuggerApp.h
code/tools/debugger/DebuggerWindow.h
code/tools/debugger/DebuggerApp.cpp
code/tools/debugger/DebuggerFindDlg.cpp
code/tools/edit_public.h
code/tools/common/SpinButton.h
code/tools/common/PropertyGrid.h
code/tools/common/RegistryOptions.cpp
code/tools/common/PropertyGrid.cpp
code/tools/common/ColorButton.cpp
code/tools/common/RenderBumpFlatDialog.h
code/tools/common/RegistryOptions.h
code/tools/common/ColorButton.h
code/tools/common/MaskEdit.cpp
code/tools/common/PropTree
code/tools/common/PropTree/PropTreeView.cpp
code/tools/common/PropTree/PropTreeInfo.cpp
code/tools/common/PropTree/PropTreeItemColor.cpp
code/tools/common/PropTree/PropTreeItemEditButton.cpp
code/tools/common/PropTree/PropTreeItemFileEdit.h
code/tools/common/PropTree/PropTreeItemButton.cpp
code/tools/common/PropTree/PropTreeItemColor.h
code/tools/common/PropTree/PropTreeItemEdit.h
code/tools/common/PropTree/PropTreeItem.cpp
code/tools/common/PropTree/PropTreeView.h
code/tools/common/PropTree/PropTreeItemCombo.h
code/tools/common/PropTree/PropTree.h
code/tools/common/PropTree/PropTreeItemEditButton.h
code/tools/common/PropTree/PropTreeInfo.h
code/tools/common/PropTree/PropTreeItemCheck.cpp
code/tools/common/PropTree/PropTreeList.cpp
code/tools/common/PropTree/PropTreeItemStatic.h
code/tools/common/PropTree/PropTreeList.h
code/tools/common/PropTree/PropTreeItemEdit.cpp
code/tools/common/PropTree/PropTreeItemFileEdit.cpp
code/tools/common/PropTree/PropTreeItem.h
code/tools/common/PropTree/PropTreeItemButton.h
code/tools/common/PropTree/PropTreeItemCombo.cpp
code/tools/common/PropTree/PropTreeItemCheck.h
code/tools/common/PropTree/PropTreeItemStatic.cpp
code/tools/common/PropTree/PropTree.cpp
code/tools/common/AlphaPopup.cpp
code/tools/common/OpenFileDialog.h
code/tools/common/RollupPanel.cpp
code/tools/common/OpenFileDialog.cpp
code/tools/common/DialogHelpers.h
code/tools/common/RenderBumpFlatDialog.cpp
code/tools/common/MaskEdit.h
code/tools/common/RollupPanel.h
code/tools/common/SpinButton.cpp
code/tools/guied
code/tools/guied/GEZOrderModifier.cpp
code/tools/guied/GEModifier.cpp
code/tools/guied/GENavigator.h
code/tools/guied/GEKeyValueModifier.cpp
code/tools/guied/GEProperties.cpp
code/tools/guied/GEOptionsDlg.h
code/tools/guied/GETransformer.h
code/tools/guied/GEZOrderModifier.h
code/tools/guied/GECheckInDlg.cpp
code/tools/guied/GEModifierStack.cpp
code/tools/guied/GEItemScriptsDlg.cpp
code/tools/guied/GEHideModifier.h
code/tools/guied/GEModifier.h
code/tools/guied/GENavigator.cpp
code/tools/guied/GEWorkspace.cpp
code/tools/guied/GEProperties.h
code/tools/guied/GEItemPropsDlg.h
code/tools/guied/GEMoveModifier.h
code/tools/guied/GEDeleteModifier.h
code/tools/guied/GEModifierStack.h
code/tools/guied/GEViewer.h
code/tools/guied/GEDeleteModifier.cpp
code/tools/guied/GESizeModifier.h
code/tools/guied/GEMoveModifier.cpp
code/tools/guied/GESelectionMgr.cpp
code/tools/guied/GEStatusBar.cpp
code/tools/guied/GEWorkspace.h
code/tools/guied/GEHideModifier.cpp
code/tools/guied/GEStateModifier.h
code/tools/guied/GEModifierGroup.cpp
code/tools/guied/GEItemScriptsDlg.h
code/tools/guied/GEOptions.h
code/tools/guied/GEInsertModifier.h
code/tools/guied/GEItemPropsDlg.cpp
code/tools/guied/GEInsertModifier.cpp
code/tools/guied/GEApp.cpp
code/tools/guied/GEApp.h
code/tools/guied/GEPropertyPage.h
code/tools/guied/guied.cpp
code/tools/guied/GEWindowWrapper_stub.cpp
code/tools/guied/GEViewer.cpp
code/tools/guied/GETransformer.cpp
code/tools/guied/GEWindowWrapper.cpp
code/tools/guied/GEOptions.cpp
code/tools/guied/GEStateModifier.cpp
code/tools/guied/GEModifierGroup.h
code/tools/guied/GEOptionsDlg.cpp
code/tools/guied/GEStatusBar.h
code/tools/guied/GESelectionMgr.h
code/tools/guied/GEKeyValueModifier.h
code/tools/guied/GEPropertyPage.cpp
code/tools/guied/GEWindowWrapper.h
code/tools/guied/GEWorkspaceFile.cpp
code/tools/guied/GESizeModifier.cpp
code/tools/sound
code/tools/sound/DialogSoundGroup.cpp
code/tools/sound/DialogSound.cpp
code/tools/sound/DialogSoundGroup.h
code/tools/sound/DialogSound.h
code/tools/script
code/tools/script/DialogScriptEditor.h
code/tools/script/DialogScriptEditor.cpp
code/tools/Help
code/tools/Help/images
code/tools/Help/images/nextanim.png
code/tools/Help/images/showskel.png
code/tools/Help/images/reloadtex.png
code/tools/Help/images/Play.png
code/tools/Help/images/FirstAnim.png
code/tools/Help/images/resetcam.png
code/tools/Help/images/lastanim.png
code/tools/Help/images/showsurf.png
code/tools/Help/images/PrevAnim.png
code/tools/Help/images/orgoff.png
code/tools/Help/images/stop.png
code/tools/Help/images/diffuse.png
code/tools/Help/images/showtris.png
code/tools/Help/images/OpenMesh.png
code/tools/Help/images/showbones.png
code/tools/Help/images/animloop.png
code/tools/Help/images/specular.png
code/tools/Help/images/shownorms.png
code/tools/Help/images/showoverdraw.png
code/tools/Help/images/OpenAnim.png
code/tools/Help/MT_help.htm
code/tools/Help/default.htm
code/tools/Help/MVH_Misc.htm
code/tools/Help/Tools.hhk
code/tools/Help/MVH_Controls.htm
code/tools/Help/MVH_Mouse.htm
code/tools/Help/MVH_Overview.htm
code/tools/Help/Tools.hhc
code/tools/Help/MVH_Issues.htm
code/tools/Help/Tools.hhp
code/tools/comafx
code/tools/comafx/DialogGoToLine.cpp
code/tools/comafx/VectorCtl.h
code/tools/comafx/VectorCtl.cpp
code/tools/comafx/DialogName.cpp
code/tools/comafx/riched20.tlh
code/tools/comafx/CPathTreeCtrl.cpp
code/tools/comafx/CPathTreeCtrl.h
code/tools/comafx/CDIB.cpp
code/tools/comafx/DialogColorPicker.cpp
code/tools/comafx/CSyntaxRichEditCtrl.cpp
code/tools/comafx/DialogName.h
code/tools/comafx/DialogColorPicker.h
code/tools/comafx/DialogGoToLine.h
code/tools/comafx/StdAfx.h
code/tools/comafx/CSyntaxRichEditCtrl.h
code/tools/comafx/CDIB.h
code/tools/comafx/StdAfx.cpp
)

set(src_core
	${src_renderer}
	${src_framework}
	${src_cm}
	${src_dmap}
	${src_aas}
	${src_roq}
	${src_renderbump}
	${src_snd}
	${src_ui}
	${src_tools}
)
set(src_stub_openal		sys/stub/openal_stub.cpp)
set(src_stub_gl			sys/stub/stub_gl.cpp)

set(src_sys_base
	code/sys/sys_local.cpp
	code/sys/posix/posix_net.cpp
	code/sys/posix/posix_main.cpp
	code/sys/linux/main.cpp
        code/sys/sys_threading.h
        code/sys/sys_intrinsics.h
        code/sys/sys_local.h
        code/sys/sys_public.h
)

set(src_sys_core
	code/sys/linux/glimp.cpp
)
add_library(idlib STATIC ${src_idlib})

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})

if (AROS)
	add_library(dll STATIC ${src_arosdll})
        if(CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
		set(AROS_ARCH "x86")
        else()
		set(AROS_ARCH ${CMAKE_SYSTEM_PROCESSOR})
	endif()
else()
	if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang" AND NOT MINGW)
		set_target_properties(idlib PROPERTIES COMPILE_FLAGS "-fPIC")
	endif()
endif()

if(CORE)
	add_executable(${DOOM3BINARY} WIN32 MACOSX_BUNDLE
		${src_core}
		${src_sys_base}
		${src_sys_core}
	)

	set_target_properties(${DOOM3BINARY} PROPERTIES COMPILE_DEFINITIONS "__DOOM_DLL__")
	set_target_properties(${DOOM3BINARY} PROPERTIES LINK_FLAGS "${ldflags}")
	set_target_properties(${DOOM3BINARY} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/sys/osx/Info.plist)

	target_link_libraries(${DOOM3BINARY}
		idlib
		${OPENAL_LIBRARY}
		${CURL_LIBRARY}
		${JPEG_LIBRARY}
		${ZLIB_LIBRARY}
		${SDLx_LIBRARY}
		${sys_libs}
	)

	if(NOT APPLE AND NOT WIN32)
		install(TARGETS ${DOOM3BINARY}
				RUNTIME DESTINATION "${bindir}"
				LIBRARY DESTINATION "${libdir}"
				ARCHIVE DESTINATION "${libdir}"
		)
	endif()
endif()

if(DEDICATED)
	add_executable(${DOOM3BINARY}ded WIN32 MACOSX_BUNDLE
		${src_core}
		${src_stub_openal}
		${src_stub_gl}
		${src_sys_base}
	)

	set_target_properties(${DOOM3BINARY}ded PROPERTIES COMPILE_DEFINITIONS "ID_DEDICATED;__DOOM_DLL__")
	set_target_properties(${DOOM3BINARY}ded PROPERTIES LINK_FLAGS "${ldflags}")
	target_link_libraries(${DOOM3BINARY}ded
		idlib
		${CURL_LIBRARY}
		${JPEG_LIBRARY}
		${ZLIB_LIBRARY}
		${SDLx_LIBRARY}
		${sys_libs}
	)

	if(NOT APPLE AND NOT WIN32)
		install(TARGETS ${DOOM3BINARY}ded
				RUNTIME DESTINATION "${bindir}"
				LIBRARY DESTINATION "${libdir}"
				ARCHIVE DESTINATION "${libdir}"
		)
	endif()
endif()

if(BASE)
	if (AROS)
		set_target_properties(base PROPERTIES OUTPUT_NAME "base.aros-${AROS_ARCH}")
	endif()
	set_target_properties(base PROPERTIES PREFIX "")
	set_target_properties(base PROPERTIES COMPILE_DEFINITIONS "GAME_DLL")
	set_target_properties(base PROPERTIES COMPILE_FLAGS "-I${CMAKE_SOURCE_DIR}/game")
	set_target_properties(base PROPERTIES LINK_FLAGS "${ldflags}")
	set_target_properties(base PROPERTIES INSTALL_NAME_DIR "@executable_path")
	if (AROS)
		target_link_libraries(base idlib dll)
	else()
		target_link_libraries(base idlib)
	endif()

	if(NOT APPLE AND NOT WIN32)
		install(TARGETS base
				RUNTIME DESTINATION "${bindir}"
				LIBRARY DESTINATION "${libdir}"
				ARCHIVE DESTINATION "${libdir}"
		)
	endif()
endif()
