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

#include "gui.h"

CGUI::CGUI(CWorld* world)
{
	m_pWorld = world;
	m_fXScale = (float)m_pWorld->m_iScreenWidth / 1024.0f;
	m_fYScale = (float)m_pWorld->m_iScreenHeight / 768.0f;

	BuildFont();
}

CGUI::~CGUI()
{
	glDeleteLists(m_uiDisplayListsBase,256);					// Delete All 256 Display Lists
}

void CGUI::BuildFont()
{
	float cx;													// Holds Our X Character Coord
	float cy;													// Holds Our Y Character Coord

	m_uiDisplayListsBase = glGenLists(256);						// Creating 256 Display Lists

	for (int i = 0; i < 256; i++)								// Loop Through All 256 Lists
	{
		cx = (float)(i % 16) / 16.0f;							// X Position Of Current Character
		cy = (float)(i / 16) / 16.0f;							// Y Position Of Current Character

		glNewList(m_uiDisplayListsBase + i, GL_COMPILE);		// Start Building A List
			glBegin(GL_QUADS);									// Use A Quad For Each Character
				glTexCoord2f(cx, 1 - cy - 0.0625f);				// Texture Coord (Bottom Left)
				glVertex2i(0, 0);								// Vertex Coord (Bottom Left)

				glTexCoord2f(cx + 0.0625f, 1 - cy - 0.0625f);	// Texture Coord (Bottom Right)
				glVertex2i(16, 0);								// Vertex Coord (Bottom Right)

				glTexCoord2f(cx + 0.0625f, 1 - cy);				// Texture Coord (Top Right)
				glVertex2i(16, 16);								// Vertex Coord (Top Right)

				glTexCoord2f(cx, 1 - cy);						// Texture Coord (Top Left)
				glVertex2i(0, 16);								// Vertex Coord (Top Left)
			glEnd();											// Done Building Our Quad (Character)
			glTranslatef(10.0f, 0.0f, 0.0f);					// Move To The Right Of The Character
		glEndList();											// Done Building The Display List
	}															// Loop Until All 256 Are Built
}

void CGUI::PrintText(int x, int y, float scale_x, float scale_y, char* string, int set)
{
	if (set > 1)												// Is set Greater Than One?
	{
		set = 1;												// If So, Make Set Equal One
	}

	if (set < 1)
	{
		set = 0;
	}

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);									// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);								// Select The Projection Matrix
	glPushMatrix();												// Store The Projection Matrix
	glLoadIdentity();											// Reset The Projection Matrix
	glOrtho(0, m_pWorld->m_iScreenWidth, 0,
		m_pWorld->m_iScreenHeight, -1, 1);						// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);									// Select The Modelview Matrix
	glPushMatrix();												// Store The Modelview Matrix
	glLoadIdentity();											// Reset The Modelview Matrix
	glTranslatef((float)x, (float)y, 0.0f);						// Position The Text (0,0 - Bottom Left)
	glScalef(scale_x, scale_y, 1.0f);
	glListBase(m_uiDisplayListsBase - 32 + (128 * set));		// Choose The Font Set (0 or 1)
	glBindTexture(GL_TEXTURE_2D, m_pWorld->m_texFont.texID);
	glCallLists(strlen(string), GL_UNSIGNED_BYTE, string);		// Write The Text To The Screen
	glMatrixMode(GL_PROJECTION);								// Select The Projection Matrix
	glPopMatrix();												// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);									// Select The Modelview Matrix
	glPopMatrix();												// Restore The Old Projection Matrix
	glEnable(GL_DEPTH_TEST);									// Enables Depth Testing
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

void CGUI::DrawMainMenu()
{
	int posx = m_pWorld->m_iScreenWidth / 2 - (int)(58.0f * m_fXScale);					// 8 + 10 * strlen
	int posy = m_pWorld->m_iScreenHeight - (m_pWorld->m_iScreenHeight / 4);
	glColor4fv(m_pWorld->m_faFontColor);
	PrintText(posx, posy, 2.0f * m_fXScale, 2.0f * m_fYScale, "Ar.P.", 1);

	posx = m_pWorld->m_iScreenWidth / 2 - (int)(109.0f * m_fXScale);					// 4 + 5 * strlen
	posy -= m_pWorld->m_iScreenHeight / 8;
	PrintText(posx, posy, 1.0f * m_fXScale, 1.0f * m_fYScale, "Artificial Planetoids", 0);

	posx = m_pWorld->m_iScreenWidth / 2 - (int)(188.0f * m_fXScale);
	posy -= m_pWorld->m_iScreenHeight / 2;
	PrintText(posx, posy, 2.0f * m_fXScale, 1.0f * m_fYScale, "Press F10 to begin", 0);	// 8 + 10 * strlen

	posx = m_pWorld->m_iScreenWidth - (int)(298.0f * m_fXScale);
	PrintText(posx, 0, 1.0f * m_fXScale, 1.0f * m_fYScale, "(c) 2003-2004 Markus Liehmann", 0);

	PrintText(0, 0, 1.0f * m_fXScale, 1.0f * m_fYScale, "v1.0", 0);
}

void CGUI::DrawIngameMenu()
{
	int posx = m_pWorld->m_iScreenWidth / 2 - (int)(118.0f * m_fXScale);
	int posy = m_pWorld->m_iScreenHeight - (m_pWorld->m_iScreenHeight / 2);
	glColor4fv(m_pWorld->m_faFontColor);
	PrintText(posx, posy, 2.0f * m_fXScale, 2.0f * m_fYScale, "Game Paused", 0);

	posx = m_pWorld->m_iScreenWidth / 2 - (int)(188.0f * m_fXScale);
	posy -= m_pWorld->m_iScreenHeight / 8 * 3;
	PrintText(posx, posy, 2.0f * m_fXScale, 1.0f * m_fYScale, "Press F1 to resume", 0);
}

void CGUI::DrawGameWon()
{
	int posx = m_pWorld->m_iScreenWidth / 2 - (int)(108.0f * m_fXScale);
	int posy = m_pWorld->m_iScreenHeight - (m_pWorld->m_iScreenHeight / 2);
	glColor4fv(m_pWorld->m_faFontColor);
	PrintText(posx, posy, 2.0f * m_fXScale, 2.0f * m_fYScale, "You win!!!", 0);

	posx = m_pWorld->m_iScreenWidth / 2 - (int)(218.0f * m_fXScale);
	posy -= m_pWorld->m_iScreenHeight / 8 * 3;
	PrintText(posx, posy, 2.0f * m_fXScale, 1.0f * m_fYScale, "Press F10 to continue", 0);
}

void CGUI::DrawGameLost()
{
	int posx = m_pWorld->m_iScreenWidth / 2 - (int)(128.0f * m_fXScale);
	int posy = m_pWorld->m_iScreenHeight - (m_pWorld->m_iScreenHeight / 2);
	glColor4fv(m_pWorld->m_faFontColor);
	PrintText(posx, posy, 2.0f * m_fXScale, 2.0f * m_fYScale, "You loose!!!", 0);

	posx = m_pWorld->m_iScreenWidth / 2 - (int)(218.0f * m_fXScale);
	posy -= m_pWorld->m_iScreenHeight / 8 * 3;
	PrintText(posx, posy, 2.0f * m_fXScale, 1.0f * m_fYScale, "Press F10 to continue", 0);
}

void CGUI::DrawPlayerHUD(int health, float weapon_delay, float weapon_max_delay, float displace_x, float displace_y)
{
	char string[256];
	itoa(health, string, 10);

	int posx = m_pWorld->m_iScreenWidth / 24 + (int)(displace_x * m_fXScale);
	int posy = m_pWorld->m_iScreenHeight / 24 + (int)(displace_y * m_fYScale);
	glColor4fv(m_pWorld->m_faFontColor);
	PrintText(posx, posy, 2.0f * m_fXScale, 2.0f * m_fYScale, string, 0);

	if (weapon_delay >= weapon_max_delay)
	{
		posx = m_pWorld->m_iScreenWidth - m_pWorld->m_iScreenWidth / 24 - (int)(240.0f * m_fXScale) + (int)(displace_x * m_fXScale);
		PrintText(posx, posy, 2.0f * m_fXScale, 2.0f * m_fYScale, "Weapon ready", 0);
	}
	else
	{
		posx = m_pWorld->m_iScreenWidth - m_pWorld->m_iScreenWidth / 24 - (int)(280.0f * m_fXScale) + (int)(displace_x * m_fXScale);
		PrintText(posx, posy, 2.0f * m_fXScale, 2.0f * m_fYScale, "Weapon loading", 0);
	}
}

void CGUI::DrawCrosshair(float displace_x, float displace_y)
{
	int x_disposition = 8 + 10 * strlen(m_pWorld->m_caCrosshair);
	int posx = m_pWorld->m_iScreenWidth / 2 - (int)((float)x_disposition * m_fXScale) + (int)(displace_x * m_fXScale);
	int posy = m_pWorld->m_iScreenHeight / 2 - (int)(15.0f * m_fXScale) + (int)(displace_y * m_fYScale);
	glColor4fv(m_pWorld->m_faFontColor);
	PrintText(posx, posy, 2.0f * m_fXScale, 2.0f * m_fYScale, m_pWorld->m_caCrosshair, 0);
}

void CGUI::DrawFPS(int fps)
{
	char string[256];
	itoa(fps, string, 10);

	int posy = m_pWorld->m_iScreenHeight - (int)(40.0f * m_fYScale);
	glColor4fv(m_pWorld->m_faFontColor);
	PrintText(0, posy, 2.0f * m_fXScale, 2.0f * m_fYScale, string, 0);
}