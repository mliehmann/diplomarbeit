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
	ArPEngine.h

	Author: Markus Liehmann
	Date: 02/08/2004
	Description: The CArPEngine class is the main interface for ArP. It is derived from CEngine and
		provides the CCamera and CWorld objects.
*/

#ifndef __MM_ARPENGINE_H__
#define __MM_ARPENGINE_H__

#include <SDL.h>
#include "Engine.h"
#include "object.h"
#include "camera.h"
#include "world.h"
#include "Player.h"
#include "time.h"

class CArPEngine : public CEngine
{
public:
	CArPEngine();
	CArPEngine(const char *strFileName);
	~CArPEngine();

protected:
	CCamera *OnGetCamera();
	CWorld *OnGetWorld();

	void OnPrepare();
	void OnMouseDownL(float x, float y);
	void OnMouseUpL(float x, float y);
	void OnMouseDownR(float x, float y);
	void OnMouseUpR(float x, float y);
	void OnMouseDownM(float x, float y);
	void OnMouseUpM(float x, float y);
	void OnMouseMove(int deltaX, int deltaY);
	void OnKeyDown(SDLKey key);
	void OnKeyUp(SDLKey key);
	void OnSetup();
	void OnFocus(int gain);

private:
	void ReadBindings();
	void ReadLevelList();
	void SaveScreenshot();

	CCamera* m_pCamera;
	CWorld* m_pWorld;
	CPlayer* m_pPlayer;

	// key bindings

	SDLKey shoot, thrust, brake, roll_left, roll_right, increase_mousesens, decrease_mousesens, music;
	int m_iBindLeft, m_iBindRight, m_iBindMiddle;

	char** m_levellist;
	int m_iLevels;
	int m_iCurrentLevel;
	bool m_bMenuActive;
	bool m_bLevelContinue;
};

#endif