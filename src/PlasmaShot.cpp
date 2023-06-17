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

#include "PlasmaShot.h"

CPlasmaShot::CPlasmaShot(CWorld* world, float deltaTime)
{
	m_pWorld = world;
	m_iHealth = m_pWorld->m_iPlasmaHealth;
	m_fLifetime = 0.0f;
	m_iIdent = 3;

	float start_factor = 0.001f / deltaTime;	// scale torque and force from config to deltaTime (config: 1 ms)

	// player is a sphere in both ODE world and space

	m_pWorld = world;
	m_ODEGeom = dCreateSphere (m_pWorld->m_ODESpace, 0.5);	// create ODE Geom
	dGeomSetData (m_ODEGeom, this);							// set this-pointer as Geom-data (needed for collision-callback)
	m_ODEBody = dBodyCreate (m_pWorld->m_ODEWorld);			// create ODE Body
	dGeomSetBody (m_ODEGeom, m_ODEBody);					// attach Body to Geom

	// shot starts in front of player

	CVector pos;

	pos.x = 0.0f * m_pWorld->m_pPlayer->m_faRotMatrix[1] +
		2.0f * m_pWorld->m_pPlayer->m_faRotMatrix[2] +
		m_pWorld->m_pPlayer->m_vecPosition.x;
	pos.y = 0.0f * m_pWorld->m_pPlayer->m_faRotMatrix[5] +
		2.0f * m_pWorld->m_pPlayer->m_faRotMatrix[6] +
		m_pWorld->m_pPlayer->m_vecPosition.y;
	pos.z = 0.0f * m_pWorld->m_pPlayer->m_faRotMatrix[9] +
		2.0f * m_pWorld->m_pPlayer->m_faRotMatrix[10] +
		m_pWorld->m_pPlayer->m_vecPosition.z;

	dBodySetPosition(m_ODEBody, pos.x, pos.y, pos.z);

	dMass plasma_mass;
	float mass = m_pWorld->m_fPlasmaMass;
	dMassSetSphereTotal (&plasma_mass, mass, 0.5);	// set mass type sphere
	dBodySetMass (m_ODEBody, &plasma_mass);			// set plasma shot mass

	// get rotation of player from ODE and transpose it for use with OpenGL

	const float* matrix = dBodyGetRotation(m_ODEBody);

	m_faRotMatrix[0] = matrix[0];
	m_faRotMatrix[1] = matrix[4];
	m_faRotMatrix[2] = matrix[8];
	m_faRotMatrix[4] = matrix[1];
	m_faRotMatrix[5] = matrix[5];
	m_faRotMatrix[6] = matrix[9];
	m_faRotMatrix[8] = matrix[2];
	m_faRotMatrix[9] = matrix[6];
	m_faRotMatrix[10] = matrix[10];

	m_faRotMatrix[3] = 0.0f;
	m_faRotMatrix[7] = 0.0f;
	m_faRotMatrix[11] = 0.0f;
	m_faRotMatrix[12] = 0.0f;
	m_faRotMatrix[13] = 0.0f;
	m_faRotMatrix[14] = 0.0f;
	m_faRotMatrix[15] = 1.0f;

	m_vecPosition = pos;
	m_fSize = 0.5f;

	// calculate force

	pos.x -= m_pWorld->m_pPlayer->m_vecPosition.x;
	pos.y -= m_pWorld->m_pPlayer->m_vecPosition.y;
	pos.z -= m_pWorld->m_pPlayer->m_vecPosition.z;
	pos.Normalize();
	pos.x *= start_factor * m_pWorld->m_fPlasmaForce;
	pos.y *= start_factor * m_pWorld->m_fPlasmaForce;
	pos.z *= start_factor * m_pWorld->m_fPlasmaForce;

	dBodyAddRelForce(m_ODEBody, pos.x, pos.y, pos.z);
}

CPlasmaShot::~CPlasmaShot()
{
	dGeomDestroy (m_ODEGeom);
	dBodyDestroy (m_ODEBody);
}

void CPlasmaShot::OnAnimate(scalar_t deltaTime)
{
	// get position from ODE (rotation is not needed)

	const float* pos = dBodyGetPosition(m_ODEBody);
	m_vecPosition.x = pos[0];
	m_vecPosition.y = pos[1];
	m_vecPosition.z = pos[2];

	m_fLifetime += deltaTime;
}

void CPlasmaShot::OnCollision(CObject *collisionObject)
{
	if (!m_pWorld->IsMenu())
	{
		if (collisionObject->m_iIdent == 2)
		{
			m_iHealth -= m_pWorld->m_iHealthLoss;
		}

		if ((collisionObject->m_iIdent == 0) && (m_pWorld->m_bPlasmaArenaBounce == false))
		{
			// plasma collided with wall, kill it (no bounce)

			m_iHealth = 0;
		}

		if (collisionObject->m_iIdent == 1)
		{
			// player got hit by plasma, play sound

			Mix_PlayChannel(-1, m_pWorld->m_pSamplePlasmaHit, 0);
		}
	}
}

void CPlasmaShot::OnDraw(CCamera *camera)
{
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_LIGHTING);

	/* outgoing from camera, translate matrix and multiplicate matrix with transposed camera matrix
	   to ensure billboarding*/

	glTranslatef(-m_vecPosition.x, -m_vecPosition.y, -m_vecPosition.z);
	float matrix[16];

	matrix[0] = camera->m_pObject->m_faRotMatrix[0];
	matrix[1] = camera->m_pObject->m_faRotMatrix[4];
	matrix[2] = camera->m_pObject->m_faRotMatrix[8];
	matrix[4] = camera->m_pObject->m_faRotMatrix[1];
	matrix[5] = camera->m_pObject->m_faRotMatrix[5];
	matrix[6] = camera->m_pObject->m_faRotMatrix[9];
	matrix[8] = camera->m_pObject->m_faRotMatrix[2];
	matrix[9] = camera->m_pObject->m_faRotMatrix[6];
	matrix[10] = camera->m_pObject->m_faRotMatrix[10];
	matrix[3] = 0.0f;
	matrix[7] = 0.0f;
	matrix[11] = 0.0f;
	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;

	glMultMatrixf(matrix);

	glColor4fv(m_pWorld->m_faPlasmaColor);
	glBindTexture(GL_TEXTURE_2D, m_pWorld->m_texPlasma.texID);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-0.5f, 0.5f, 0.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-0.5f, -0.5f, 0.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.5f, -0.5f, 0.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.5f, 0.5f, 0.0f);
	glEnd();

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

void CPlasmaShot::OnPrepare()
{
	if ((m_fLifetime >= m_pWorld->m_fPlasmaLifetime) || (m_iHealth <= 0))
	{
		// delete shot if lifetime expired shot has no health anymore

		m_bDelete = true;

		// create new particle effect and attach it to m_pParticles

		m_pExplosion = new CPlasmaExplosion(m_pWorld, 20, m_vecPosition, 0.5f);
		m_pExplosion->AttachTo(m_pWorld->m_pParticles);
	}
}
