// Game_render.cpp
//

#include "game_precompiled.h"
#include "Game_local.h"

/*
=======================================

Game Render

The engine renderer is designed to do two things, generate the geometry pass, and the shadow passes. The pipeline,
including post process, is now handled in the game code. This allows more granular control over how the final pixels,
are presented on screen based on whatever is going on in game.

=======================================
*/

/*
========================
idGameLocal::InitGameRenderSystem
========================
*/
void idGameLocal::InitGameRenderSystem(void) {
	{
		idImageOpts opts;
		opts.format = FMT_RGBA8;
		opts.colorFormat = CFM_DEFAULT;
		opts.numLevels = 1;
		opts.textureType = TT_2D;
		opts.isPersistant = true;
		opts.width = renderSystem->GetScreenWidth();
		opts.height = renderSystem->GetScreenHeight();
		opts.numMSAASamples = renderSystem->GetNumMSAASamples();

		idImage *albedoImage = renderSystem->CreateImage("_forwardRenderAlbedo", &opts, TF_LINEAR);
		idImage *emissiveImage = renderSystem->CreateImage("_forwardRenderEmissive", &opts, TF_LINEAR);

		opts.numMSAASamples = renderSystem->GetNumMSAASamples();
		opts.format = FMT_DEPTH_STENCIL;
		idImage *depthImage = renderSystem->CreateImage("_forwardRenderDepth", &opts, TF_LINEAR);

		gameRender.forwardRenderPassRT = renderSystem->CreateRenderTexture(albedoImage, depthImage, emissiveImage);
	}

	{
		idImageOpts opts;
		opts.format = FMT_RGBA8;
		opts.colorFormat = CFM_DEFAULT;
		opts.numLevels = 1;
		opts.textureType = TT_2D;
		opts.isPersistant = true;
		opts.width = renderSystem->GetScreenWidth();
		opts.height = renderSystem->GetScreenHeight();
		opts.numMSAASamples = 0;

		idImage *albedoImage = renderSystem->CreateImage("_forwardRenderResolvedAlbedo", &opts, TF_LINEAR);
		idImage *emissiveImage = renderSystem->CreateImage("_forwardRenderResolvedEmissive", &opts, TF_LINEAR);
		opts.format = FMT_DEPTH;
		idImage *depthImage = renderSystem->CreateImage("_forwardRenderResolvedDepth", &opts, TF_LINEAR);

		gameRender.forwardRenderPassResolvedRT = renderSystem->CreateRenderTexture(albedoImage, depthImage, emissiveImage);
	}

	{
		idImageOpts opts;
		opts.colorFormat = CFM_DEFAULT;
		opts.numLevels = 1;
		opts.textureType = TT_2D;
		opts.isPersistant = true;
		opts.width = 160;
		opts.height = 90;
		opts.readback = 1;
		opts.numMSAASamples = 0;

		for (int i = 0; i < 2; i++)
		{
			opts.format = FMT_RGBAF16;
			idImage *albedoImage = renderSystem->CreateImage(va("_feedbackAlbedo%d", i), &opts, TF_NEAREST);
			opts.format = FMT_DEPTH;
			idImage *depthImage = renderSystem->CreateImage(va("_feedbackDepth%d", i), &opts, TF_NEAREST);

			gameRender.feedbackRenderPassRT[i] = renderSystem->CreateRenderTexture(albedoImage, depthImage);
		}
	}

	gameRender.feedbackBufferId = 0;

	gameRender.noPostProcessMaterial = declManager->FindMaterial("postprocess/nopostprocess", false);
}

/*
========================
idGameLocal::ResizeRenderTextures
========================
*/
void idGameLocal::ResizeRenderTextures(int width, int height) {
	// Resize all of the different render textures.
	renderSystem->ResizeRenderTexture(gameRender.forwardRenderPassRT, width, height);
	renderSystem->ResizeRenderTexture(gameRender.forwardRenderPassResolvedRT, width, height);
}

/*
====================
idGameLocal::RenderScene
====================
*/
void idGameLocal::RenderScene(const renderView_t *view, idRenderWorld *renderWorld) {
	// Run the feedback job on last frames data.
	renderSystem->RunFeedbackJob(gameRender.feedbackRenderPassRT[!gameRender.feedbackBufferId]);

	// Minimum render is used for screen captures(such as envcapture) calls, caller is responsible for all rendertarget setup.
	//if (view->minimumRender)
	//{
	//	RenderSky(view);
	//	if (view->cubeMapTargetImage)
	//	{
	//		renderView_t worldRefDef = *view;
	//		worldRefDef.cubeMapClearBuffer = false;
	//		renderWorld->RenderScene(&worldRefDef);
	//	}
	//	else
	//	{
	//		renderWorld->RenderScene(view);
	//	}
	//
	//	return;
	//}

	// Make sure all of our render textures are the right dimensions for this render.
	if (!view->forceScreenSize) {
		if (view->window_width == 0 || view->window_height == 0) {
			common->FatalError("RenderScene: Invalid Window Dimensions!");
		}
		ResizeRenderTextures(view->window_width, view->window_height);
	}
	else {
		ResizeRenderTextures(renderSystem->GetScreenWidth(), renderSystem->GetScreenHeight());
	}

	// Render the scene to the forward render pass rendertexture.
	renderSystem->BindRenderTexture(gameRender.forwardRenderPassRT, gameRender.feedbackRenderPassRT[gameRender.feedbackBufferId]);
	{
		// Clear the depth buffer only.
		//renderSystem->ClearRenderTarget(false, true, 1.0f, 0.0f, 0.0f, 0.0f);
	
		// Render our sky first.
		//RenderSky(view);
	
		// Render the current world.
		renderWorld->RenderScene(view);
	}
	renderSystem->BindRenderTexture(nullptr, nullptr);

	// Resolve our MSAA buffer.
	renderSystem->ResolveMSAA(gameRender.forwardRenderPassRT, gameRender.forwardRenderPassResolvedRT);

	// Render to the back buffer.
	renderSystem->DrawStretchPic(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 1.0f, 1.0f, 0.0f, gameRender.noPostProcessMaterial);

	// Copy everything to _currentRender
	renderSystem->CaptureRenderToImage("_currentRender");

	gameRender.feedbackBufferId = !gameRender.feedbackBufferId;
}