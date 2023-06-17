#include "OpenGLWindow.h"

COpenGLWindow::COpenGLWindow()
{
	if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO) == -1)
    {
		char sz[256];
		sprintf (sz, "Can't init SDL:  %s\n", SDL_GetError());
		throw sz;
    }

	bFullscreen = false;
	iWidth = 0;
	iHeight = 0;
	iBits = 0;
}

COpenGLWindow::COpenGLWindow(const char *szName, bool fscreen, int w, int h, int b)
{
	bFullscreen = fscreen;
	iWidth = w;
	iHeight = h;
	iBits = b;

	Uint32 flags;

    if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO) == -1)
    {
		char sz[256];
		sprintf (sz, "Can't init SDL:  %s\n", SDL_GetError());
		throw sz;
    }

	SDL_WM_SetCaption(szName, NULL);

	flags = SDL_OPENGL;

	if (bFullscreen)
	{
		flags |= SDL_FULLSCREEN;
	}

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (SDL_SetVideoMode(iWidth, iHeight, iBits, flags) == NULL)
	{
		char sz[256];
		sprintf (sz, "Can't set video mode: %s\n", SDL_GetError());
		throw sz;
	}
}

COpenGLWindow::~COpenGLWindow()
{
	SDL_Quit();
}

void COpenGLWindow::SetWindow(const char *szName, bool fscreen, int w, int h, int b)
{
	bFullscreen = fscreen;
	iWidth = w;
	iHeight = h;
	iBits = b;

	Uint32 flags;

	SDL_WM_SetCaption(szName, NULL);

	flags = SDL_OPENGL;

	if (bFullscreen)
	{
		flags |= SDL_FULLSCREEN;
	}

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (SDL_SetVideoMode(iWidth, iHeight, iBits, flags) == NULL)
	{
		char sz[256];
		sprintf (sz, "Can't set video mode: %s\n", SDL_GetError());
		throw sz;
	}
}