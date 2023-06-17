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

#include "PlasmaExplosion.h"

CPlasmaExplosion::CPlasmaExplosion(CWorld* world, int maxParticles, CVector origin, float radius)
  : CParticleSystem(maxParticles, origin)
{
	m_pWorld = world;
	m_fRadius = radius;
	CParticleSystem::InitializeSystem();
	Emit(maxParticles);
}

CPlasmaExplosion::~CPlasmaExplosion()
{
	KillSystem();
}

void CPlasmaExplosion::InitializeParticle(int index)
{
  // start the particle at a random location in the emission zone

  m_pParticleList[index].m_vecCurrentPosition.x = m_vecPosition.x +
	  (m_fRadius - (float)rand() / (float)RAND_MAX * 2.0f * m_fRadius);
  m_pParticleList[index].m_vecCurrentPosition.y = m_vecPosition.y +
	  (m_fRadius - (float)rand() / (float)RAND_MAX * 2.0f * m_fRadius);
  m_pParticleList[index].m_vecCurrentPosition.z = m_vecPosition.z +
	  (m_fRadius - (float)rand() / (float)RAND_MAX * 2.0f * m_fRadius);

  // set the size of the particle

  m_pParticleList[index].m_fSize = m_fRadius + (m_fRadius * 0.5f - (float)rand() / (float)RAND_MAX * m_fRadius);

  // give the particle a random velocity

  m_pParticleList[index].m_vecVelocity.x = 1.0f - 2.0f * (float)rand() / (float)RAND_MAX;
  m_pParticleList[index].m_vecVelocity.y = 1.0f - 2.0f * (float)rand() / (float)RAND_MAX;
  m_pParticleList[index].m_vecVelocity.z = 1.0f - 2.0f * (float)rand() / (float)RAND_MAX;

  m_pParticleList[index].m_vecAcceleration = 0;		// no acceleration

  m_pParticleList[index].m_faColor[0] = m_pWorld->m_faPlasmaColor[0];
  m_pParticleList[index].m_faColor[1] = m_pWorld->m_faPlasmaColor[1];
  m_pParticleList[index].m_faColor[2] = m_pWorld->m_faPlasmaColor[2];
  m_pParticleList[index].m_faColor[3] = m_pWorld->m_faPlasmaColor[3];

  m_pParticleList[index].m_fLifetime = 1.5f;

  m_pParticleList[index].m_faColorDelta[0] = 0.0f;
  m_pParticleList[index].m_faColorDelta[1] = 0.0f;
  m_pParticleList[index].m_faColorDelta[2] = 0.0f;
  m_pParticleList[index].m_faColorDelta[3] = -m_pWorld->m_faPlasmaColor[3]/m_pParticleList[index].m_fLifetime;

  m_pParticleList[index].m_fSizeDelta = -m_pParticleList[index].m_fSize/m_pParticleList[index].m_fLifetime;

}

void CPlasmaExplosion::OnPrepare()
{
}

void CPlasmaExplosion::OnAnimate(float deltaTime)
{
	if (m_iNumParticles == 0)
	{
		m_bDelete = true;
	}
  for (int i = 0; i < m_iNumParticles; )
  {
    // update the particle's position based on the elapsed time and velocity

    m_pParticleList[i].m_vecCurrentPosition = m_pParticleList[i].m_vecCurrentPosition +
		m_pParticleList[i].m_vecVelocity * deltaTime;
    m_pParticleList[i].m_vecVelocity = m_pParticleList[i].m_vecVelocity +
		m_pParticleList[i].m_vecAcceleration * deltaTime;

    m_pParticleList[i].m_fLifetime -= deltaTime;
    m_pParticleList[i].m_fSize += m_pParticleList[i].m_fSizeDelta * deltaTime;

    m_pParticleList[i].m_faColor[3] += m_pParticleList[i].m_faColorDelta[3] * deltaTime;

    // if the particle has expired, kill it

    if (m_pParticleList[i].m_fLifetime <= 0.0)
    {
		// move the last particle to the current positon, and decrease the count

		m_pParticleList[i] = m_pParticleList[--m_iNumParticles];
    }
    else
    {
      ++i;
    }
  }
}

void CPlasmaExplosion::OnDraw(CCamera *camera)
{
	float viewMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);

	CVector right(viewMatrix[0], viewMatrix[4], viewMatrix[8]);
	CVector up(viewMatrix[1], viewMatrix[5], viewMatrix[9]);

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_LIGHTING);

	glBindTexture(GL_TEXTURE_2D, m_pWorld->m_texParticle.texID);

	GLfloat size;
	CVector pos;

	glBegin(GL_QUADS);
	for (int i = 0; i < m_iNumParticles; ++i)
	{
		size = m_pParticleList[i].m_fSize/2;
		pos = m_pParticleList[i].m_vecCurrentPosition;
		glColor4fv(m_pParticleList[i].m_faColor);
		glTexCoord2f(0.0, 0.0); glVertex3fv((-pos + (right + up) * -size).v);
		glTexCoord2f(0.0, 1.0); glVertex3fv((-pos + (up - right) * size).v);
		glTexCoord2f(1.0, 1.0); glVertex3fv((-pos + (right + up) * size).v);
		glTexCoord2f(1.0, 0.0); glVertex3fv((-pos + (right - up) * size).v);
	}
	glEnd();

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}