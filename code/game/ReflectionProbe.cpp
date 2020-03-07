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

		idMat4 view;
		view.Zero();
		float* viewaxis = view.ToFloatPtr();

		switch (side) {
		case 0:
			viewaxis[0] = 1;
			viewaxis[9] = 1;
			viewaxis[6] = -1;
			break;
		case 1:
			viewaxis[0] = -1;
			viewaxis[9] = -1;
			viewaxis[6] = -1;
			break;
		case 2:
			viewaxis[4] = 1;
			viewaxis[1] = -1;
			viewaxis[10] = 1;
			break;
		case 3:
			viewaxis[4] = -1;
			viewaxis[1] = -1;
			viewaxis[10] = -1;
			break;
		case 4:
			viewaxis[8] = 1;
			viewaxis[1] = -1;
			viewaxis[6] = -1;
			break;
		case 5:
			viewaxis[8] = -1;
			viewaxis[1] = 1;
			viewaxis[6] = -1;
			break;
		}

		renderView.viewaxis = view.ToMat3();
		
		// Render this slice of the cubemap.
		renderSystem->BeginFrame(renderView.window_width, renderView.window_height);
			gameLocal.RenderScene(&renderView, gameRenderWorld);
		renderSystem->EndFrame(NULL, NULL);

		// Read back the cubemap image.
		renderSystem->ReadRenderTexture(gameLocal.GetGameRender()->forwardRenderPassResolvedRT, captureFrameBuffer.Ptr());

		// Write out this slice to a targa.
		idStr tgaName = va("%s/%s_reflectionprobe_%d", gameLocal.GetMapFileName(), GetName(), side);
		tgaName.Replace(".", "_");
		tgaName += ".tga";
		renderSystem->WriteTGA(tgaName.c_str(), captureFrameBuffer.Ptr(), renderView.window_width, renderView.window_height, false, "");
	}
}