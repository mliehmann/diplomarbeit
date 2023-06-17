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
	Arena.h

	Author: Markus Liehmann
	Date: 02/08/2004
	Description: The CArena class is derived from CObject and represents the cubic arena, in which
		the player has to destroy the planetoids. The arena consists of a skybox and a cube. To process
		collision detection, the cube is defined by 6 ODE-Geom-Planes.
		The OpenGL lights are also set and controlled by the arena.
*/

#ifndef __MM_ARENA_H__
#define __MM_ARENA_H__

#include "object.h"
#include "texture.h"
#include "world.h"
#include <ode/ode.h>

class CWorld;

class CArena : public CObject
{
private:
	void BuildArena();

	float m_fWidth;

	float m_faLight0Position[4];
	float m_faLight1Position[4];
	float m_faLight2Position[4];
	float m_faLight0Color[4];
	float m_faLight1Color[4];
	float m_faLight2Color[4];
	dGeomID m_ODEGeomPlanes[6];

protected:
	// arena doesn't move, so no physics animations

	void OnAnimate(scalar_t deltaTime) { }
	void OnDraw(CCamera *camera);
	void OnCollision(CObject *collisionObject) { }

public:
	CWorld* m_pWorld;

	CArena();
	CArena(CWorld* world);
	~CArena();

	float GetWidth() { return m_fWidth; }
};

#endif