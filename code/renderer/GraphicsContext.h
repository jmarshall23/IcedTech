// GraphicsContext.h
//

#pragma once

extern idCVar r_debugScopedContextSwitch;

//
// rvmScopedGraphicsContext
//
class rvmScopedGraphicsContext {
public:
	rvmScopedGraphicsContext(HDC hdc) {
		idTimer *timer;

		contextHDC = hdc;

		if (r_debugScopedContextSwitch.GetBool())
		{
			timer = new idTimer();
			timer->Start();
		}
		isContextValid = qwglMakeCurrent(hdc, win32.hGLRC);

		if (r_debugScopedContextSwitch.GetBool())
		{
			timer->Stop();

			common->Printf("wglMakeCurrent took %fms\n", timer->Milliseconds());

			delete timer;
		}
	}

	~rvmScopedGraphicsContext() {
		if (IsContextValid())
		{
			glFinish();
			qwglSwapBuffers(contextHDC);
			qwglMakeCurrent(NULL, NULL);
		}
	}

	// Returns true if make current succeeded.
	bool IsContextValid() const { return isContextValid; }
private:
	bool isContextValid;
	HDC contextHDC;
};