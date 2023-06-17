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
	EnemyPlanetoid.h

	Author: Markus Liehmann
	Date: 02/08/2004
	Description: The CEnemyPlanetoid class is derived from CObject and represents a planetoid in the
		game-world. The planetoid is created using the CPlanetoid class, and is represented in the
		ODE by a simple sphere (both Body and Geom).
*/

#ifndef __MM_ENEMYPLANETOID_H__
#define __MM_ENEMYPLANETOID_H__

#include "camera.h"
#include "object.h"
#include "Arena.h"
#include "Planetoid.h"
#include "world.h"
#include "PlanetoidExplosion.h"
#include <sdl_opengl.h>
#include <ode/ode.h>

class CWorld;
class CArena;
class CPlanetoidExplosion;

class CEnemyPlanetoid : public CObject
{
private:
	void BuildPlanetoid();

	CPlanetoid* m_pPlanetoid;
	CPlanetoidExplosion* m_pExplosion;
	GLuint m_oglPlanetoid;					// ID of the display-list
	float m_fPlayerDistance;				// distance to player, needed for sound-volume
	float m_fBoundingSphereRadius;			// size of the planetoid
	CVector m_vecDisplacement;				// displacement to 0

protected:
	void OnCollision(CObject *collisionObject);
	void OnPrepare();
	void OnAnimate(float deltaTime);
	void OnDraw(CCamera *camera);

public:
	CWorld* m_pWorld;
	dGeomID m_ODEGeom;
	dBodyID m_ODEBody;
	int m_iClass;
	int m_iHealth;

	CEnemyPlanetoid(CWorld* world, CVector* pos, int planetoid_class, float deltaTime);
	~CEnemyPlanetoid();
};

#endif