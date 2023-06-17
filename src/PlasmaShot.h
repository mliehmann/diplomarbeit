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
	PlasmaShot.h

	Author: Markus Liehmann
	Date: 02/08/2004
	Description: The CPlasmaShot class is derived from CObject and represents a shot fired by the
		player. The plasma shot is also an object in ODE, a simple sphere. Each shot is attached
		to the player.
*/

#ifndef __MM_PLASMASHOT_H__
#define __MM_PLASMASHOT_H__

#include "camera.h"
#include "object.h"
#include "Arena.h"
#include "world.h"
#include "PlasmaExplosion.h"
#include <sdl_opengl.h>
#include <ode/ode.h>

class CWorld;
class CPlasmaExplosion;

class CPlasmaShot : public CObject
{
private:
	float m_fLifetime;
	CPlasmaExplosion* m_pExplosion;

protected:
	void OnAnimate(scalar_t deltaTime);
	void OnCollision(CObject *collisionObject);
	void OnDraw(CCamera *camera);
	void OnPrepare();

public:
	dGeomID m_ODEGeom;
	dBodyID m_ODEBody;
	CWorld* m_pWorld;
	int m_iHealth;

	CPlasmaShot(CWorld* world, float deltaTime);
	~CPlasmaShot();
};

#endif