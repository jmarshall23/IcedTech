// Image_IES.cpp
//

#include "engine_precompiled.h"

#include "tr_local.h"
#include "../external/iesloader/ies_loader.h"

/*
=======================
R_CreateIESImage
=======================
*/
idImage *R_CreateIESImage(const char *name, IESLoadHelper &data, IESFileInfo &iesinfo) {
	idImageOpts opts;
	opts.format = FMT_R32;
	opts.colorFormat = CFM_DEFAULT;
	opts.numLevels = 1;
	opts.textureType = TT_2D;
	opts.isPersistant = true;
	opts.width = 256;
	opts.height = 1;
	opts.numMSAASamples = 0;

	idImage* image = globalImages->ScratchImage(name, &opts, TF_LINEAR, TR_REPEAT, TD_DIFFUSE);

	idTempArray<float> pixelbuffer(opts.width * opts.height);

	data.saveAs1D(iesinfo, pixelbuffer.Ptr(), 256, 1);
	image->SubImageUpload(0, 0, 0, 0, opts.width, opts.height, pixelbuffer.Ptr());

	return image;
}

/*
=======================
idImageManager::LoadIESImage
=======================
*/
idImage* idImageManager::LoadIESImage(const char* name) const {
	idImage* loadedImage = GetImage(name);
	if(loadedImage) {
		return loadedImage;
	}

	char* fileData = NULL;
	int fileLength = fileSystem->ReadFile(name, (void **)&fileData, NULL);

	if(fileLength <= 0 || fileData == NULL) {
		common->Warning("idImageManager::LoadIESImage: Failed to load image %s\n", name);
		return NULL;
	}

	IESLoadHelper iesload;
	IESFileInfo iesinfo;

	if(!iesload.load(fileData, fileLength, iesinfo)) {
		fileSystem->FreeFile(fileData);
		common->FatalError("Failed to load IES file, failed to parse\n", name);
	}

	return R_CreateIESImage(name, iesload, iesinfo);
}