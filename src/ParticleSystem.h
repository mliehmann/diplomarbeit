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
	ParticleSystem.h

	Author: Markus Liehmann (based on CParticleSystem written by Dave Astle for OpenGL Game Programming)
	Date: 02/08/2004
	Description: The CParticleSystem class represents the base class for all particle effects.
*/

#ifndef __MM_PARTICLESYSTEM_H__
#define __MM_PARTICLESYSTEM_H__

#include "object.h"
#include "vector.h"
#include <sdl_opengl.h>

class CParticleSystem;

struct particle_t
{
	CVector  m_vecCurrentPosition;			// current position of the particle
	CVector  m_vecPreviousPosition;			// last position of the particle
	CVector  m_vecVelocity;					// direction and speed
	CVector  m_vecAcceleration;				// acceleration

	float   m_fLifetime;					// determines how long the particle is alive

	float   m_fSize;						// size of particle
	float   m_fSizeDelta;					// amount to change the size over time

	float   m_fWeight;						// determines how gravity affects the particle
	float   m_fWeightDelta;					// change over time

	float   m_faColor[4];					// current color of the particle
	float   m_faColorDelta[4];				// how the color changes with time
};

class CParticleSystem : public CObject
{
public:
	CParticleSystem(int maxParticles, CVector origin);

	// abstract functions, InitializeParticle MUST be implemented in derived class ( = 0) 

	virtual int Emit(int numParticles);

	virtual void InitializeSystem();
	virtual void KillSystem();

protected:
	virtual void InitializeParticle(int index) = 0;
	particle_t* m_pParticleList;				// particles for this emitter
	int m_iMaxParticles;						// maximum number of particles in total
	int m_iNumParticles;						// indicies of all free particles

	float m_fAccumulatedTime;					// used to track how long since the last particle was emitted

	CVector m_vecForce;							// force (gravity, wind, etc.) acting on the particle system
};

#endif