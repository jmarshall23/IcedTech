// MegaLight.cpp
//

#include "tools_precompiled.h"
#include "MegaLight_file.h"

/*
================
AddModelToMegaGeometry
================
*/
void AddModelToMegaGeometry(MegaLightGeometry_t &geometry, idVec3 origin, idMat3 axis, idRenderModel *renderModel)
{
	for (int i = 0; i < renderModel->NumSurfaces(); i++)
	{
		const modelSurface_t *surface = renderModel->Surface(i);
		bool hasMegaTexture = surface->shader->HasMegaTexture();

		int startIndex = geometry.vertexes.Num();
		for (int d = 0; d < surface->geometry->numIndexes; d++)
		{
			geometry.indexes.Append(startIndex + surface->geometry->indexes[d]);
		}

		for (int d = 0; d < surface->geometry->numVerts; d++)
		{
			const idDrawVert *vert = &surface->geometry->verts[d];
			MegaLightVertex_t megaVert;

			megaVert.xyz = vert->xyz * axis + origin;
			megaVert.st = vert->st;
			megaVert.normal = vert->normal;
			megaVert.isMegaTextureGeometry = hasMegaTexture;
			geometry.vertexes.Append(megaVert);
		}
	}
}

/*
================
MegaLight
================
*/
void MegaLight(idStr mapName, int lightmapSize) {
	MegaLightGeometry_t geometry;

	mapName.StripFileExtension();
	idStr procName = "maps/";
	procName += mapName;

	common->Printf("---- MegaLight ----\n");

	idRenderWorld *world = renderSystem->AllocRenderWorld();
	if (!world->InitFromMap(procName))
	{
		renderSystem->FreeRenderWorld(world);
		common->Warning("Failed to load renderworld %s\n", procName.c_str());
		return;
	}

	if (world->IsLegacyWorld())
	{
		renderSystem->FreeRenderWorld(world);
		common->Warning("This map is in the legacy format, please run dmap on this map to generate world files.\n", procName.c_str());
		return;
	}

	idMapFile *mapFile = new idMapFile();
	if (!mapFile->Parse(procName + ".map")) {
		renderSystem->FreeRenderWorld(world);
		delete mapFile;
		common->Warning("Failed to load rendermap %s\n", procName.c_str());
		return;
	}

	for (int i = 0; i < world->GetNumWorldModels(); i++)
	{
		idMat3	axis;
		idVec3 origin;
		idRenderModel *model;

		axis.Identity();
		origin.Zero();

		model = world->GetWorldModel(i);

		AddModelToMegaGeometry(geometry, origin, axis, model);
	}

	for (int i = 0; i < mapFile->GetNumEntities(); i++)
	{
		idMapEntity *entity = mapFile->GetEntity(i);
		const char *className = entity->epairs.GetString("classname");
	/*
		if (!idStr::Icmp(className, "func_static")) {
			const char *modelName = entity->epairs.GetString("model");
			if (!modelName) {
				continue;
			}
			idRenderModel	*model = renderModelManager->FindModel(modelName);

			idMat3	axis;
			// get the rotation matrix in either full form, or single angle form
			if (!entity->epairs.GetMatrix("rotation", "1 0 0 0 1 0 0 0 1", axis)) {
				float angle = entity->epairs.GetFloat("angle");
				if (angle != 0.0f) {
					axis = idAngles(0.0f, angle, 0.0f).ToMat3();
				}
				else {
					axis.Identity();
				}
			}

			idVec3 origin = entity->epairs.GetVec4("origin").ToVec3();
			AddModelToMegaGeometry(geometry, origin, axis, model);
			continue;
		}
		else if (!idStr::Icmp(className, "light")) {
	*/
		if (!idStr::Icmp(className, "light") && entity->epairs.GetBool("bake")) {
			MegaLight_t megaLight;
			megaLight.origin = entity->epairs.GetVec4("origin").ToVec3();
			megaLight.color = entity->epairs.GetVec4("_color", "1 1 1 1").ToVec3();
			megaLight.isSkyLight = entity->epairs.GetInt("skylight", "0") != 0;
			geometry.lights.Append(megaLight);
		}
	}

	common->Printf("Generating MegaLight file with %d lights %d vertexes and %d indexes\n", geometry.lights.Num(), geometry.vertexes.Num(), geometry.indexes.Num());
	common->Printf("Writing MegaLight file...\n");

	MegaLightFileHeader_t header;
	header.numVertexes = geometry.vertexes.Num();
	header.numIndexes = geometry.indexes.Num();
	header.numLights = geometry.lights.Num();

	idFile *megaLightFile = fileSystem->OpenFileWrite(procName + ".megalight", "fs_basepath");
	if (!megaLightFile)
	{
		common->FatalError("Failed to open megafile for writing\n");
		return;
	}

	megaLightFile->Write(&header, sizeof(MegaLightFileHeader_t));
	megaLightFile->Write(&geometry.vertexes[0], sizeof(MegaLightVertex_t) * geometry.vertexes.Num());
	megaLightFile->Write(&geometry.indexes[0], sizeof(unsigned int) * geometry.indexes.Num());
	megaLightFile->Write(&geometry.lights[0], sizeof(MegaLight_t) * geometry.lights.Num());
	megaLightFile->Flush();

	fileSystem->CloseFile(megaLightFile);

	delete mapFile;
	renderSystem->FreeRenderWorld(world);

	idStr megaLightPath = fileSystem->RelativePathToOSPath(procName);
	megaLightPath.SetFileExtension(".megalight");

	idStr mapFileName;
	mapName.ExtractFileName(mapFileName);

	idStr megaLitFileName = "megaTextures/";
	megaLitFileName += mapFileName;
	megaLitFileName += "_lit";

	idStr megaLitFullFileName = fileSystem->RelativePathToOSPath(megaLitFileName);

	idStr cmdLine = va("tools\\renderlight.exe %s %s %d", megaLightPath.c_str(), megaLitFullFileName.c_str(), lightmapSize);

	common->Printf("Launching RenderLight...\n");
	sys->StartProcess(cmdLine.c_str(), false , "tools");
	common->Printf("RenderLight Complete\n");
}

/*
================
MegaLight_f
================
*/
void MegaLight_f(const idCmdArgs &args) {
	common->SetRefreshOnPrint(true);

	if (args.Argc() < 3) {
		common->Warning("Usage: megalight <proc_file> <lightmapsize>\n");
		return;
	}

	idStr mapName = args.Argv(1);
	int lightmapSize = atoi(args.Argv(2));

	MegaLight(mapName, lightmapSize);

	common->SetRefreshOnPrint(false);
}