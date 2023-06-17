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

#include "OpenGLWindow.h"

COpenGLWindow::COpenGLWindow()
{
	// initialize SDL library

	if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO) == -1)
    {
		char sz[256];
		sprintf (sz, "Can't init SDL:  %s\n", SDL_GetError());
		throw sz;
    }

	WriteLog("SDL_Init.\n");

	m_bFullscreen = false;
	m_iWidth = 0;
	m_iHeight = 0;
	m_iBits = 0;
}

COpenGLWindow::COpenGLWindow(const char* strName, bool fscreen, int w, int h, int b)
{
	// initialize SDL library

	if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO) == -1)
    {
		char sz[256];
		sprintf (sz, "Can't init SDL: %s\n", SDL_GetError());
		throw sz;
    }

	WriteLog("SDL_Init.\n");

	SetWindow(strName, fscreen, w, h, b);
}

COpenGLWindow::~COpenGLWindow()
{
	SDL_Quit();
	WriteLog("SDL_Quit.\n");
}

void COpenGLWindow::SetWindow(const char* strName, bool fscreen, int w, int h, int b)
{
	m_bFullscreen = fscreen;
	m_iWidth = w;
	m_iHeight = h;
	m_iBits = b;

	Uint32 flags;

	// set window title and hide cursor

	SDL_WM_SetCaption(strName, NULL);
	SDL_ShowCursor(SDL_DISABLE);

	flags = SDL_OPENGL;

	if (m_bFullscreen)
	{
		flags |= SDL_FULLSCREEN;
	}

	// set OpenGL attributes

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (SDL_SetVideoMode(m_iWidth, m_iHeight, m_iBits, flags) == NULL)
	{
		char sz[256];
		sprintf (sz, "Can't set video mode: %s\n", SDL_GetError());
		throw sz;
	}

	SDL_WM_GrabInput(SDL_GRAB_ON);

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(54.0f, (float)w/(float)h, 0.1f, 256.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	char str[256];

	WriteLog("SDL_SetVideoMode successful.\n");

	sprintf(str, "Width: %d.\nHeight: %d.\nBits: %d.\n", m_iWidth, m_iHeight, m_iBits);
	WriteLog(str);

	if (m_bFullscreen == true)
	{
		WriteLog("Fullscreen: true.\n");
	}
	else
	{
		WriteLog("Fullscreen: false.\n");
	}
}

int COpenGLWindow::GetWidth()
{
	return m_iWidth;
}

int COpenGLWindow::GetHeight()
{
	return m_iHeight;
}

int COpenGLWindow::GetBits()
{
	return m_iBits;
}

bool COpenGLWindow::GetFullscreen()
{
	return m_bFullscreen;
}

void COpenGLWindow::WriteLog(const char* str)
{
	FILE* log;
	log = fopen("log.txt", "a");
	fprintf(log, str);
	fclose(log);
}
