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

#include "tools_precompiled.h"
#pragma hdrstop

#include "qe3.h"
#include "radiant.h"
#include "MainFrm.h"
#include "lightdlg.h"

#include "../../sys/win32/rc/doom_resource.h"
#include "../radiant.net/Shared/MenuCommands.cs" // RadiantNetMenuCommands_t

#include <process.h>    // for _beginthreadex and _endthreadex
#include <ddeml.h>  // for MSGF_DDEMGR

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

idCVar radiant_entityMode( "radiant_entityMode", "0", CVAR_TOOL | CVAR_ARCHIVE, "" );

INT_PTR g_radiant_net_dll = 0;

RadiantNetInterface_t radiantNetInterface;

bool g_radiantNetInit = false;

// jmarshall
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
// jmarshall end

/////////////////////////////////////////////////////////////////////////////
// CRadiantApp

BEGIN_MESSAGE_MAP(CRadiantApp, CWinApp)
	//{{AFX_MSG_MAP(CRadiantApp)
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadiantApp construction

CRadiantApp::CRadiantApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRadiantApp object

CRadiantApp theApp;
HINSTANCE g_DoomInstance = NULL;
bool g_editorAlive = false;

void RadiantPrint( const char *text ) {
	if ( g_editorAlive && g_Inspectors ) {
		if (g_Inspectors->consoleWnd.GetSafeHwnd()) {
			g_Inspectors->consoleWnd.AddText( text );
		}
	}
}

void RadiantShutdown( void ) {
	theApp.ExitInstance();
}

// jmarshall - radiant.net
bool isXYMouseDownRight = false;
bool isXYMouseDownLeft = false;

void RadiantAPI_CreateEntity(int x, int y, char *name)
{
	void CreateRightClickEntity(CXYWnd *pWnd, int x, int y, char *pName);
	CreateRightClickEntity(g_pParentWnd->GetXYWnd(), x, y, name);
}

const char *RadiantAPI_GetEntityClassList(void) {
	static idStr classList;

	classList = "";

	eclass_t	*e;
	for (e = eclass; e; e = e->next) {
		classList += e->name;
		classList += ";";
	}

	return classList.c_str();
}

void RadiantAPI_MouseMove(int x, int y)
{
	CPoint pt;
	pt.x = x;
	pt.y = y;

	int button = 0;
	
	if (isXYMouseDownLeft)
	{
		button = MK_LBUTTON;
	}
	else if (isXYMouseDownRight)
	{
		button = MK_RBUTTON;
	}

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		button |= MK_CONTROL;
	}
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		button |= MK_SHIFT;
	}

	g_pParentWnd->GetXYWnd()->OnMouseMove(button, pt);
}

void RadiantAPI_XYMouseRight(bool down, int x, int y)
{
	CPoint pt;
	pt.x = x;
	pt.y = y;
	isXYMouseDownRight = down;
	if (down)
	{
		g_pParentWnd->GetXYWnd()->OnRButtonDown(MK_RBUTTON, pt);
	}
	else
	{
		g_pParentWnd->GetXYWnd()->OnRButtonUp(MK_RBUTTON, pt);
	}
}

void RadiantAPI_XYMouseLeft(bool down, int x, int y)
{
	CPoint pt;
	pt.x = x;
	pt.y = y;
	isXYMouseDownLeft = down;
	int button = MK_LBUTTON;
	
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		button |= MK_CONTROL;
	}
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		button |= MK_SHIFT;
	}

	if (down)
	{
		g_pParentWnd->GetXYWnd()->OnLButtonDown(button, pt);
	}
	else
	{
		g_pParentWnd->GetXYWnd()->OnLButtonUp(button, pt);
	}
}

void RadiantAPI_TexWndMouseLeft(bool down, int x, int y)
{
	CPoint pt;
	pt.x = x;
	pt.y = y;
	if (g_Inspectors)
	{
		if (down)
		{
			g_Inspectors->texWnd.OnLButtonDown(MK_LBUTTON, pt);
		}
		else
		{
			g_Inspectors->texWnd.OnLButtonUp(MK_LBUTTON, pt);
		}
	}
}

void RadiantAPI_CameraMouseLeft(bool down, int x, int y)
{
	CPoint pt;
	pt.x = x;
	pt.y = y;
	isXYMouseDownLeft = down;
	int button = MK_LBUTTON;

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		button |= MK_CONTROL;
	}
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		button |= MK_SHIFT;
	}

	if (down)
	{
		g_pParentWnd->GetCamera()->OriginalMouseDown(button, pt);
	}
	else
	{
		g_pParentWnd->GetCamera()->OriginalMouseUp(button, pt);
	}
}


void RadiantAPI_RedrawWindows(void)
{
	if (g_pParentWnd->GetCamera())
	{
		g_pParentWnd->GetCamera()->RenderCamera();
	}

	if (g_pParentWnd->GetXYWnd())
	{
		g_pParentWnd->GetXYWnd()->Render();
	}

	if (g_Inspectors)
	{
		g_Inspectors->texWnd.OnRender();
	}
}

void RadiantAPI_GotoEntity(const char *name)
{
	entity_t *ent = FindEntity("name", name);
	if (ent) {
		idVec3 origin = ent->epairs.GetVector("origin");
		g_pParentWnd->GetCamera()->SetView(origin);
		g_pParentWnd->GetCamera()->RenderCamera();
		g_pParentWnd->GetXYWnd()->RedrawWindow();
	}
}

void RadiantAPI_ShowDoom(bool doomVisible)  {
	common->SetEditorRunning(!doomVisible);
	Sys_GrabMouseCursor(doomVisible);
	win32.activeApp = doomVisible;

	//if(!qwglMakeCurrent(radiantNetInterface.GetGameWindowHandle(), win32.hGLRC)) {
	//	common->FatalError("Failed to show doom, qwglMakeCurrent failed!\n");
	//}
}

void RadiantAPI_SelectObject(const char *name)
{
	Select_Deselect();

	entity_t *ent = FindEntity("name", name);
	if (ent) {
		Select_Brush(ent->brushes.onext, true, false);
	}
}

void RadiantAPI_SetRenderMode(bool render)
{
	g_pParentWnd->GetCamera()->SetRenderMode(render);
	g_pParentWnd->GetCamera()->RenderCamera();
}

void RadiantAPI_GameWindowMouseKey(bool rightButton, bool leftButton, bool down) {
	int value = 0;
	if(down) {
		value = 1; 
	}

	if (leftButton) {
		Sys_QueEvent(Sys_Milliseconds(), SE_KEY, K_MOUSE1, value, 0, NULL);
		usercmdGen->SetMouseButtonState(true, false, down);
	}
	else {
		Sys_QueEvent(Sys_Milliseconds(), SE_KEY, K_MOUSE2, value, 0, NULL);
		usercmdGen->SetMouseButtonState(false, true, down);
	}
}

// The input system isn't great, we have to send down three different events?
void RadiantAPI_GameWindowKeyboard(unsigned char ch, bool down) {
	if (ch == 192) {
		ch = Sys_GetConsoleKey(false);
	}

	if (down) {
		Sys_QueEvent(GetTickCount(), SE_CHAR, ch, down, 0, NULL);		
	}

	Sys_QueEvent(GetTickCount(), SE_KEY, ch, down, 0, NULL);
	
	usercmdGen->SetKeyState(ch, down);
}

void RadiantAPI_GameWindowMouseMovement(int mouseX, int mouseY) {
	Sys_QueEvent(0, SE_MOUSE, mouseX, mouseY, 0, NULL);
	usercmdGen->UpdateMouseDelta(mouseX, mouseY);
}

bool RadiantAPI_GameMouseFocus(void) {
	return !win32.mouseReleased;
}

void RadiantAPI_ProcessMenuCommands(int command) {
	switch(command)
	{
		case RADIANT_NET_COMMAND_NEW:
			g_pParentWnd->OnFileNew();
			break;
		case RADIANT_NET_COMMAND_OPEN:
			g_pParentWnd->OnFileOpen();
			break;
		case RADIANT_NET_COMMAND_SAVE:
			g_pParentWnd->OnFileSave();
			break;
		case RADIANT_NET_COMMAND_SAVEAS:
			g_pParentWnd->OnFileSaveas();
			break;
		case RADIANT_NET_COMMAND_SAVESELECTION:
			break;
		case RADIANT_NET_COMMAND_SAVEREGION:
			g_pParentWnd->OnFileSaveregion();
			break;
		case RADIANT_NET_COMMAND_NEWPROJECT:
			g_pParentWnd->OnFileNewproject();
			break;
		case RADIANT_NET_COMMAND_LOADPROJECT:
			g_pParentWnd->OnFileLoadproject();
			break;
		case RADIANT_NET_COMMAND_PROJECTSETTINGS:
			g_pParentWnd->OnFileProjectsettings();
			break;
		case RADIANT_NET_COMMAND_POINTFILE:
			g_pParentWnd->OnFilePointfile();
			break;
		case RADIANT_NET_COMMAND_EXIT:
			g_pParentWnd->OnFileExit();
			break;
		default:
			common->FatalError("RadiantAPI_ProcessMenuCommands: Unknown command %d\n", command);
			break;
	}
}

void RadaintAPI_KeyEvent(char key, bool down)
{
	if (down) {
		switch (key)
		{
			case VK_UP:
				g_pParentWnd->OnCameraForward();
				break;

			case VK_DOWN:
				g_pParentWnd->OnCameraBack();
				break;

			case VK_LEFT:
				g_pParentWnd->OnCameraLeft();
				break;

			case VK_RIGHT:
				g_pParentWnd->OnCameraRight();
				break;

			case 'D':
				g_pParentWnd->OnCameraUp();
				break;
			case 'C':
				g_pParentWnd->OnCameraDown();
				break;
			case 'A':
				g_pParentWnd->OnCameraAngleup();
				break;
			case 'Z':
				g_pParentWnd->OnCameraAngledown();
				break;
		}
	}
	g_pParentWnd->HandleKey(key, 0, 0, down);
}

void RadiantUpdateLevelEntities(void) {
	idStr entityList;
	for (entity_t* pEntity = entities.next; pEntity != &entities; pEntity = pEntity->next) {
		if (pEntity->brushes.onext == &pEntity->brushes) {
			continue;
		}

		entityList += va("%s;",pEntity->epairs.GetString("name"));
	}
	radiantNetInterface.UpdateLevelEntities(entityList.c_str());
}

void RadiantGetGameWindowSize(int &width, int &height) {
	width = radiantNetInterface.GetGameWindowWidth();
	height = radiantNetInterface.GetGameWindowHeight();
}

void RadiantDotNetInit(void) {

	common->Printf("Initializing Radiant.NET...\n");
	
	// Load in the Radiant.Net.Dll
	g_radiant_net_dll = sys->DLL_Load("radiant.net.dll");
	if (g_radiant_net_dll == 0)
	{
		common->FatalError("Failed to load the radiant.net dll!");
		return;
	}

	// Get all the functions 
	radiantNetInterface.InitRadiant = (void(__cdecl *)(void *))sys->DLL_GetProcAddress(g_radiant_net_dll, "InitRadiant");
	radiantNetInterface.UpdateLevelEntities = (void(__cdecl*)(const char*))sys->DLL_GetProcAddress(g_radiant_net_dll, "UpdateLevelEntities");
	radiantNetInterface.ToggleProgressDialog = (void(__cdecl *)(bool, const char *, const char *, float))sys->DLL_GetProcAddress(g_radiant_net_dll, "ToggleProgressDialog");
	radiantNetInterface.ProgressBarUserCancaled = (bool(__cdecl *)(void))sys->DLL_GetProcAddress(g_radiant_net_dll, "ProgressBarUserCancaled");
	radiantNetInterface.RedrawCameraWindow = (void(__cdecl *)(void))sys->DLL_GetProcAddress(g_radiant_net_dll, "RedrawCameraWindow");

	radiantNetInterface.GetCameraWindowHandle = (HDC(__cdecl *)(void))sys->DLL_GetProcAddress(g_radiant_net_dll, "GetCameraWindowHandle");
	radiantNetInterface.GetCameraDialogHandle = (HWND(__cdecl *)(void))sys->DLL_GetProcAddress(g_radiant_net_dll, "GetCameraDialogHandle");

	radiantNetInterface.GetXYWindowHandle = (HDC(__cdecl *)(void))sys->DLL_GetProcAddress(g_radiant_net_dll, "GetXYWindowHandle");
	radiantNetInterface.GetXYDialogHandle = (HWND(__cdecl *)(void))sys->DLL_GetProcAddress(g_radiant_net_dll, "GetXYDialogHandle");
	
	radiantNetInterface.GetGameWindowHandle = (HDC(__cdecl*)(void))sys->DLL_GetProcAddress(g_radiant_net_dll, "GetGameWindowHandle");

	radiantNetInterface.GetTexWndHandle = (HDC(__cdecl *)(void))sys->DLL_GetProcAddress(g_radiant_net_dll, "GetTexWndHandle");
	radiantNetInterface.SetMapName = (void(__cdecl*)(const char *))sys->DLL_GetProcAddress(g_radiant_net_dll, "SetMapName");

	radiantNetInterface.GetGameWindowWidth = (int(__cdecl*)(void))sys->DLL_GetProcAddress(g_radiant_net_dll, "GetGameWindowWidth");
	radiantNetInterface.GetGameWindowHeight = (int(__cdecl*)(void))sys->DLL_GetProcAddress(g_radiant_net_dll, "GetGameWindowHeight");

	radiantNetInterface.GetGameWindowHWND = (HWND(__cdecl*)(void))sys->DLL_GetProcAddress(g_radiant_net_dll, "GetGameWindowHWND");
	

	HWND hwnd = g_pParentWnd->GetSafeHwnd();
	radiantNetInterface.InitRadiant((void *)hwnd);

	win32.gameEditorHWND = radiantNetInterface.GetGameWindowHWND();

	g_radiantNetInit = true;
}
// jmarshall end

/*
=================
RadiantInit

This is also called when you 'quit' in doom
=================
*/
void RadiantInit( void ) {

	// make sure the renderer is initialized
	if ( !renderSystem->IsOpenGLRunning() ) {
		common->Printf( "no OpenGL running\n" );
		return;
	}

	g_editorAlive = true;

	// allocate a renderWorld and a soundWorld
	if ( g_qeglobals.rw == NULL ) {
		g_qeglobals.rw = renderSystem->AllocRenderWorld();
		g_qeglobals.rw->InitFromMap( NULL );
	}
	if ( g_qeglobals.sw == NULL ) {
		g_qeglobals.sw = soundSystem->AllocSoundWorld( g_qeglobals.rw );
	}

	if ( g_DoomInstance ) {
		if ( ::IsWindowVisible( win32.hWnd ) ) {
			::ShowWindow( win32.hWnd, SW_HIDE );
			g_pParentWnd->ShowWindow( SW_SHOW );
			g_pParentWnd->SetFocus();
		}
	} else {
		Sys_GrabMouseCursor( false );

		g_DoomInstance = win32.hInstance;
		CWinApp* pApp = AfxGetApp();
		CWinThread *pThread = AfxGetThread();

		InitAfx();

		// App global initializations (rare)
		pApp->InitApplication();

		// Perform specific initializations
		pThread->InitInstance();

		// Setup the game proxy window.
		{
			HDC hDC = radiantNetInterface.GetGameWindowHandle();

			QEW_SetupPixelFormat(hDC, true);
		}

		glFinish();
		//qwglMakeCurrent(0, 0);
		//qwglMakeCurrent(win32.hDC, win32.hGLRC);

		// hide the doom window by default
		::ShowWindow( win32.hWnd, SW_HIDE );
	}
}


extern void Map_VerifyCurrentMap(const char *map);

void RadiantSync( const char *mapName, const idVec3 &viewOrg, const idAngles &viewAngles ) {
	if ( g_DoomInstance == NULL ) {
		RadiantInit();
	}

	if ( g_DoomInstance ) {
		idStr osPath;
		osPath = fileSystem->RelativePathToOSPath( mapName );
		Map_VerifyCurrentMap( osPath );
		idAngles flip = viewAngles;
		flip.pitch = -flip.pitch;
		g_pParentWnd->GetCamera()->SetView( viewOrg, flip );
		g_pParentWnd->SetFocus();
		Sys_UpdateWindows( W_ALL );
		g_pParentWnd->RoutineProcessing();
	}
}

void RadiantRun( void ) {
	static bool exceptionErr = false;
	//int show = common->IsEditorRunning();//::IsWindowVisible(win32.hWnd);

	try {
		if (!exceptionErr && !common->IsEditorGameRunning()) {
			glUseProgram(0);
			//glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDepthMask(true);
			theApp.Run();
			//glPopAttrib();
			//qwglMakeCurrent(0, 0);
			//qwglMakeCurrent(win32.hDC, win32.hGLRC);
		}
	}
	catch( idException &ex ) {
		::MessageBox(NULL, ex.error, "Exception error", MB_OK);
		RadiantShutdown();
	}
}

void RadiantSetGameContext( void ) {
	GetLastError();
	HDC hdc = radiantNetInterface.GetGameWindowHandle();
	if (!qwglMakeCurrent(hdc, win32.hGLRC)) {
		common->Warning("Failed to update window context %d!", GetLastError());
	}
}

void RadiantSwapBuffer(void) {
	if(g_radiantNetInit == false) {
		qwglSwapBuffers(win32.hDC);
	}
	else {
		HDC hdc = radiantNetInterface.GetGameWindowHandle();
		qwglSwapBuffers(hdc);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRadiantApp initialization

HINSTANCE g_hOpenGL32 = NULL;
HINSTANCE g_hOpenGL = NULL;
bool g_bBuildList = false;

BOOL CRadiantApp::InitInstance()
{
  //g_hOpenGL32 = ::LoadLibrary("opengl32.dll");
	// AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
  //AfxEnableMemoryTracking(FALSE);

#ifdef _AFXDLL
	//Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	//Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// If there's a .INI file in the directory use it instead of registry

	char RadiantPath[_MAX_PATH];
	GetModuleFileName( NULL, RadiantPath, _MAX_PATH );

	// search for exe
	CFileFind Finder;
	Finder.FindFile( RadiantPath );
	Finder.FindNextFile();
	// extract root
	CString Root = Finder.GetRoot();
	// build root\*.ini
	CString IniPath = Root + "\\REGISTRY.INI";
	// search for ini file
	Finder.FindNextFile();
	if (Finder.FindFile( IniPath ))
	{
		Finder.FindNextFile();
		// use the .ini file instead of the registry
		free((void*)m_pszProfileName);
		m_pszProfileName=_tcsdup(_T(Finder.GetFilePath()));
		// look for the registry key for void* buffers storage ( these can't go into .INI files )
		int i=0;
		CString key;
		HKEY hkResult;
		DWORD dwDisp;
		DWORD type;
		char iBuf[3];
		do
		{
			sprintf( iBuf, "%d", i );
			key = "Software\\Q3Radiant\\IniPrefs" + CString(iBuf);
			// does this key exists ?
			if ( RegOpenKeyEx( HKEY_CURRENT_USER, key, 0, KEY_ALL_ACCESS, &hkResult ) != ERROR_SUCCESS )
			{
				// this key doesn't exist, so it's the one we'll use
				strcpy( g_qeglobals.use_ini_registry, key.GetBuffer(0) );
				RegCreateKeyEx( HKEY_CURRENT_USER, key, 0, NULL, 
					REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisp );
				RegSetValueEx( hkResult, "RadiantName", 0, REG_SZ, reinterpret_cast<CONST BYTE *>(RadiantPath), strlen( RadiantPath )+1 );
				RegCloseKey( hkResult );
				break;
			}
			else
			{
				char RadiantAux[ _MAX_PATH ];
				unsigned long size = _MAX_PATH;
				// the key exists, is it the one we are looking for ?
				RegQueryValueEx( hkResult, "RadiantName", 0, &type, reinterpret_cast<BYTE *>(RadiantAux), &size );
				RegCloseKey( hkResult );
				if ( !strcmp( RadiantAux, RadiantPath ) )
				{
					// got it !
					strcpy( g_qeglobals.use_ini_registry, key.GetBuffer(0) );
					break;
				}
			}
			i++;
		} while (1);
		g_qeglobals.use_ini = true;
	}
	else
	{
		// Change the registry key under which our settings are stored.
		SetRegistryKey( EDITOR_REGISTRY_KEY );
		g_qeglobals.use_ini = false;
	}

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

//	CMultiDocTemplate* pDocTemplate;
//	pDocTemplate = new CMultiDocTemplate(
//		IDR_RADIANTYPE,
//		RUNTIME_CLASS(CRadiantDoc),
//		RUNTIME_CLASS(CMainFrame), // custom MDI child frame
//		RUNTIME_CLASS(CRadiantView));
//	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window

	g_PrefsDlg.LoadPrefs();

	glEnableClientState( GL_VERTEX_ARRAY );

	CString strTemp = m_lpCmdLine;
	strTemp.MakeLower();
	if (strTemp.Find("builddefs") >= 0) {
		g_bBuildList = true;
	}

	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(NULL, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL)) {
		return FALSE;
	}

	if (pMainFrame->m_hAccelTable) {
		::DestroyAcceleratorTable(pMainFrame->m_hAccelTable);
	}
  
	pMainFrame->LoadAccelTable(MAKEINTRESOURCE(IDR_MINIACCEL));

	m_pMainWnd = pMainFrame;

	// The main window has been initialized, so show and update it.
// jmarshall - editor will always start maximized
	pMainFrame->ShowWindow(SW_MAXIMIZE);
	pMainFrame->SetIcon(LoadIcon(MAKEINTRESOURCE(IDI_ICON1)), false);
// jmarshall end
	pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CRadiantApp commands

int CRadiantApp::ExitInstance() 
{
	common->Shutdown();
	g_pParentWnd = NULL;
	int ret = CWinApp::ExitInstance();
	ExitProcess(0);
	return ret;
}


BOOL CRadiantApp::OnIdle(LONG lCount) {
	if (g_pParentWnd) {
		g_pParentWnd->RoutineProcessing();
	}
	return FALSE;
	//return CWinApp::OnIdle(lCount);
}

void CRadiantApp::OnHelp()
{
	ShellExecute(m_pMainWnd->GetSafeHwnd(), "open", "http://www.idDevNet.com", NULL, NULL, SW_SHOW);
}

int CRadiantApp::Run( void ) 
{
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;


#if _MSC_VER >= 1300
	MSG *msg = AfxGetCurrentMessage();			// TODO Robert fix me!!
#else
	MSG *msg = &m_msgCur;
#endif

	// phase1: check to see if we can do idle work
	while (bIdle &&	!::PeekMessage(msg, NULL, NULL, NULL, PM_NOREMOVE)) {
		// call OnIdle while in bIdle state
		if (!OnIdle(lIdleCount++)) {
			bIdle = FALSE; // assume "no idle" state
		}
	}

	// phase2: pump messages while available
	do {
		// pump message, but quit on WM_QUIT
		if (!PumpMessage()) {
			return ExitInstance();
		}

		// reset "no idle" state after pumping "normal" message
		if (IsIdleMessage(msg)) {
			bIdle = TRUE;
			lIdleCount = 0;
		}

	} while (::PeekMessage(msg, NULL, NULL, NULL, PM_NOREMOVE));

	return 0;
}


/*
=============================================================

REGISTRY INFO

=============================================================
*/

bool SaveRegistryInfo(const char *pszName, void *pvBuf, long lSize)
{
	SetCvarBinary(pszName, pvBuf, lSize);
	common->WriteFlaggedCVarsToFile( "editor.cfg", CVAR_TOOL, "sett" );
	return true;
}

bool LoadRegistryInfo(const char *pszName, void *pvBuf, long *plSize)
{
	return GetCvarBinary(pszName, pvBuf, *plSize);
}

bool SaveWindowState(HWND hWnd, const char *pszName)
{
	RECT rc;
	GetWindowRect(hWnd, &rc);
	if (hWnd != g_pParentWnd->GetSafeHwnd()) {
	    if (::GetParent(hWnd) != g_pParentWnd->GetSafeHwnd()) {
	      ::SetParent(hWnd, g_pParentWnd->GetSafeHwnd());
	    }
		MapWindowPoints(NULL, g_pParentWnd->GetSafeHwnd(), (POINT *)&rc, 2);
	}
	return SaveRegistryInfo(pszName, &rc, sizeof(rc));
}


bool LoadWindowState(HWND hWnd, const char *pszName)
{
	RECT rc;
	LONG lSize = sizeof(rc);

	if (LoadRegistryInfo(pszName, &rc, &lSize))
	{
		if (rc.left < 0)
			rc.left = 0;
		if (rc.top < 0)
			rc.top = 0;
		if (rc.right < rc.left + 16)
			rc.right = rc.left + 16;
		if (rc.bottom < rc.top + 16)
			rc.bottom = rc.top + 16;

		MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
		return true;
	}

	return false;
}

/*
===============================================================

  STATUS WINDOW

===============================================================
*/

void Sys_UpdateStatusBar( void )
{
	extern int   g_numbrushes, g_numentities;

	char numbrushbuffer[100] = "";

	sprintf( numbrushbuffer, "Brushes: %d Entities: %d", g_numbrushes, g_numentities );
	Sys_Status( numbrushbuffer, 2 );
}

void Sys_Status(const char *psz, int part )
{
	if ( part < 0 ) {
		common->Printf("%s", psz);
		part = 0;
	}
	g_pParentWnd->SetStatusText(part, psz);
}
