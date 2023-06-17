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
	gui.h

	Author: Markus Liehmann
	(based on lesson 17 of OpenGL tutorials by Giuseppe D'Agata and Jeff Molofee (NeHe))
	Date: 02/08/2004
	Description: The CGUI class represents the GUI in the game. It has functions to draw the main menu,
		the ingame menu (pause), game won/lost messages and the player HUD (including the crosshair).
*/

#ifndef __MM_GUI_H__
#define __MM_GUI_H__

#include "world.h"
#include <sdl_opengl.h>

class CGUI
{
private:
	void BuildFont();
	CWorld* m_pWorld;
	unsigned int m_uiDisplayListsBase;

	// scale factors to ensure that the chars have always the same size compared to the screen

	float m_fXScale;
	float m_fYScale;

public:
	CGUI(CWorld* world);
	~CGUI();

	void PrintText(int x, int y, float scale_x, float scale_y, char* string, int set);
	void DrawMainMenu();
	void DrawIngameMenu();
	void DrawGameWon();
	void DrawGameLost();
	void DrawPlayerHUD(int health, float weapon_delay, float weapon_max_delay, float displace_x, float displace_y);
	void DrawCrosshair(float displace_x, float displace_y);
	void DrawFPS(int fps);
};

#endif