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

#include "ParticleSystem.h"

CParticleSystem::CParticleSystem(int maxParticles, CVector origin)
{
	m_iMaxParticles = maxParticles;
	m_vecForce = CVector(0.0f, 0.0f, 0.0f);
	m_vecPosition = origin;
	m_pParticleList = NULL;
}

int CParticleSystem::Emit(int numParticles)
{
	// create numParticles new particles (if there's room)

	while (numParticles && (m_iNumParticles < m_iMaxParticles))
	{
		// initialize the current particle and increase the count

		InitializeParticle(m_iNumParticles++);
		--numParticles;
	}
	return numParticles;
}

void CParticleSystem::InitializeSystem()
{
	// if this is just a reset, free the memory

	if (m_pParticleList)
	{
		delete[] m_pParticleList;
		m_pParticleList = NULL;
	}

	// allocate the maximum number of particles

	m_pParticleList = new particle_t[m_iMaxParticles];

	// reset the number of particles and accumulated time

	m_iNumParticles = 0;
	m_fAccumulatedTime = 0.0f;
}

void CParticleSystem::KillSystem()
{
	if (m_pParticleList)
	{
		delete[] m_pParticleList;
		m_pParticleList = NULL;
	}

	m_iNumParticles = 0;
}