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
	PlanetoidExplosion.h

	Author: Markus Liehmann (based on CExplosion written by Dave Astle for OpenGL Game Programming)
	Date: 02/08/2004
	Description: The CPlanetoidExplosion class is derived from CParticleSystem and represents
		the particle effect of an exploding planetoid.
*/

#ifndef __MM_PLANETOIDEXPLOSION_H__
#define __MM_PLANETOIDEXPLOSION_H__

#include "world.h"
#include "ParticleSystem.h"
#include <sdl_opengl.h>

class CWorld;

class CPlanetoidExplosion : public CParticleSystem
{
public:
	CPlanetoidExplosion(CWorld* world, int maxParticles, CVector origin, CVector velocity, float radius);
	~CPlanetoidExplosion();

	void OnPrepare();
	void OnAnimate(float deltaTime);
	void OnDraw(CCamera *camera);

	CWorld* m_pWorld;

protected:
	void InitializeParticle(int index);

private:
	float m_fRadius;
	CVector m_vecVelocity;
};

#endif