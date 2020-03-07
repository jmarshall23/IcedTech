// ReflectionProbe.cpp
//

#include "game_precompiled.h"
#include "Game_local.h"

idCVar g_defaultCaptureSize("g_defaultCaptureSize", "256", CVAR_INTEGER, "default capture size for reflection probes");

CLASS_DECLARATION(idEntity, rvmReflectionProbe)
END_CLASS

/*
======================
rvmReflectionProbe::Spawn
======================
*/
void rvmReflectionProbe::Spawn(void) {
	captureSize = spawnArgs.GetInt("captureSize", va("%d", g_defaultCaptureSize.GetInteger()));
}

/*
======================
rvmReflectionProbe::Capture
======================
*/
void rvmReflectionProbe::Capture(void) {
	idTempArray<byte>	captureFrameBuffer(captureSize * captureSize * 4);
	char* extensions[6] = { "_px.tga", "_nx.tga", "_py.tga", "_ny.tga", "_pz.tga", "_nz.tga" };

	for(int side = 0; side < 6; side++) {
		renderView_t renderView;

		renderView.x = 0;
		renderView.y = 0;
		renderView.width = SCREEN_WIDTH;
		renderView.height = SCREEN_HEIGHT;
		renderView.window_width = captureSize;
		renderView.window_height = captureSize;
		renderView.fov_x = 90;
		renderView.fov_y = 2 * atan((float)renderView.window_height / renderView.window_width) * idMath::M_RAD2DEG;
		renderView.vieworg = GetPhysics()->GetOrigin();
		renderView.viewaxis.Zero();
		renderView.skipPostProcess = true;

	
		switch (side) {
		case 0:
			renderView.viewaxis[0][0] = 1;
			renderView.viewaxis[1][2] = 1;
			renderView.viewaxis[2][1] = 1;
			break;
		case 1:
			renderView.viewaxis[0][0] = -1;
			renderView.viewaxis[1][2] = -1;
			renderView.viewaxis[2][1] = 1;
			break;
		case 2:
			renderView.viewaxis[0][1] = 1;
			renderView.viewaxis[1][0] = -1;
			renderView.viewaxis[2][2] = -1;
			break;
		case 3:
			renderView.viewaxis[0][1] = -1;
			renderView.viewaxis[1][0] = -1;
			renderView.viewaxis[2][2] = 1;
			break;
		case 4:
			renderView.viewaxis[0][2] = 1;
			renderView.viewaxis[1][0] = -1;
			renderView.viewaxis[2][1] = 1;
			break;
		case 5:
			renderView.viewaxis[0][2] = -1;
			renderView.viewaxis[1][0] = 1;
			renderView.viewaxis[2][1] = 1;
			break;
		}

	
		// Render this slice of the cubemap.
		renderSystem->BeginFrame(renderView.window_width, renderView.window_height);
			gameLocal.RenderScene(&renderView, gameRenderWorld);
		renderSystem->EndFrame(NULL, NULL);

		// Read back the cubemap image.
		renderSystem->ReadRenderTexture(gameLocal.GetGameRender()->forwardRenderPassResolvedRT, captureFrameBuffer.Ptr());

		// Write out this slice to a targa.
		idStr tgaName = va("%s/%s_reflectionprobe_%s", gameLocal.GetMapFileName(), GetName(), extensions[side]);
		tgaName.Replace(".", "_");
		tgaName += ".tga";
		renderSystem->WriteTGA(tgaName.c_str(), captureFrameBuffer.Ptr(), renderView.window_width, renderView.window_height, false, "");
	}
}