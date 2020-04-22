// tr_qgl.h
//

#pragma once

#include "glew.h"

extern int   (WINAPI * qwglChoosePixelFormat)(HDC, CONST PIXELFORMATDESCRIPTOR *);
extern int   (WINAPI * qwglDescribePixelFormat) (HDC, int, UINT, LPPIXELFORMATDESCRIPTOR);
extern int   (WINAPI * qwglGetPixelFormat)(HDC);
extern BOOL(WINAPI * qwglSetPixelFormat)(HDC, int, CONST PIXELFORMATDESCRIPTOR *);
extern BOOL(WINAPI * qwglSwapBuffers)(HDC);
extern BOOL(WINAPI * qwglCopyContext)(HGLRC, HGLRC, UINT);
extern HGLRC(WINAPI * qwglCreateContext)(HDC);
extern HGLRC(WINAPI * qwglCreateLayerContext)(HDC, int);
extern BOOL(WINAPI * qwglDeleteContext)(HGLRC);
extern HGLRC(WINAPI * qwglGetCurrentContext)(VOID);
extern HDC(WINAPI * qwglGetCurrentDC)(VOID);
extern PROC(WINAPI * qwglGetProcAddress)(LPCSTR);
extern BOOL(WINAPI * qwglMakeCurrent)(HDC, HGLRC);
extern BOOL(WINAPI * qwglShareLists)(HGLRC, HGLRC);
extern BOOL(WINAPI * qwglUseFontBitmaps)(HDC, DWORD, DWORD, DWORD);
extern BOOL(WINAPI * qwglUseFontOutlines)(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT);
extern BOOL(WINAPI * qwglDescribeLayerPlane)(HDC, int, int, UINT, LPLAYERPLANEDESCRIPTOR);
extern int  (WINAPI * qwglSetLayerPaletteEntries)(HDC, int, int, int, CONST COLORREF *);
extern int  (WINAPI * qwglGetLayerPaletteEntries)(HDC, int, int, int, COLORREF *);
extern BOOL(WINAPI * qwglRealizeLayerPalette)(HDC, int, BOOL);
extern BOOL(WINAPI * qwglSwapLayerBuffers)(HDC, UINT);