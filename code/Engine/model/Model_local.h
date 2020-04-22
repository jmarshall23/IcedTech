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

#ifndef __MODEL_LOCAL_H__
#define __MODEL_LOCAL_H__

class ColladaParser;
struct rvmEffect_t;
struct rvmEffectSimulation_t;
struct rvmEffectPoint_t;

/*
===============================================================================

	Static model

===============================================================================
*/

class idRenderModelStatic : public idRenderModel {
public:
	// the inherited public interface
	static idRenderModel *		Alloc();

								idRenderModelStatic();
	virtual						~idRenderModelStatic();

	virtual void				InitFromFile( const char *fileName );
	virtual void				DrawEditorModel(idVec3& origin, idMat3& axis, bool cameraView);
	virtual void				PartialInitFromFile( const char *fileName );
	virtual void				PurgeModel();
	virtual void				Reset() {};
	virtual void				LoadModel();
	virtual int					NumFrames() const;
	virtual bool				IsLoaded();
	virtual void				SetLevelLoadReferenced( bool referenced );
	virtual bool				IsLevelLoadReferenced();
	virtual void				TouchData();
	virtual void				ForceFastLoad();
	virtual void				InitEmpty( const char *name );
	virtual void				AddSurface( modelSurface_t surface );
	virtual void				FinishSurfaces();
	virtual void				FreeVertexCache();
	virtual bool				IsSkeletalMesh() const;
	virtual bool				IsWorldMesh(void) const;
	virtual void				MarkWorldMesh(void);
	virtual const char *		Name() const;
	virtual void				Print() const;
	virtual void				List() const;
	virtual int					Memory() const;
	virtual ID_TIME_T				Timestamp() const;
	virtual int					NumSurfaces() const;
	virtual int					NumBaseSurfaces() const;
	virtual const modelSurface_t *Surface( int surfaceNum ) const;
	virtual srfTriangles_t *	AllocSurfaceTriangles( int numVerts, int numIndexes ) const;
	virtual void				FreeSurfaceTriangles( srfTriangles_t *tris ) const;
	virtual srfTriangles_t *	ShadowHull() const;
	virtual bool				IsStaticWorldModel() const;
	virtual dynamicModel_t		IsDynamicModel() const;
	virtual bool				IsDefaultModel() const;
	virtual bool				IsReloadable() const;
	virtual idRenderModel *		InstantiateDynamicModel(const class idRenderEntity* ent, const struct viewDef_s *view, idRenderModel *cachedModel );
	virtual int					NumJoints( void ) const;
	virtual const idMD5Joint *	GetJoints( void ) const;
	virtual jointHandle_t		GetJointHandle( const char *name ) const;
	virtual const char *		GetJointName( jointHandle_t handle ) const;
	virtual const idJointQuat *	GetDefaultPose( void ) const;
	virtual int					NearestJoint( int surfaceNum, int a, int b, int c ) const;
	virtual idBounds			Bounds( const class idRenderEntity*ent ) const;
	virtual void				ReadFromDemoFile( class idDemoFile *f );
	virtual void				WriteToDemoFile( class idDemoFile *f );
	virtual float				DepthHack() const;

	void						MakeDefaultModel();
	
	bool						LoadASE( const char *fileName );
	bool						LoadDAE(const char *fileName);
	bool						LoadLWO( const char *fileName );
	bool						LoadFLT( const char *fileName );
	bool						LoadMA( const char *filename );

	bool						ConvertASEToModelSurfaces( const struct aseModel_s *ase );
	bool						ConvertDAEToModelSurfaces(const ColladaParser* dae);
	bool						ConvertLWOToModelSurfaces( const struct st_lwObject *lwo );
	bool						ConvertMAToModelSurfaces (const struct maModel_s *ma );

	struct aseModel_s *			ConvertLWOToASE( const struct st_lwObject *obj, const char *fileName );

	bool						LoadOBJ(const char *fileName);
	void						ParseOBJ(rvmListSTL<idDrawVert> &drawVerts, const char *fileName, const char *objFileBuffer, int length);

	bool						DeleteSurfaceWithId( int id );
	void						DeleteSurfacesWithNegativeId( void );
	bool						FindSurfaceWithId( int id, int &surfaceNum );

public:
	idList<modelSurface_t>		surfaces;
	idBounds					bounds;
	int							overlaysAdded;

	bool						isWorldMesh;

	// when an md5 is instantiated, the inverted joints array is stored to allow GPU skinning
	int							numInvertedJoints;
	idJointMat *				jointsInverted;
protected:
	int							lastModifiedFrame;
	int							lastArchivedFrame;

	idStr						name;
	srfTriangles_t *			shadowHull;
	bool						isStaticWorldModel;
	bool						defaulted;
	bool						purged;					// eventually we will have dynamic reloading
	bool						fastLoad;				// don't generate tangents and shadow data
	bool						reloadable;				// if not, reloadModels won't check timestamp
	bool						levelLoadReferenced;	// for determining if it needs to be freed
	ID_TIME_T						timeStamp;

	static idCVar				r_mergeModelSurfaces;	// combine model surfaces with the same material
	static idCVar				r_slopVertex;			// merge xyz coordinates this far apart
	static idCVar				r_slopTexCoord;			// merge texture coordinates this far apart
	static idCVar				r_slopNormal;			// merge normals that dot less than this
protected:
	idVertexBuffer*				editorVertexMesh;
	idIndexBuffer*				editorIndexBuffer;
	int							numEditorIndexes;
};

/*
===============================================================================

	Terrain Mesh

===============================================================================
*/

struct rvmHeightMapPixel_t {
	byte r;
	byte g;
	byte b;
	byte a;
};

struct rvmHeightMap_t {
	rvmHeightMap_t()
	{
		buffer = nullptr;
		width = 0;
		height = 0;
	}
	~rvmHeightMap_t()
	{
		if (buffer != nullptr)
		{
			R_StaticFree(buffer);
			buffer = nullptr;
		}

		width = 0;
		height = 0;
	}

	rvmHeightMapPixel_t *buffer;
	int width;
	int height;
};

class rvmRenderModelTerrain : public idRenderModelStatic {
public:
	virtual void	InitFromFile(const char *fileName) override;
private:
	bool ParseFile(const char *fileName);
	float GetHeightForPixel(int x, int y);

	void BuildTerrain(void);
private:
	rvmHeightMap_t heightMap;
	const idMaterial *megaTextureMaterial;
};

/*
===============================================================================

	Raven MDR animated model

===============================================================================
*/
// jmarshall
class rvmRenderModelMDR : public idRenderModelStatic {
public:
	rvmRenderModelMDR();
	~rvmRenderModelMDR();

	virtual void				InitFromFile(const char *fileName);
	virtual dynamicModel_t		IsDynamicModel() const;
	virtual idBounds			Bounds(const class idRenderEntity *ent) const;
	virtual void				TouchData();
	virtual void				LoadModel();
	virtual int					Memory() const;
	virtual idRenderModel *		InstantiateDynamicModel(const class idRenderEntity* ent, const struct viewDef_s *view, idRenderModel *cachedModel);
	virtual int					NumJoints(void) const;
	virtual const idMD5Joint *	GetJoints(void) const;
	virtual jointHandle_t		GetJointHandle(const char *name) const;
	virtual const char *		GetJointName(jointHandle_t handle) const;
	virtual const idJointQuat *	GetDefaultPose(void) const;
	virtual int					NearestJoint(int surfaceNum, int a, int b, int c) const;
private:
	bool						LoadMDRFile(void *buffer, int filesize);
	void						RenderFramesToModel(idRenderModelStatic *staticModel, const struct idRenderEntityParms *ent, const struct viewDef_s *view, const idMaterial *overrideMaterial, int scale);

	int							numLods;

	idBounds					bounds;

	idList<struct mdrFrame_t *>	frames;
	idList<const idMaterial *>	materials;

	int							dataSize;
	struct mdrHeader_t			*modelData;

	idRenderModelStatic			*cachedModel;
};
// jmarshall end

/*
===============================================================================

	MD5 animated model

===============================================================================
*/

class idMD5Mesh {
	friend class				idRenderModelMD5;

public:
								idMD5Mesh();
								~idMD5Mesh();

 	void						ParseMesh( idLexer &parser, int numJoints, const idJointMat *joints );
	idBounds					CalcBounds( const idJointMat *joints );
	int							NearestJoint( int a, int b, int c ) const;
	int							NumVerts( void ) const;
	int							NumTris( void ) const;
	int							NumWeights( void ) const;
	const idMaterial*			GetShader(void) const { return shader; }
public:
	struct deformInfo_s*		deformInfo;			// used to create srfTriangles_t from base frames and new vertexes
private:
	idList<idVec2>				texCoords;			// texture coordinates
	int							numWeights;			// number of weights
	idVec4 *					scaledWeights;		// joint weights
	int *						weightIndex;		// pairs of: joint offset + bool true if next weight is for next vertex
	const idMaterial *			shader;				// material applied to mesh
	int							numTris;			// number of triangles	
	int							surfaceNum;			// number of the static surface created for this mesh

	int							numMeshJoints;
	float						maxJointVertDist;
	byte						*meshJoints;
	idBounds					bounds;
};

class idRenderModelMD5 : public idRenderModelStatic {
public:
// jmarshall
	idRenderModelMD5();
	~idRenderModelMD5();
// jmarshall end

	virtual void				InitFromFile( const char *fileName );
	virtual dynamicModel_t		IsDynamicModel() const;
	virtual idBounds			Bounds( const class idRenderEntity*ent ) const;
	virtual void				Print() const;
	virtual void				List() const;
	virtual void				TouchData();
	virtual idRenderModel*		InstantiateDynamicModel(const class idRenderEntity* ent, const struct viewDef_s* view, idRenderModel* cachedModel);
	virtual void				PurgeModel();
	virtual void				LoadModel();
	virtual int					Memory() const;
	virtual int					NumJoints( void ) const;
	virtual const idMD5Joint *	GetJoints( void ) const;
	virtual jointHandle_t		GetJointHandle( const char *name ) const;
	virtual const char *		GetJointName( jointHandle_t handle ) const;
	virtual const idJointQuat *	GetDefaultPose( void ) const;
	virtual int					NearestJoint( int surfaceNum, int a, int b, int c ) const;
private:
	idJointBuffer*				jointBuffer;

	idList<idMD5Joint>			joints;
	idList<idJointQuat>			defaultPose;
	idList<idMD5Mesh>			meshes;
// jmarshall
	idJointMat					*poseMat3;
	idList<idJointMat>			invertedDefaultPose;
// jmarshall end
	void						CalculateBounds( const idJointMat *joints );
	void						DrawJoints( const idRenderEntityParms *ent, const struct viewDef_s *view ) const;
	void						ParseJoint( idLexer &parser, idMD5Joint *joint, idJointQuat *defaultPose );
};

//
// idRenderModelMD5Instance
//
class idRenderModelMD5Instance : public idRenderModelStatic {
public:
	virtual bool				IsSkeletalMesh() const override;
	void						CreateStaticMeshSurfaces(const idList<idMD5Mesh>& meshes);

	void						UpdateSurfaceGPU(struct deformInfo_s* deformInfo, const const idRenderEntityParms* ent, modelSurface_t* surf, const idMaterial* shader);
public:
	idJointBuffer*				jointBuffer; // This is shared across all models of the same instance!
};

/*
===============================================================================

	MD3 animated model

===============================================================================
*/

struct md3Header_s;
struct md3Surface_s;

class idRenderModelMD3 : public idRenderModelStatic {
public:
	virtual void				InitFromFile( const char *fileName );
	virtual dynamicModel_t		IsDynamicModel() const;
	virtual idRenderModel *		InstantiateDynamicModel(const class idRenderEntity* ent, const struct viewDef_s *view, idRenderModel *cachedModel );
	virtual idBounds			Bounds( const class idRenderEntity*ent ) const;

private:
	int							index;			// model = tr.models[model->index]
	int							dataSize;		// just for listing purposes
	struct md3Header_s *		md3;			// only if type == MOD_MESH
	int							numLods;

	void						LerpMeshVertexes( srfTriangles_t *tri, const struct md3Surface_s *surf, const float backlerp, const int frame, const int oldframe ) const;
};

/*
===============================================================================

	Beam model

===============================================================================
*/

class idRenderModelBeam : public idRenderModelStatic {
public:
	virtual dynamicModel_t		IsDynamicModel() const;
	virtual bool				IsLoaded() const;
	virtual idRenderModel *		InstantiateDynamicModel(const class idRenderEntity* ent, const struct viewDef_s *view, idRenderModel *cachedModel );
	virtual idBounds			Bounds( const class idRenderEntity *ent ) const;
};

/*
================================================================================

	idRenderModelSprite 

================================================================================
*/
class idRenderModelSprite : public idRenderModelStatic {
public:
	virtual	dynamicModel_t	IsDynamicModel() const;
	virtual	bool			IsLoaded() const;
	virtual	idRenderModel *	InstantiateDynamicModel(const class idRenderEntity* ent, const struct viewDef_s *view, idRenderModel *cachedModel );
	virtual	idBounds		Bounds(const class idRenderEntity *ent ) const;
};


/*
===============================================================================

	FX animated model

===============================================================================
*/
class rvmRenderModelFX : public idRenderModelStatic {
public:
							rvmRenderModelFX();
							~rvmRenderModelFX();

	virtual void			InitFromFile(const char* fileName);
	virtual	dynamicModel_t	IsDynamicModel() const;
	virtual	bool			IsLoaded() const;
	virtual	idRenderModel*  InstantiateDynamicModel(const class idRenderEntity* ent, const struct viewDef_s* view, idRenderModel* cachedModel);
	virtual	idBounds		Bounds(const class idRenderEntity* ent) const;
	virtual int				NumFrames() const;
private:
	void					CreateParticle(rvmEffectPoint_t& point, float simScale, idMat3 axis, int size, idDrawVert* verts) const;

	void					ProcessEffectGeometry(int frameNum, idMat3 axis, modelSurface_t* surf, rvmEffect_t* effect);
	void					LoadModel(void);
	void					ParseEffect(rvmEffect_t* effect, idParser* src);
	void					ParseSimulation(const char* fileName, rvmEffectSimulation_t& simuation);
private:
	idList<rvmEffect_t*>	effects;
	int						numFrames;
	idBounds				modelBounds;
	int						size;
	bool					normalizedAlpha;
};

#endif /* !__MODEL_LOCAL_H__ */
