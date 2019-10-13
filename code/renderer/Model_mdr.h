// Model_mdr.h
//

#define	LL(x) x=LittleLong(x)

/*
==============================================================================

MDR file format

==============================================================================
*/

#define MAX_MD3PATH			64		// from quake3

/*
 * Here are the definitions for Ravensoft's model format of md4. Raven stores their
 * playermodels in .mdr files, in some games, which are pretty much like the md4
 * format implemented by ID soft. It seems like ID's original md4 stuff is not used at all.
 * MDR is being used in EliteForce, JediKnight2 and Soldiers of Fortune2 (I think).
 * So this comes in handy for anyone who wants to make it possible to load player
 * models from these games.
 * This format has bone tags, which is similar to the thing you have in md3 I suppose.
 * Raven has released their version of md3view under GPL enabling me to add support
 * to this codebase. Thanks to Steven Howes aka Skinner for helping with example
 * source code.
 *
 * - Thilo Schulz (arny@ats.s.bawue.de)
 */

#define MDR_IDENT	(('5'<<24)+('M'<<16)+('D'<<8)+'R')
#define MDR_VERSION	2
#define	MDR_MAX_BONES	128

struct mdrWeight_t {
	int			boneIndex;	// these are indexes into the boneReferences,
	float		   boneWeight;		// not the global per-frame bone list
	idVec3		offset;
};

struct mdrVertex_t {
	idVec3		normal;
	idVec2		texCoords;
	int			numWeights;
	mdrWeight_t	weights[1];		// variable sized
};

struct mdrTriangle_t {
	int			indexes[3];
};

struct mdrSurface_t {
	int			ident;

	char		name[MAX_MD3PATH];	// polyset name
	char		shader[MAX_MD3PATH];
	int			shaderIndex;	// for in-game use

	int			ofsHeader;	// this will be a negative number

	int			numVerts;
	int			ofsVerts;

	int			numTriangles;
	int			ofsTriangles;

	// Bone references are a set of ints representing all the bones
	// present in any vertex weights for this surface.  This is
	// needed because a model may have surfaces that need to be
	// drawn at different sort times, and we don't want to have
	// to re-interpolate all the bones for each surface.
	int			numBoneReferences;
	int			ofsBoneReferences;

	int			ofsEnd;		// next surface follows
};

struct mdrBone_t {
	float		matrix[3][4];
};

struct mdrFrame_t {
	idVec3		bounds[2];		// bounds of all surfaces of all LOD's for this frame
	idVec3		localOrigin;		// midpoint of bounds, used for sphere cull
	float		radius;			// dist from localOrigin to corner
	char		name[16];
	mdrBone_t	bones[1];		// [numBones]
};

struct mdrCompBone_t {
	unsigned char Comp[24]; // MC_COMP_BYTES is in MatComp.h, but don't want to couple
};

struct mdrCompFrame_t {
	idVec3          bounds[2];		// bounds of all surfaces of all LOD's for this frame
	idVec3          localOrigin;		// midpoint of bounds, used for sphere cull
	float           radius;			// dist from localOrigin to corner
	mdrCompBone_t   bones[1];		// [numBones]
};

struct mdrLOD_t {
	int			numSurfaces;
	int			ofsSurfaces;		// first surface, others follow
	int			ofsEnd;				// next lod follows
};

struct mdrTag_t {
	int                     boneIndex;
	char            name[32];
};

struct mdrHeader_t {
	int			ident;
	int			version;

	char		name[MAX_MD3PATH];	// model name

	// frames and bones are shared by all levels of detail
	int			numFrames;
	int			numBones;
	int			ofsFrames;			// mdrFrame_t[numFrames]

	// each level of detail has completely separate sets of surfaces
	int			numLODs;
	int			ofsLODs;

	int                     numTags;
	int                     ofsTags;

	int			ofsEnd;				// end of file
};

/*
=============================================================

UNCOMPRESSING BONES

=============================================================
*/

#define MC_BITS_X (16)
#define MC_BITS_Y (16)
#define MC_BITS_Z (16)
#define MC_BITS_VECT (16)

#define MC_SCALE_X (1.0f/64)
#define MC_SCALE_Y (1.0f/64)
#define MC_SCALE_Z (1.0f/64)

#define MC_MASK_X ((1<<(MC_BITS_X))-1)
#define MC_MASK_Y ((1<<(MC_BITS_Y))-1)
#define MC_MASK_Z ((1<<(MC_BITS_Z))-1)
#define MC_MASK_VECT ((1<<(MC_BITS_VECT))-1)

#define MC_SCALE_VECT (1.0f/(float)((1<<(MC_BITS_VECT-1))-2))

#define MC_POS_X (0)
#define MC_SHIFT_X (0)

#define MC_POS_Y ((((MC_BITS_X))/8))
#define MC_SHIFT_Y ((((MC_BITS_X)%8)))

#define MC_POS_Z ((((MC_BITS_X+MC_BITS_Y))/8))
#define MC_SHIFT_Z ((((MC_BITS_X+MC_BITS_Y)%8)))

#define MC_POS_V11 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z))/8))
#define MC_SHIFT_V11 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z)%8)))

#define MC_POS_V12 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT))/8))
#define MC_SHIFT_V12 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT)%8)))

#define MC_POS_V13 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*2))/8))
#define MC_SHIFT_V13 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*2)%8)))

#define MC_POS_V21 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*3))/8))
#define MC_SHIFT_V21 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*3)%8)))

#define MC_POS_V22 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*4))/8))
#define MC_SHIFT_V22 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*4)%8)))

#define MC_POS_V23 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*5))/8))
#define MC_SHIFT_V23 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*5)%8)))

#define MC_POS_V31 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*6))/8))
#define MC_SHIFT_V31 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*6)%8)))

#define MC_POS_V32 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*7))/8))
#define MC_SHIFT_V32 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*7)%8)))

#define MC_POS_V33 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*8))/8))
#define MC_SHIFT_V33 ((((MC_BITS_X+MC_BITS_Y+MC_BITS_Z+MC_BITS_VECT*8)%8)))

ID_INLINE void MC_UnCompress(float mat[3][4], const unsigned char * comp)
{
	int val;

	val = (int)((unsigned short *)(comp))[0];
	val -= 1 << (MC_BITS_X - 1);
	mat[0][3] = ((float)(val))*MC_SCALE_X;

	val = (int)((unsigned short *)(comp))[1];
	val -= 1 << (MC_BITS_Y - 1);
	mat[1][3] = ((float)(val))*MC_SCALE_Y;

	val = (int)((unsigned short *)(comp))[2];
	val -= 1 << (MC_BITS_Z - 1);
	mat[2][3] = ((float)(val))*MC_SCALE_Z;

	val = (int)((unsigned short *)(comp))[3];
	val -= 1 << (MC_BITS_VECT - 1);
	mat[0][0] = ((float)(val))*MC_SCALE_VECT;

	val = (int)((unsigned short *)(comp))[4];
	val -= 1 << (MC_BITS_VECT - 1);
	mat[0][1] = ((float)(val))*MC_SCALE_VECT;

	val = (int)((unsigned short *)(comp))[5];
	val -= 1 << (MC_BITS_VECT - 1);
	mat[0][2] = ((float)(val))*MC_SCALE_VECT;


	val = (int)((unsigned short *)(comp))[6];
	val -= 1 << (MC_BITS_VECT - 1);
	mat[1][0] = ((float)(val))*MC_SCALE_VECT;

	val = (int)((unsigned short *)(comp))[7];
	val -= 1 << (MC_BITS_VECT - 1);
	mat[1][1] = ((float)(val))*MC_SCALE_VECT;

	val = (int)((unsigned short *)(comp))[8];
	val -= 1 << (MC_BITS_VECT - 1);
	mat[1][2] = ((float)(val))*MC_SCALE_VECT;


	val = (int)((unsigned short *)(comp))[9];
	val -= 1 << (MC_BITS_VECT - 1);
	mat[2][0] = ((float)(val))*MC_SCALE_VECT;

	val = (int)((unsigned short *)(comp))[10];
	val -= 1 << (MC_BITS_VECT - 1);
	mat[2][1] = ((float)(val))*MC_SCALE_VECT;

	val = (int)((unsigned short *)(comp))[11];
	val -= 1 << (MC_BITS_VECT - 1);
	mat[2][2] = ((float)(val))*MC_SCALE_VECT;
}