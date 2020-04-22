// Common_loadscreen.cpp
//

#include "Engine_precompiled.h"
#include "Common_local.h"
#include "../sys/win32/win_local.h"

idSysMutex		com_loadScreenMutex;
bool			com_loadScreenActive = false;
bool			com_loadthreadCleanedup = false;

/*
======================
rvmScopedLoadContext::rvmScopedLoadContext
======================
*/
rvmScopedLoadContext::rvmScopedLoadContext() {
	if(!com_loadScreenActive) {
		contextSwitched = false;
		return;
	}
	contextSwitched = true;

#ifdef ID_ALLOW_TOOLS
	if(com_editors & EDITOR_RADIANT) {
		RadiantSetGameContext();
		return;
	}
#endif
	wglMakeCurrent(win32.hDC, win32.hGLRC);
}

/*
======================
rvmScopedLoadContext::~rvmScopedLoadContext
======================
*/
rvmScopedLoadContext::~rvmScopedLoadContext() {
	if (!contextSwitched) {
		return;
	}
	wglMakeCurrent(NULL, NULL);
}

/*
======================
Com_LoadScreenJob
======================
*/
int Com_LoadScreenJob(void *data) {
	com_loadthreadCleanedup = false;

	while (com_loadScreenActive)
	{
		{
			idScopedCriticalSection lock(com_loadScreenMutex);
			rvmScopedLoadContext scopedContext;

			session->PacifierUpdate();
		}
		Sys_Sleep(16); // Refresh every 16ms.
	}

	wglMakeCurrent(NULL, NULL);
	com_loadthreadCleanedup = true;
	return 0;
}

/*
======================
idCommonLocal::BeginLoadScreen
======================
*/
void idCommonLocal::BeginLoadScreen(void) {
	wglMakeCurrent(NULL, NULL);
	com_loadScreenActive = true;
	com_loadthreadCleanedup = false;

	Sys_CreateThread((xthread_t)Com_LoadScreenJob, NULL, THREAD_NORMAL, "LoadScreen", CORE_ANY);
}

/*
======================
idCommonLocal::EndLoadScreen
======================
*/
void idCommonLocal::EndLoadScreen(void) {
	com_loadScreenActive = false;

	{
		while(!com_loadthreadCleanedup) {
			Sys_Sleep(16);
		}
		wglMakeCurrent(NULL, NULL);
		wglMakeCurrent(win32.hDC, win32.hGLRC);
	}	
}