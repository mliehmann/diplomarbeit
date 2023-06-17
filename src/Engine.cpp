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

#include "Engine.h"

CEngine::CEngine()
{
	// default values

	m_pOpenGLWindow = NULL;
	m_pConfigFile = NULL;
	m_iAspect = 0;
	m_iBits = 0;
	m_bFullscreen = false;
	m_bShowFPS = false;
	m_iFrames = 0;
	m_fTimeCount = 0.0f;
	m_iWidth = 0;
	m_iHeight = 0;
	m_fMouseSensitivity = 0.5f;
	m_iInvertMouse = 1;
	m_iSoundChannels = 8;
	m_iSoundBufferSize = 1024;
	m_bStereo = false;
	m_iSoundFrequency = 22050;

	for (int i = 0; i < 256; i++)
	{
		m_baKeys[i] = false;
	}
}

CEngine::CEngine(const char *strFileName)
{
	// default values

	m_pOpenGLWindow = NULL;
	m_pConfigFile = NULL;
	m_iAspect = 0;
	m_iBits = 0;
	m_bFullscreen = false;
	m_bShowFPS = false;
	m_iFrames = 0;
	m_fTimeCount = 0.0f;
	m_iWidth = 0;
	m_iHeight = 0;
	m_fMouseSensitivity = 0.5f;
	m_iInvertMouse = 1;
	m_iSoundChannels = 8;
	m_iSoundBufferSize = 1024;
	m_bStereo = false;
	m_iSoundFrequency = 22050;

	for (int i = 0; i < 256; i++)
	{
		m_baKeys[i] = false;
	}

	SetupEngine(strFileName);
}

CEngine::~CEngine()
{
	Mix_CloseAudio();
	WriteLog("Mix_CloseAudio.\n");
	delete m_pOpenGLWindow;
	delete m_pConfigFile;
}

void CEngine::SetupEngine(const char* strFileName)
{
	if (m_pConfigFile != NULL)
	{
		// shut down SDL_mixer and SDL

		Mix_CloseAudio();
		WriteLog("Mix_CloseAudio.\n");
		delete m_pOpenGLWindow;
		delete m_pConfigFile;

		// set default values

		m_pOpenGLWindow = NULL;
		m_pConfigFile = NULL;
		m_iAspect = 0;
		m_iBits = 0;
		m_bFullscreen = false;
		m_iWidth = 0;
		m_iHeight = 0;
		m_fMouseSensitivity = 0.5f;
		m_iInvertMouse = 1;
		m_iSoundChannels = 8;
		m_iSoundBufferSize = 1024;
		m_bStereo = false;
		m_iSoundFrequency = 22050;

		for (int i = 0; i < 256; i++)
		{
			m_baKeys[i] = false;
		}
	}

	m_pConfigFile = new CConfigFile(strFileName);

	ReadConfig();

	if (m_iWidth > m_iHeight)
	{
		m_iAspect = m_iWidth;
	}
	else
	{
		m_iAspect = m_iHeight;
	}

	int channels = m_bStereo == true ? 2 : 1;

	// initialize SDL and set OpenGL-window

	m_pOpenGLWindow = new COpenGLWindow("ArP", m_bFullscreen, m_iWidth, m_iHeight, m_iBits);
	char str[256];

	// initialize SDL_mixer

	if (Mix_OpenAudio(m_iSoundFrequency, MIX_DEFAULT_FORMAT, channels, m_iSoundBufferSize) == 0)
	{
		sprintf(str, "Mix_OpenAudio successful.\nFrequency: %d.\nChannels: %d.\nBuffersize: %d.\n",
			m_iSoundFrequency, channels, m_iSoundBufferSize);
		WriteLog(str);
	}
	else
	{
		sprintf(str, "Mix_OpenAudio failed.\n");
		WriteLog(str);
	}

	int alloc_channels = Mix_AllocateChannels(m_iSoundChannels);

	sprintf(str, "%d mix-channels allocated.\n\n", alloc_channels);
	WriteLog(str);

	OnSetup();
}

void CEngine::SetupScreen(const char* strName, bool fscreen, int w, int h, int b)
{
	if (m_pOpenGLWindow == NULL)
	{
		// setup new window

		m_pOpenGLWindow = new COpenGLWindow(strName, fscreen, w, h, b);
		m_iBits = b;
		m_bFullscreen = fscreen;
		m_iWidth = w;
		m_iHeight = h;

		if (w > h)
		{
			m_iAspect = w;
		}
		else
		{
			m_iAspect = h;
		}
	}
	else
	{
		// delete old window and setup new window

		delete m_pOpenGLWindow;
		m_pOpenGLWindow = new COpenGLWindow(strName, fscreen, w, h, b);
		m_iBits = b;
		m_bFullscreen = fscreen;
		m_iWidth = w;
		m_iHeight = h;

		if (w > h)
		{
			m_iAspect = w;
		}
		else
		{
			m_iAspect = h;
		}
	}

	OnSize();
}

void CEngine::EnterMessageLoop()
{
	if (m_pConfigFile == NULL)
	{
		// engine not initialized

		char sz[256];
		sprintf (sz, "Cannot enter message loop, CEngine not initialized\n", SDL_GetError());
		throw sz;
	}

	SDL_Event event;
	Uint32 oldTime;
	Uint32 newTime;
	float deltaTime;

	// disable keyboard repeat

	SDL_EnableKeyRepeat(0, 0);

	// remove immediate events

	SDL_PollEvent(&event);
	SDL_PollEvent(&event);

	oldTime = SDL_GetTicks();

	for (;;)
	{
		newTime = SDL_GetTicks();
		deltaTime = (float)(newTime - oldTime);

		if (deltaTime == 0.0f)
		{
			// ignore cycles with 0 deltaTime

			continue;
		}

		deltaTime /= 1000.0f;
		GameCycle(deltaTime);
		oldTime = newTime;

		//SDL_Delay(250); // debug, simulate low framerate

		while(SDL_PollEvent(&event))
		{
			if (ProceedMessage(&event, deltaTime) == -1)
			{
				// got quit message, exit message loop

				return;
			}
		}
	}
}

void CEngine::GameCycle(float deltaTime)
{
	CCamera *camera = OnGetCamera();	// get the camera
	CWorld *world = OnGetWorld();		// get the world

	if (m_bShowFPS == true)
	{
		m_fTimeCount += deltaTime;
		m_iFrames++;

		if (m_fTimeCount > 0.5f)
		{
			// update every half second

			world->m_iFPS = (int)((float)m_iFrames / m_fTimeCount);
			m_iFrames = 0;
			m_fTimeCount = 0.0f;
		}
	}

	// setup opengl for frame (clear, identity)

	OnPrepare();

	// prepare objects

	world->Prepare();

	// move/orient camera

	camera->Animate();

	// move/orient objects

	world->Animate(deltaTime);

	// draw objects

	world->Draw(camera);

	// swap buffers

	SDL_GL_SwapBuffers();
}

int CEngine::ProceedMessage(SDL_Event* event, float deltaTime)
{
	switch(event->type)
	{
	case SDL_KEYDOWN:

		OnKeyDown(event->key.keysym.sym);
		break;

	case SDL_KEYUP:

		OnKeyUp(event->key.keysym.sym);
		break;

	case SDL_MOUSEMOTION:

		OnMouseMove(event->motion.xrel, m_iInvertMouse * event->motion.yrel);
		break;

	case SDL_MOUSEBUTTONDOWN:

		if (event->button.button == SDL_BUTTON_LEFT)
		{
			OnMouseDownL(event->button.x, event->button.y);
		}

		if (event->button.button == SDL_BUTTON_RIGHT)
		{
			OnMouseDownR(event->button.x, event->button.y);
		}

		if (event->button.button == SDL_BUTTON_MIDDLE)
		{
			OnMouseDownM(event->button.x, event->button.y);
		}

		break;

	case SDL_MOUSEBUTTONUP:

		if (event->button.button == SDL_BUTTON_LEFT)
		{
			OnMouseUpL(event->button.x, event->button.y);
		}

		if (event->button.button == SDL_BUTTON_RIGHT)
		{
			OnMouseUpR(event->button.x, event->button.y);
		}

		if (event->button.button == SDL_BUTTON_MIDDLE)
		{
			OnMouseUpM(event->button.x, event->button.y);
		}

		break;

	case SDL_ACTIVEEVENT:

		if (event->active.state == SDL_APPMOUSEFOCUS)
		{
			OnFocus(event->active.gain);
		}

		break;

	case SDL_QUIT:

		return -1;
		break;
	}

	return 0;
}

void CEngine::ReadConfig()
{
	// read config file and fill engine data members

	int result = m_pConfigFile->OpenConfig();

	if (result == 0)
	{
		char sz[256];
		sprintf (sz, "Can't load config: %s\n", m_pConfigFile->GetFilename());
		throw sz;
	}

	do
	{
		result = m_pConfigFile->ParseLine();

		if (m_pConfigFile->GetToken()[0] == 0)
		{
			// no token

			continue;
		}

		// debug
		/*FILE* debug;
		debug = fopen("debug_engine_readconfig.txt","a");
	    fprintf(debug, "Token: '%s', Value: '%s'\n", m_pConfigFile->GetToken(), m_pConfigFile->GetValue());
		fclose(debug);*/

		if (strcmp(m_pConfigFile->GetToken(), "fullscreen") == 0)
		{
			if (strcmp(m_pConfigFile->GetValue(), "true") == 0)
			{
				m_bFullscreen = true;
			}
			else
			{
				m_bFullscreen = false;
			}
		}
		else if (strcmp(m_pConfigFile->GetToken(), "bits") == 0)
		{
			m_iBits = atoi(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "width") == 0)
		{
			m_iWidth = atoi(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "height") == 0)
		{
			m_iHeight = atoi(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "mouse_sensitivity") == 0)
		{
			m_fMouseSensitivity = (float)atof(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "invert_mouse") == 0)
		{
			if (strcmp(m_pConfigFile->GetValue(), "true") == 0)
			{
				m_iInvertMouse = -1;
			}
			else
			{
				m_iInvertMouse = 1;
			}
		}
		else if (strcmp(m_pConfigFile->GetToken(), "sound_frequency") == 0)
		{
			m_iSoundFrequency = atoi(m_pConfigFile->GetValue());

			if (m_iSoundFrequency <= 0)
			{
				m_iSoundFrequency = 22050;
			}
		}
		else if (strcmp(m_pConfigFile->GetToken(), "sound_channels") == 0)
		{
			m_iSoundChannels = atoi(m_pConfigFile->GetValue());

			if (m_iSoundChannels <= 0)
			{
				m_iSoundChannels = 16;
			}
		}
		else if (strcmp(m_pConfigFile->GetToken(), "sound_buffer") == 0)
		{
			m_iSoundBufferSize = atoi(m_pConfigFile->GetValue());

			if (m_iSoundBufferSize <= 0)
			{
				m_iSoundBufferSize = 1024;
			}
		}
		else if (strcmp(m_pConfigFile->GetToken(), "sound_stereo") == 0)
		{
			if (strcmp(m_pConfigFile->GetValue(), "true") == 0)
			{
				m_bStereo = true;
			}
			else
			{
				m_bStereo = false;
			}
		}
		else if (strcmp(m_pConfigFile->GetToken(), "show_fps") == 0)
		{
			if (strcmp(m_pConfigFile->GetValue(), "true") == 0)
			{
				m_bShowFPS = true;
			}
			else
			{
				m_bShowFPS = false;
			}
		}

	}while(result == 1);

	m_pConfigFile->CloseConfig();
}

void CEngine::WriteLog(const char* str)
{
	FILE* log;
	log = fopen("log.txt", "a");
	fprintf(log, str);
	fclose(log);
}
