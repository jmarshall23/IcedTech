// VirtualTextureFeedbackJob.cpp
//

//#include <renderer/qgllib/glew.h>
#include <GL/glew.h>
#include "Engine_precompiled.h"

#include "tr_local.h"

idCVar vt_transcodeDebugPageOffset("vt_transcodeDebugPageOffset", "0", CVAR_INTEGER, "debug page offset");
idCVar vt_evictionSeconds("vt_evictionSeconds", "10", CVAR_INTEGER, "if not rendered for this many sections, then it can be evicted");
/*
=============================
rvmVirtualTextureSystem::CheckFeedbackPixel
=============================
*/
bool rvmVirtualTextureSystem::CheckFeedbackPixel(feedbackPixel_t pixel, int &uploadX, int &uploadY) {
	int freePage = -1;
	double currentTime = Sys_GetClockTicks();
	double evictionTime = currentTime + (Sys_ClockTicksPerSecond() * vt_evictionSeconds.GetInteger());


	int pageX, pageY, pageSource, pageLOD;
	UnpackFeedbackPixel(pixel, pageX, pageY, pageSource, pageLOD);

	// Page Source 0 means non virtual textured.
	if (pageSource == 0)
		return false;

	pageSource = pageSource - 1;

	// Check to see if the page is already loaded.
	for (int i = 0; i < numVTPages * numVTPages; i++)
	{
		if (vt_pages[i].feedbackInfo.packed == pixel.packed)
		{
			vt_pages[i].evictionTime = evictionTime;
			return false;
		}
	}

	// Check to see if we need to assign this material to a new page, and in the process if we can evict a old page.
	for (int i = 0; i < numVTPages * numVTPages; i++)
	{
		if (vt_pages[i].material == nullptr || currentTime >= vt_pages[i].evictionTime )
		{
			freePage = i;
			uploadX = vt_pages[i].uploadX;
			uploadY = vt_pages[i].uploadY;

			vt_pages[i].material = virtualMaterials[pageSource];
			vt_pages[i].evictionTime = evictionTime;
			break;
		}
	}

	if (freePage == -1)
		return false;

	vt_pages[freePage].feedbackInfo.packed = pixel.packed;
	return true;
}

/*
============================
rvmVirtualTextureSystem::RunFeedbackJob
============================
*/
void rvmVirtualTextureSystem::RunFeedbackJob(idImage *feedbackImage) {
	idList<int64_t> feedbackUniqueInfoList;
	idList<idMaterial *> updatedMaterials;

	if (feedbackCPUbuffer == nullptr) {
		feedbackCPUbuffer = new feedbackPixel_t[feedbackImage->GetUploadWidth() * feedbackImage->GetUploadHeight()];
		feedbackCPUbufferLen = feedbackImage->GetUploadWidth() * feedbackImage->GetUploadHeight();
	} else if( feedbackCPUbufferLen != feedbackImage->GetUploadWidth() * feedbackImage->GetUploadHeight() ){
	    common->Error("Buffersize mismatch!. Allocated(%d) - currentImage(%d)\n", feedbackCPUbufferLen, feedbackImage->GetUploadWidth() * feedbackImage->GetUploadHeight() );
	}

	// This function is called more than once.
	//	--->	if(feedbackCPUbuffer[i].packed == 0)
	// Buffer must be reset in order to check for this. Could be optimized.
	memset( feedbackCPUbuffer, 0, (feedbackCPUbufferLen * sizeof(feedbackPixel_t)) );

	// If the transcode show pages cvar has been changed, reset the current available pages so we can resubmit the transcoded buffers.
	if (vt_transcodeShowPages.IsModified())
	{
		ClearVTPages();
	}

    assert( feedbackImage->dataFormat == GL_RGBA );
	assert( feedbackImage->dataType == GL_HALF_FLOAT );
    if( glConfig.directStateAccess ){
        glGetTextureImage(feedbackImage->texnum, 0, GL_RGBA, GL_HALF_FLOAT, feedbackCPUbufferLen * sizeof(feedbackPixel_t), feedbackCPUbuffer);
    } else {
        glBindTexture( GL_TEXTURE_2D, feedbackImage->texnum );
        glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_HALF_FLOAT, feedbackCPUbuffer );
    }

	for (int i = 0; i < feedbackImage->GetUploadWidth() * feedbackImage->GetUploadHeight(); i++)
	{
		if(feedbackCPUbuffer[i].packed == 0)
			continue;

		feedbackUniqueInfoList.AddUnique(feedbackCPUbuffer[i].packed);
	}

	for (int i = 0; i < feedbackUniqueInfoList.Num(); i++)
	{
		feedbackPixel_t pixel;
		pixel.packed = feedbackUniqueInfoList[i];

		// Check to see if we need to upload this tile.
		int uploadX, uploadY;
		if(!CheckFeedbackPixel(pixel, uploadX, uploadY))
			continue;

		// Parse the feedback pixel.
		int pageX, pageY, pageSource, pageLOD;
		UnpackFeedbackPixel(pixel, pageX, pageY, pageSource, pageLOD);
		
		// Page Source 0 in the feedback means non virtual textured, but we can have a pageSource of 0 as a material so substract 1.
		pageSource = pageSource - 1;

		idMaterial *material = virtualMaterials[pageSource];

        if( !material )// lwss- is this a HACK? was getting nullptr deref
            continue;

		updatedMaterials.AddUnique(material);

		// Transcode the albedo virtual image.
		TranscodePage(albedoVirtualTexture, material->GetVirtualAlbedoImage(), pageX, pageY, uploadX, uploadY, pageLOD);
		
		// If this material has a normal image, transcode it, if not use the default image.
		if (material->GetVirtualNormalImage() != nullptr)
		{
			TranscodePage(normalVirtualTexture, material->GetVirtualNormalImage(), pageX, pageY, uploadX, uploadY, pageLOD);
		}
		else
		{
			// Since Doom 3 is lacking normal maps for a bunch of textures, we have this system. 
			rvmVirtualImage *defaultNormalImage = GetDefaultVirtualNormalTexture(material->GetVirtualAlbedoImage()->GetWidth(pageLOD), material->GetVirtualAlbedoImage()->GetHeight(pageLOD));

			TranscodePage(normalVirtualTexture, defaultNormalImage, pageX, pageY, uploadX, uploadY, pageLOD);
		}

		// If this material has a spec image, transcode it, if not use the default image.
		if (material->GetVirtualSpecImage() != nullptr)
		{
			TranscodePage(specularVirtualTexture, material->GetVirtualSpecImage(), pageX, pageY, uploadX, uploadY, pageLOD);
		}
		else
		{
			TranscodePage(specularVirtualTexture, defaultSpecVirtualImage, pageX, pageY, uploadX, uploadY, pageLOD);
		}

		float pageOffsetX = uploadX;
		float pageOffsetY = uploadY;
		material->SetVirtualPageOffset(pageLOD, pageX, pageY, pageOffsetX, pageOffsetY);

		if (vt_transcodeDebugPageOffset.GetInteger() > 0)
		{
			common->Printf("%s - pageX=%d,pageY=%d, pageSource=%d, pageLOD=%d\n", material->GetVirtualAlbedoImage()->GetName(), pageX, pageY, pageSource, pageLOD);
		}
	}

	for (int i = 0; i < updatedMaterials.Num(); i++)
	{
		updatedMaterials[i]->UpdateVirtualMaterialOffsetImage();		
	}

	// If vt_transcodeShowPages has been modified, clear the modified flag.
	if (vt_transcodeShowPages.IsModified())
	{
		vt_transcodeShowPages.ClearModified();
	}
}