/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/


#include "maya_precompiled.h"
#include "maya_main.h"
#include "ExportOptions.h"

#undef strncmp

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>   
#include <assimp/postprocess.h> 
#include <assimp/scene.h>      

using namespace std;

idSys* sys = NULL;
idCommon* common = NULL;
idCVarSystem* cvarSystem = NULL;
idFileSystem* fileSystem = NULL;

idCVar* idCVar::staticVars = NULL;

struct rvmExportJointWeight {
	void Reset() {
		for(int i = 0; i < 4; i++) {
			jointIndex[i] = -1;
			weights[i] = 0;
		}

		numWeights = 0;
	}

	int numWeights;
	int jointIndex[4];
	float weights[4];
};

//
// rvmExportMesh
//
struct rvmExportMesh {
	aiMesh* mesh;
	idStr mtrName;
	idList<int> indexes;
	idList<idDrawVert> vertexes;
	idList<rvmExportJointWeight> vertexWeights;

	void ResetWeights() {
		for (int i = 0; i < vertexWeights.Num(); i++) {
			vertexWeights[i].Reset();
		}
	}
};

struct rvmBoneEntry {
	idStr name;
	const aiBone* bone;

	inline bool operator==(const rvmBoneEntry& rhs) { 
		if (name != rhs.name)
			return false;

		return true;
	}
};

/*
===============
ConvertToIdSpace
===============
*/
idVec3 ConvertToIdSpace(const aiVector3D& pos) {
	idVec3 idpos;

	idpos.x = pos.x;
	idpos.y = -pos.z;
	idpos.z = pos.y;

	return idpos;
}

/*
===============
ConvertToIdSpace
===============
*/
idVec3 ConvertToIdSpace(const idVec3& pos) {
	idVec3 idpos;

	idpos.x = pos.x;
	idpos.y = -pos.z;
	idpos.z = pos.y;

	return idpos;
}

// https://github.com/ccxvii/asstools/blob/master/assview.c
// find a node by name in the hierarchy (for anims and bones)
struct aiNode* FindNodeByName(struct aiNode* node, const char* name)
{
	int i;
	if (!strcmp(name, node->mName.data))
		return node;
	for (i = 0; i < node->mNumChildren; i++) {
		struct aiNode* found = FindNodeByName(node->mChildren[i], name);
		if (found)
			return found;
	}
	return NULL;
}

// https://github.com/ccxvii/asstools/blob/master/assview.c
void transformnode(aiMatrix4x4* result, struct aiNode* node)
{
	if (node->mParent) {
		transformnode(result, node->mParent);
		aiMultiplyMatrix4(result, &node->mTransformation);
	}
	else {
		*result = node->mTransformation;
	}
}


// https://www.gamedev.net/forums/topic/648859-assimp-build-the-skeleton-from-the-scene/
static void GatherBoneNames(const aiScene* scene, struct aiNode* nodes, idList<rvmBoneEntry> &bones)
{
	bones.Clear();

	// Loop through each bone of each mesh.
	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		const aiMesh* mesh = scene->mMeshes[meshIndex];
		for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
		{
			const aiBone* bone = mesh->mBones[boneIndex];
			const aiNode* node = FindNodeByName(nodes, bone->mName.C_Str());
			while (node)
			{
				rvmBoneEntry entry;
				entry.bone = bone;
				entry.name = node->mName.C_Str();
				bones.AddUnique(entry);
				node = node->mParent;
				if (node && node->mNumChildren == 1) {
					break;	// don't chase up until the scene root, if possible
				}
			}
		}
	}
}

struct BoneDesc
{
	const aiNode* node;
	idStr	name;
	int		parentIndex;
	idMat3	rotation;
	idMat3  worldRotation;
	idVec3 location;
	idVec3 worldTransform;
	const aiBone* bone;
};

// parentTransform * node->mTransformation;

static void BuildSkeleton(const aiNode* node, const idList<rvmBoneEntry>& boneNames, const idVec3 translation, const idMat3 &rotation, const int parentBoneIndex, idList< BoneDesc >& skeleton)
{
	idVec3 nodeTranslation = idVec3(node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4);
	aiMatrix3x3 matrix = aiMatrix3x3(node->mTransformation);

	// accumulated parents => node transform
	idVec3 globalSkeletalTranslation = translation + nodeTranslation;
	idMat3 currentRotation;
	memcpy(currentRotation.ToFloatPtr(), &matrix, sizeof(idMat3));

	idMat3 globalRotation = currentRotation * rotation;

	int newBoneIndex = parentBoneIndex;

	int index = -1; 
	
	for(int i = 0; i < boneNames.Num(); i++) {
		if(boneNames[i].name == node->mName.C_Str()) {
			index = i;
			break;
		}
	}

	if (index != -1)
	{
		newBoneIndex = skeleton.Num();

		BoneDesc newBone;

		newBone.node = node;
		newBone.name = node->mName.C_Str();
		newBone.location = globalSkeletalTranslation;
		newBone.rotation = globalRotation;

		newBone.parentIndex = parentBoneIndex;
		newBone.bone = boneNames[index].bone;

		skeleton.Append(newBone);

		globalRotation.Identity();
		globalSkeletalTranslation.Zero();
	}
	for (int childIndex = 0; childIndex < node->mNumChildren; childIndex++)
	{
		const aiNode* childNode = node->mChildren[childIndex];
		BuildSkeleton(childNode, boneNames, globalSkeletalTranslation, globalRotation, newBoneIndex, skeleton);
	}
}

int FindExportJoint(aiBone* bone, idList<BoneDesc>& exportJoints) {
	for (int i = 0; i < exportJoints.Num(); i++) {
		if (exportJoints[i].name == bone->mName.C_Str()) {
			return i;
		}
	}

	common->FatalError("FindExportJoint: Failed to find joint!\n");
	return -1;
}
/*
===============
WriteMD6Mesh
===============
*/
void WriteMD6Mesh(const char *dest, idList< BoneDesc > &skeleton, rvmExportMesh* meshes, int numMeshes, const char *commandLine) {
	common->Printf("Writing MD6 mesh %s\n", dest);

	idFile* file = fileSystem->OpenExplicitFileWrite(dest);

	file->WriteFloatString(MD6_VERSION_STRING " %d\n", MD6_VERSION);

	// Write the init block.
	file->WriteFloatString("init {\n");
		file->WriteFloatString("commandLine \"%s\"\n", commandLine);
		file->WriteFloatString("sourceFile \"not_set.mb\"\n");
		file->WriteFloatString("numMeshes %d\n", numMeshes);
		file->WriteFloatString("numJoints %d\n", skeleton.Num());
		file->WriteFloatString("numUserChannels 0\n");
		file->WriteFloatString("numWeightSets 8\n");
		file->WriteFloatString("minExpand ( -16 -16 -16 )\n");
		file->WriteFloatString("maxExpand ( 16 16 16 )\n");
		file->WriteFloatString("remapForSkinning 1\n");
		file->WriteFloatString("morphMap \"\"\n");
	file->WriteFloatString("}\n");

	// Write the joint block
	file->WriteFloatString("joints {\n");
		for(int i = 0; i < skeleton.Num(); i++) {
			idQuat quat = skeleton[i].rotation.ToQuat();
			idVec3 translation = skeleton[i].location;

			idStr parentName = "";
			if(skeleton[i].parentIndex != -1) {				
				parentName = skeleton[skeleton[i].parentIndex].name;

				//translation = translation + skeleton[skeleton[i].parentIndex].worldTransform;
				skeleton[i].worldRotation = skeleton[i].worldRotation * skeleton[skeleton[i].parentIndex].worldRotation;
			}

			skeleton[i].worldTransform = translation;
			skeleton[i].worldRotation = quat.ToMat3();

			file->WriteFloatString("\t \"%s\" %d ( 1 0 0 0 0 0 0 0 ) ( %f %f %f %f ) ( 1 1 1 ) ( %f %f %f ) // %s\n", skeleton[i].name.c_str(), skeleton[i].parentIndex, 
									quat.x, quat.y, quat.z, quat.w,
									translation.x, translation.y, translation.z,
									parentName.c_str());
		}
	file->WriteFloatString("}\n");

	// Write out all of our meshes.
	for(int i = 0; i < numMeshes; i++) {
		file->WriteFloatString("mesh {\n");
			file->WriteFloatString("\tname \"%s\"\n", meshes[i].mesh->mName.C_Str());
			file->WriteFloatString("\tshader \"%s\"\n", meshes[i].mtrName.c_str());

			// Write out all the vertexes.
			file->WriteFloatString("\tverts %d {\n", meshes[i].vertexes.Num());
				for(int d = 0; d < meshes[i].vertexes.Num(); d++) {
					idDrawVert* vert = &meshes[i].vertexes[d];
					rvmExportJointWeight* weight = &meshes[i].vertexWeights[d];
					file->WriteFloatString("\t\tvert %d ( %f %f %f ) ( %f %f ) ( %d %d %d %d %f %f %f %f )\n", d, vert->xyz.x, vert->xyz.y, vert->xyz.z,
						vert->st.x, vert->st.y,
						weight->jointIndex[0], weight->jointIndex[1], weight->jointIndex[2], weight->jointIndex[3],
						weight->weights[0], weight->weights[1], weight->weights[2], weight->weights[3]);
				}
			file->WriteFloatString("\t}\n");

			// Write out all the triangles
			int triCount = meshes[i].indexes.Num() / 3;
			file->WriteFloatString("\ttris %d {\n", triCount);
				for(int d = 0; d < triCount; d++) {
					file->WriteFloatString("\ttri %d\t %d %d %d\n", d, meshes[i].indexes[d * 3 + 0], meshes[i].indexes[d * 3 + 1], meshes[i].indexes[d * 3 + 2]);
				}
			file->WriteFloatString("\t}\n");
		file->WriteFloatString("}\n");
	}

	fileSystem->CloseFile(file);
}

/*
===============
ExportMesh
===============
*/
void ExportMesh(const char *src, const char *dest, idExportOptions &options) {
	common->Printf("Opening Mesh %s\n", src);
	aiScene* scene = (aiScene*)aiImportFile(src, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	if (scene == nullptr) {
		common->Warning("ExportMesh: Failed to open mesh %s\n", dest);
		return;
	}

	// Find the origin node.
	struct aiNode* originNode = FindNodeByName(scene->mRootNode, "origin");
	if (originNode == NULL) {
		common->Error("You don't have a origin bone in your mesh!\n");
		return;
	}

	idList< BoneDesc > skeleton;
	idList<rvmBoneEntry> boneNames;
	GatherBoneNames(scene, originNode, boneNames);
	
	idVec3 _origin;
	idMat3 _axis;

	_origin.Zero();
	_axis.Identity();

	BuildSkeleton(originNode, boneNames, _origin, _axis, -1, skeleton);

	// Grab all of the meshes.
	int numMeshes = scene->mNumMeshes;
	idList<rvmExportMesh> meshes;// = new rvmExportMesh[numMeshes];
	meshes.SetNum(numMeshes);

	for(int i = 0; i < numMeshes; i++) {
		rvmExportMesh* mesh = &meshes[i];

		mesh->mesh = scene->mMeshes[i];
		mesh->vertexes.SetNum(mesh->mesh->mNumVertices);
		mesh->vertexWeights.SetNum(mesh->mesh->mNumVertices);

		mesh->ResetWeights();

		for(int f = 0; f < mesh->mesh->mNumVertices; f++) {
			mesh->vertexes[f].xyz = ConvertToIdSpace(mesh->mesh->mVertices[f]);
			mesh->vertexes[f].st = idVec2(mesh->mesh->mTextureCoords[0][f].x, mesh->mesh->mTextureCoords[0][f].y);
		}

		mesh->indexes.SetNum(mesh->mesh->mNumFaces * 3);
		for(int f = 0; f < mesh->mesh->mNumFaces; f++) {
			aiFace* face = &mesh->mesh->mFaces[f];
			mesh->indexes[f * 3 + 2] = face->mIndices[0];
			mesh->indexes[f * 3 + 1] = face->mIndices[1];
			mesh->indexes[f * 3 + 0] = face->mIndices[2];
		}

		for(int f = 0; f < mesh->mesh->mNumBones; f++) {
			aiBone* bone = mesh->mesh->mBones[f];

			for(int g = 0; g < bone->mNumWeights; g++) {
				int vertexId = bone->mWeights[g].mVertexId;
				float weight = bone->mWeights[g].mWeight;
				
				if (weight > 0) {
					mesh->vertexWeights[vertexId].jointIndex[mesh->vertexWeights[vertexId].numWeights] = FindExportJoint(bone, skeleton);
					mesh->vertexWeights[vertexId].weights[mesh->vertexWeights[vertexId].numWeights] = weight;
					mesh->vertexWeights[vertexId].numWeights++;
				}
			}
		}

		aiString Path;
		const aiMaterial* material = scene->mMaterials[mesh->mesh->mMaterialIndex];
		material->GetTexture(aiTextureType_DIFFUSE, 0, &Path);
		mesh->mtrName = Path.C_Str();

		if(mesh->mtrName.Length() <= 0) {
			common->Warning("No diffuse texture assigned for mesh %s, using implicit path", mesh->mesh->mName.C_Str());

			const char* meshName = mesh->mesh->mName.C_Str();
			const char *skipTo = strstr(meshName, ":");
			if(skipTo != NULL) {
				meshName = skipTo + 1;
			}

			idStr modelSrcPath = src;			
			modelSrcPath = fileSystem->OSPathToRelativePath(modelSrcPath.c_str());
			modelSrcPath.StripFilename();
			modelSrcPath += "/";
			modelSrcPath += meshName;

			mesh->mtrName = modelSrcPath;
		}
		else {
			mesh->mtrName = fileSystem->OSPathToRelativePath(mesh->mtrName);
		}
	}

	WriteMD6Mesh(dest, skeleton, meshes.Ptr(), numMeshes, options.commandLine);
}

/*
==============================================================================

dll setup

==============================================================================
*/

/*
===============
Maya_Shutdown
===============
*/
void Maya_Shutdown( void ) {

}

/*
===============
Maya_ConvertModel
===============
*/
const char *Maya_ConvertModel( const char *ospath, const char *commandline ) {
	idExportOptions options(commandline, ospath);

	switch(options.type) {
		case WRITE_MESH:
			ExportMesh(options.src, options.dest, options);
			break;
	}

	return "Ok";
}

/*
===============
dllEntry
===============
*/
bool dllEntry( int version, idCommon *common, idSys *sys, idFileSystem* fileSystem) {

	if ( !common || !sys ) {
		return false;
	}

	::common = common;
	::sys = sys;
	::fileSystem = fileSystem;
	::cvarSystem = NULL;

	idLib::sys			= sys;
	idLib::common		= common;
	idLib::cvarSystem	= NULL;
	idLib::fileSystem	= fileSystem;

	idLib::Init();

	if ( version != MD5_VERSION ) {
		common->Printf( "Error initializing Maya exporter: DLL version %d different from .exe version %d\n", MD5_VERSION, version );
		return false;
	}

	return true;
}

// Force type checking on the interface functions to help ensure that they match the ones in the .exe
const exporterDLLEntry_t	ValidateEntry = &dllEntry;
const exporterInterface_t	ValidateConvert = &Maya_ConvertModel;
const exporterShutdown_t	ValidateShutdown = &Maya_Shutdown;
