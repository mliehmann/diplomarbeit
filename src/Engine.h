/*
Ar.P. - Artificial Planetoids
Copyright (c) 2003-2004 Markus Liehmann

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*
	Engine.h

	Author: Markus Liehmann
	Date: 02/08/2004
	Description: The CEngine class initializes SDL (by creating a COpenGLWindow-object) and
		SDL_mixer. It handles messages and distributes them among the engine. It also holds the
		messageloop, the virtual functions OnPrepare, OnGetCamera, OnGetWorld, ProceedMessage
		and GameCycle and the virtual functions for different messages.
*/

#ifndef __MM_ENGINE_H__
#define __MM_ENGINE_H__

#include <SDL.h>
#include <SDL_mixer.h>
#include "world.h"
#include "camera.h"
#include "OpenGLWindow.h"
#include "ConfigFile.h"

class CEngine
{
private:
	void ReadConfig();

public:
	CEngine();
	CEngine(const char *szFileName);
	~CEngine();

	void SetupEngine(const char* strFileName);
	void SetupScreen(const char* strName, bool fscreen, int w, int h, int b);
	void EnterMessageLoop();

protected:
	// virtual functions for message handling

	virtual void OnSetup() { }						// engine setup
	virtual void OnSize() { }
	virtual void OnFocus(int gain) { }
	virtual void OnMouseDownL(float x, float y) { }
	virtual void OnMouseDownR(float x, float y) { }
	virtual void OnMouseDownM(float x, float y) { }
	virtual void OnMouseUpL(float x, float y) { }
	virtual void OnMouseUpR(float x, float y) { }
	virtual void OnMouseUpM(float x, float y) { }
	virtual void OnMouseMove(int x, int y, int centerX, int centerY) { }
	virtual void OnMouseMove(int deltaX, int deltaY) { }
	virtual void OnMouseDragL(int x, int y, int dx, int dy) { }
	virtual void OnMouseDragR(int x, int y, int dx, int dy) { }
	virtual void OnKeyUp(SDLKey key) { }
	virtual void OnKeyDown(SDLKey key) { }
	virtual void OnChar(char c) { }

	virtual void GameCycle(float deltaTime);

	virtual void OnPrepare() { }					// setup OpenGL for frame
	virtual CCamera *OnGetCamera() { return NULL; }	// override in derived engine
	virtual CWorld *OnGetWorld() { return NULL; }

	virtual int ProceedMessage(SDL_Event* event, float deltaTime);

	void WriteLog(const char* str);

	COpenGLWindow* m_pOpenGLWindow;
	CConfigFile* m_pConfigFile;
	int m_iWidth;
	int m_iHeight;
	int m_iBits;
	int m_iAspect;
	bool m_bFullscreen;
	bool m_bShowFPS;
	int m_iFrames;
	float m_fMouseSensitivity;
	int m_iInvertMouse;				// 1: normal, -1: inverted
	bool m_baKeys[256];				// array may be used by derived engine to check keyboard-input
	bool m_bStereo;
	int m_iSoundFrequency;
	int m_iSoundChannels;
	int m_iSoundBufferSize;
	float m_fTimeCount;				// used for FPS
};

#endif