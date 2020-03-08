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
#include "engine_precompiled.h"
#pragma hdrstop

#include "tr_local.h"

idCVar idMegaTexture::r_megaTextureLevel( "r_megaTextureLevel", "0", CVAR_RENDERER | CVAR_INTEGER, "draw only a specific level" );
idCVar idMegaTexture::r_showMegaTexture( "r_showMegaTexture", "0", CVAR_RENDERER | CVAR_BOOL, "display all the level images" );
idCVar idMegaTexture::r_showMegaTextureLabels( "r_showMegaTextureLabels", "0", CVAR_RENDERER | CVAR_BOOL, "draw colored blocks in each tile" );
idCVar idMegaTexture::r_skipMegaTexture( "r_skipMegaTexture", "0", CVAR_RENDERER | CVAR_INTEGER, "only use the lowest level image" );
idCVar idMegaTexture::r_terrainScale( "r_terrainScale", "3", CVAR_RENDERER | CVAR_INTEGER, "vertically scale USGS data" );
/*
====================
idMegaTexture::idMegaTexture
====================
*/
idMegaTexture::idMegaTexture() {
	albedoLitMegaTextureFile = nullptr;
}

/*
====================
idMegaTexture::~idMegaTexture
====================
*/
idMegaTexture::~idMegaTexture() {
	if (albedoLitMegaTextureFile != nullptr) {
		delete albedoLitMegaTextureFile;
		albedoLitMegaTextureFile = nullptr;
	}
}

/*
====================
InitFromMegaFile
====================
*/
bool idMegaTexture::InitFromMegaFile( const char *fileBase ) {
	idStr	name = "megaTextures/";
	idStr   megaMaskName;

	name += fileBase;
	name.StripFileExtension();
	megaMaskName = name;

	name += ".mega";
	megaMaskName += "_mask.tga";

	// Open the mega mask image.
	megaNormalMaskImage = globalImages->ImageFromFile(megaMaskName.c_str(), TF_LINEAR, TR_REPEAT, TD_DIFFUSE, CF_2D);
	if (megaNormalMaskImage == nullptr) {
		common->Printf("idMegaTexture: failed to open mega mask %s\n", megaMaskName.c_str());
		return false;
	}

	// Load in the megatexture file.
	albedoLitMegaTextureFile = rvmMegaTextureFile::LoadMegaTextureFile(name);
	if (albedoLitMegaTextureFile == nullptr) {
		return false;
	}

	currentTriMapping = NULL;

	// force first bind to load everything
	currentViewOrigin[0] = -99999999.0f;
	currentViewOrigin[1] = -99999999.0f;
	currentViewOrigin[2] = -99999999.0f;

	return true;
}

/*
====================
SetMappingForSurface

analyzes xyz and st to create a mapping
This is not very robust, but works for rectangular grids
====================
*/
void	idMegaTexture::SetMappingForSurface( const srfTriangles_t *tri ) {
	if ( tri == currentTriMapping ) {
		return;
	}
	currentTriMapping = tri;

	if ( !tri->verts ) {
		return;
	}
// jmarshall - heavily modified this, basically all we care about here is the min/max of the terrain.
	surfaceBounds = tri->bounds;
// jmarshall end
}

/*
====================
BindForViewOrigin
====================
*/
void idMegaTexture::BindForViewOrigin( const idVec3 viewOrigin ) {

	SetViewOrigin( viewOrigin );

	albedoLitMegaTextureFile->BindForViewOrigin(viewOrigin);

	// Bind the mega normal mask image.
	GL_SelectTexture(7);
	megaNormalMaskImage->Bind();
}

/*
====================
Unbind

This can go away once everything uses fragment programs so the enable states don't
need tracking
====================
*/
void idMegaTexture::Unbind( void ) {
	for ( int i = 0 ; i < MAX_LEVELS; i++ ) {
		GL_SelectTexture( 1+i );
		globalImages->BindNull();
	}
}


/*
====================
SetViewOrigin
====================
*/
void idMegaTexture::SetViewOrigin( const idVec3 viewOrigin ) {
	if ( r_showMegaTextureLabels.IsModified() ) {
		r_showMegaTextureLabels.ClearModified();
		currentViewOrigin[0] = viewOrigin[0] + 0.1;	// force a change
		albedoLitMegaTextureFile->Invalidate();
	}

	if ( viewOrigin == currentViewOrigin ) {
		return;
	}
	if ( r_skipMegaTexture.GetBool() ) {
		return;
	}

	currentViewOrigin = viewOrigin;
// jmarshall
	float	texCenter[2];

	// convert the viewOrigin to a texture center, which will
	// be a different conversion for each megaTexture
	//for ( int i = 0 ; i < 2 ; i++ ) {
	//	texCenter[i] = 
	//		viewOrigin[0] * localViewToTextureCenter[i][0] +
	//		viewOrigin[1] * localViewToTextureCenter[i][1] +
	//		viewOrigin[2] * localViewToTextureCenter[i][2] +
	//		localViewToTextureCenter[i][3];
	//}

	// TexCenter should be the normalized position between the min and max based on the player position.
	idVec3 center = surfaceBounds.GetCenter();
	idVec3 megaViewOrigin = currentViewOrigin - center;
	idBounds megaBounds;
	megaBounds[0] = surfaceBounds[0] - center;
	megaBounds[1] = surfaceBounds[1] - center;

	// Normalize megaViewOrigin between megabounds.
	texCenter[0] = ((megaViewOrigin[0] - megaBounds[0][0]) / (megaBounds[1][0] - megaBounds[0][0]));
	texCenter[1] = ((megaViewOrigin[1] - megaBounds[0][1]) / (megaBounds[1][1] - megaBounds[0][1]));

	albedoLitMegaTextureFile->UpdateForCenter(texCenter);
// jmarshall end
}


/*
====================
UpdateTile

A local tile will only be mapped to globalTile[ localTile + X * TILE_PER_LEVEL ] for some x
====================
*/
void idTextureLevel::UpdateTile( int localX, int localY, int globalX, int globalY ) {
	idTextureTile	*tile = &tileMap[localX][localY];

	if ( tile->x == globalX && tile->y == globalY ) {
		return;
	}
	if ( (globalX & (TILE_PER_LEVEL-1)) != localX || (globalY & (TILE_PER_LEVEL-1)) != localY ) {
		common->Error( "idTextureLevel::UpdateTile: bad coordinate mod" );
	}

	tile->x = globalX;
	tile->y = globalY;

	byte	data[ TILE_SIZE * TILE_SIZE ];

	if ( globalX >= tilesWide || globalX < 0 || globalY >= tilesHigh || globalY < 0 ) {
		// off the map
		memset( data, 0, sizeof( data ) );
	} else {
		// extract the data from the full image (FIXME: background load from disk)
		int		tileNum = tileOffset + tile->y * tilesWide + tile->x;

		mega->ReadTile(data, tileNum);		
	}

	if ( idMegaTexture::r_showMegaTextureLabels.GetBool() ) {
		// put a color marker in it
		byte	color[4] = { 255 * localX / TILE_PER_LEVEL, 255 * localY / TILE_PER_LEVEL, 0, 0 };
		for ( int x = 0 ; x < 8 ; x++ ) {
			for ( int y = 0 ; y < 8 ; y++ ) {
				*(int *)&data[ ( ( y + TILE_SIZE/2 - 4 ) * TILE_SIZE + x + TILE_SIZE/2 - 4 ) ] = *(int *)color;
			}
		}
	}

	// upload all the mip-map levels
	int	level = 0;
	int size = TILE_SIZE;
	glCompressedTexSubImage2D(GL_TEXTURE_2D, level, localX * size, localY * size, size, size, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, size * size, data);
}

/*
====================
UpdateForCenter

Center is in the 0.0 to 1.0 range
====================
*/
void idTextureLevel::UpdateForCenter( float center[2] ) {
	int		globalTileCorner[2];
	int		localTileOffset[2];

	if ( tilesWide <= TILE_PER_LEVEL && tilesHigh <= TILE_PER_LEVEL ) {
		globalTileCorner[0] = 0;
		globalTileCorner[1] = 0;
		localTileOffset[0] = 0;
		localTileOffset[1] = 0;
		// orient the mask so that it doesn't mask anything at all
		parms[0] = 0.25;
		parms[1] = 0.25;
		parms[3] = 0.25;
	} else {
		for ( int i = 0 ; i < 2 ; i++ ) {
			float	global[2];

			// this value will be outside the 0.0 to 1.0 range unless
			// we are in the corner of the megaTexture
			global[i] = ( center[i] * parms[3] - 0.5 ) * TILE_PER_LEVEL;

			globalTileCorner[i] = (int)( global[i] + 0.5 );

			localTileOffset[i] = globalTileCorner[i] & (TILE_PER_LEVEL-1);

			// scaling for the mask texture to only allow the proper window
			// of tiles to show through
			parms[i] = -globalTileCorner[i] / (float)TILE_PER_LEVEL;
		}
	}

	image->Bind();

	for ( int x = 0 ; x < TILE_PER_LEVEL ; x++ ) {
		for ( int y = 0 ; y < TILE_PER_LEVEL ; y++ ) {
			int		globalTile[2];

			globalTile[0] = globalTileCorner[0] + ( ( x - localTileOffset[0] ) & (TILE_PER_LEVEL-1) );
			globalTile[1] = globalTileCorner[1] + ( ( y - localTileOffset[1] ) & (TILE_PER_LEVEL-1) );

			UpdateTile( x, y, globalTile[0], globalTile[1] );
		}
	}
}

/*
=====================
Invalidate

Forces all tiles to be regenerated
=====================
*/
void idTextureLevel::Invalidate() {
	for ( int x = 0 ; x < TILE_PER_LEVEL ; x++ ) {
		for ( int y = 0 ; y < TILE_PER_LEVEL ; y++ ) {
			tileMap[x][y].x =
			tileMap[x][y].y = -99999;
		}
	}
}
