// VirtualTextureTranscode.cpp
//

#include "Engine_precompiled.h"
#include "tr_local.h"

#include "DXT/DXTCodec.h"

idCVar vt_transcodeShowPages("vt_transcodeShowPages", "0", CVAR_INTEGER, "shows transcoded page information");
idCVar vt_transcodeDebug("vt_transcodeDebug", "0", CVAR_BOOL, "shows information about transcoding");

#define TRANSCODE_DEBUG_TEXTSIZE 16

/*
==============
R_CopyImage
==============
*/
void R_CopyImage(byte *source, int sourceX, int sourceY, int sourceWidth, byte *dest, int destX, int destY, int destWidth, int width, int height)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int _x = x * 4;
			int _y = y * 4;
			int destPos = (destWidth * (_y + (destY * 4))) + (_x + (destX * 4));
			int sourcePos = (sourceWidth * (_y + (sourceY * 4))) + (_x + (sourceX * 4));

			dest[destPos + 0] = source[sourcePos + 0];
			dest[destPos + 1] = source[sourcePos + 1];
			dest[destPos + 2] = source[sourcePos + 2];
			dest[destPos + 3] = source[sourcePos + 3];
		}
	}
}

/*
============================
rvmVirtualTextureSystem::DrawTranscodeText
============================
*/
void rvmVirtualTextureSystem::DrawTranscodeText(const char *msg, int x, int y, byte *transcodePage) {
	y = (VIRTUALTEXTURE_TILESIZE - TRANSCODE_DEBUG_TEXTSIZE - 1) - y;

	for (int i = 0; i < strlen(msg); i++)
	{
		int fontx = 0;
		int fonty = 0;

		if (msg[i] >= 'a' && msg[i] <= 'z')
		{
			int c = msg[i] - 97;
			fontx = c * 16;
		}

		if (msg[i] >= 'A' && msg[i] <= 'Z')
		{
			int c = msg[i] - 65;
			fontx = c * 16;
		}

		if (msg[i] >= '0' && msg[i] <= '9')
		{
			char crap[2];
			crap[0] = msg[i];
			crap[1] = 0;

			int num = 26 + atoi(crap);
			fontx = num * 16;
		}

		if (msg[i] != ' ')
		{
			R_CopyImage(transcodeFont, fontx, fonty, transcodeFontWidth, transcodePage, x, y, VIRTUALTEXTURE_TILESIZE, TRANSCODE_DEBUG_TEXTSIZE, TRANSCODE_DEBUG_TEXTSIZE);
		}

		x += TRANSCODE_DEBUG_TEXTSIZE;
	}
}

/*
============================
rvmVirtualTextureSystem::TranscodePage
============================
*/
void rvmVirtualTextureSystem::TranscodePage(idImage *image, rvmVirtualImage *virtualImage, int pageX, int pageY, int uploadX, int uploadY, int pageLOD) {
	int size = VIRTUALTEXTURE_TILESIZE;
	int pageMemSize = VIRTUALTEXTURE_TILESIZE * VIRTUALTEXTURE_TILESIZE;

	// Clamp page LOD between 0 and max mips.
	//pageLOD = 0;// idMath::ClampInt(0, virtualImage->numMips - 1, pageLOD);

	assert( pageLOD <= virtualImage->numMips && pageLOD >= 0 );

	int tilePagePosition = virtualImage->GetStartPageOffset(pageLOD) + (pageMemSize * pageY * virtualImage->GetWidthInPages(pageLOD)) + (pageMemSize * pageX);
	virtualTextureFile->Seek(tilePagePosition, FS_SEEK_SET);

	virtualTextureFile->Read(transcodePage, pageMemSize);

	if (vt_transcodeShowPages.GetInteger() > 0 && virtualImage->GetUsage() == TD_DIFFUSE)
	{
		idDxtDecoder decoder;
		byte *temp = new byte[VIRTUALTEXTURE_TILESIZE * VIRTUALTEXTURE_TILESIZE * 4];

		decoder.DecompressImageDXT5(transcodePage, temp, VIRTUALTEXTURE_TILESIZE, VIRTUALTEXTURE_TILESIZE);

		for (int d = 0; d < VIRTUALTEXTURE_TILESIZE; d++)
		{
			// Top
			{
				temp[(d * 4) + 0] = 255;
				temp[(d * 4) + 1] = 255;
				temp[(d * 4) + 2] = 0;
				temp[(d * 4) + 3] = 255;
			}
	
			// Bottom
			{
				int bottomRowPosition = (VIRTUALTEXTURE_TILESIZE * (VIRTUALTEXTURE_TILESIZE - 1) + d);
				temp[(bottomRowPosition * 4) + 0] = 255;
				temp[(bottomRowPosition * 4) + 1] = 255;
				temp[(bottomRowPosition * 4) + 2] = 0;
				temp[(bottomRowPosition * 4) + 3] = 255;
			}

			// Left
			{
				int leftRowPosition = (VIRTUALTEXTURE_TILESIZE * d);
				temp[(leftRowPosition * 4) + 0] = 255;
				temp[(leftRowPosition * 4) + 1] = 255;
				temp[(leftRowPosition * 4) + 2] = 0;
				temp[(leftRowPosition * 4) + 3] = 255;
			}

			// right
			{
				int rightRowPosition = (VIRTUALTEXTURE_TILESIZE * d) + (VIRTUALTEXTURE_TILESIZE - 1);
				temp[rightRowPosition] = 255;
				temp[(rightRowPosition * 4) + 0] = 255;
				temp[(rightRowPosition * 4) + 1] = 255;
				temp[(rightRowPosition * 4) + 2] = 0;
				temp[(rightRowPosition * 4) + 3] = 255;
			}

			if (vt_transcodeShowPages.GetInteger() == 1)
			{
				const char *text = va("%d %d", pageX, pageY);
				DrawTranscodeText(text, 0, 0, temp);
			}

			if (vt_transcodeShowPages.GetInteger() == 2)
			{
				const char *text = va("  %d %d", pageX, pageY);
				DrawTranscodeText(text, 0, 0, temp);
			}

			if (vt_transcodeShowPages.GetInteger() == 3)
			{
				const char *text2 = va("%d %d", uploadX, uploadY);
				DrawTranscodeText(text2, 0, 0, temp);
			}

			if (vt_transcodeShowPages.GetInteger() == 4)
			{
				const char *text2 = va(" %d %d", uploadX, uploadY);
				DrawTranscodeText(text2, 0, 0, temp);
			}
			
			const char *text2 = va("%d %d", pageLOD, virtualImage->GetNumMips());
			DrawTranscodeText(text2, 0, TRANSCODE_DEBUG_TEXTSIZE + 1, temp);
		}

		idDxtEncoder encoder;
		encoder.CompressImageDXT5Fast(temp, transcodePage, VIRTUALTEXTURE_TILESIZE, VIRTUALTEXTURE_TILESIZE);
		delete temp;
	}

	image->Bind();
	glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, uploadX * size, uploadY * size, size, size, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, pageMemSize, transcodePage);

	if (vt_transcodeDebug.GetBool())
	{
		common->Printf("Virtual Image %s LOD %d pages %dx%d uploaded to %dx%d image size %dx%d\n", virtualImage->GetName(), pageLOD, pageX, pageY, uploadX * size, uploadY * size, virtualImage->GetWidth(pageLOD), virtualImage->GetHeight(pageLOD));
	}
}