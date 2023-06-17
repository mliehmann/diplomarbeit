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

#include "EnemyPlanetoid.h"

CEnemyPlanetoid::CEnemyPlanetoid(CWorld* world, CVector* pos, int planetoid_class, float deltaTime)
{
	m_pWorld = world;

	// set default values

	m_pExplosion = NULL;
	float mass = 0.0f;
	float torque_max_factor = 0.0f;
	float force_max_factor = 0.0f;
	m_iHealth = 0;
	m_iClass = planetoid_class;

	float start_factor = 0.001f / deltaTime;	// scale torque and force from config to deltaTime (config: 1 ms)

	/* switch on 3 classes
	   class 1: small planetoid (radius 1.0)
	   class 2: middle planetoid (radius 2.0)
	   class 3: big planetoid (radius 4.0)*/

	switch(planetoid_class)
	{
	case 1:
		// create planetoid, set mass, health, max torque and max force

		m_pPlanetoid = new CPlanetoid(m_pWorld->m_iPlanetoid1Complexity, 1.0f,
			m_pWorld->m_fPlanetoid1Roughness, m_pWorld->m_fPlanetoid1MininumSize);
		mass = m_pWorld->m_fPlanetoid1Mass;
		torque_max_factor = m_pWorld->m_fPlanetoid1MaxTorque * start_factor;
		force_max_factor = m_pWorld->m_fPlanetoid1MaxForce * start_factor;
		m_pPlanetoid->ProjectTerrain();
		m_pPlanetoid->CalculateNormals();
		m_iHealth = m_pWorld->m_iPlanetoid1Health;
		break;

	case 2:
		// create planetoid, set mass, health, max torque and max force

		m_pPlanetoid = new CPlanetoid(m_pWorld->m_iPlanetoid2Complexity, 2.0f,
			m_pWorld->m_fPlanetoid2Roughness, m_pWorld->m_fPlanetoid2MininumSize);
		mass = m_pWorld->m_fPlanetoid2Mass;
		torque_max_factor = m_pWorld->m_fPlanetoid2MaxTorque * start_factor;
		force_max_factor = m_pWorld->m_fPlanetoid2MaxForce * start_factor;
		m_pPlanetoid->ProjectTerrain();
		m_pPlanetoid->CalculateNormals();
		m_iHealth = m_pWorld->m_iPlanetoid2Health;
		break;

	case 3:
		// create planetoid, set mass, health, max torque and max force

		m_pPlanetoid = new CPlanetoid(m_pWorld->m_iPlanetoid3Complexity, 4.0f,
			m_pWorld->m_fPlanetoid3Roughness, m_pWorld->m_fPlanetoid3MininumSize);
		mass = m_pWorld->m_fPlanetoid3Mass;
		torque_max_factor = m_pWorld->m_fPlanetoid3MaxTorque * start_factor;
		force_max_factor = m_pWorld->m_fPlanetoid3MaxForce * start_factor;
		m_pPlanetoid->ProjectTerrain();
		m_pPlanetoid->CalculateNormals();
		m_iHealth = m_pWorld->m_iPlanetoid3Health;
		break;
	}

	// planetoid is a sphere in both ODE world and space

	float radius = m_pPlanetoid->GetBoundingSphereRadius();
	m_ODEGeom = dCreateSphere (m_pWorld->m_ODESpace, radius);	// create ODE Geom
	dGeomSetData (m_ODEGeom, this);								// set this-pointer as Geom-data (needed for collision-callback)
	m_ODEBody = dBodyCreate (m_pWorld->m_ODEWorld);				// create ODE Body
	dGeomSetBody (m_ODEGeom, m_ODEBody);						// attach Body to Geom
	dBodySetPosition(m_ODEBody, pos->x, pos->y, pos->z);		// set planetoid position
	dMass planetoid_mass;
	dMassSetSphereTotal (&planetoid_mass, mass, radius);		// set mass type sphere
	dBodySetMass (m_ODEBody, &planetoid_mass);					// set planetoid mass

	// get rotation of planetoid from ODE and transpose it for use with OpenGL

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

	// set CObject data

	m_vecPosition.x = pos->x;
	m_vecPosition.y = pos->y;
	m_vecPosition.z = pos->z;
	m_fSize = radius;
	m_iIdent = 2;

	m_fPlayerDistance = 0.0f;

	// calculate random torque and force and add to ODE Body

	float torque_x = -1.0f * torque_max_factor + (float)rand() * 2.0f * torque_max_factor / (float)RAND_MAX;
	float torque_y = -1.0f * torque_max_factor + (float)rand() * 2.0f * torque_max_factor / (float)RAND_MAX;
	float torque_z = -1.0f * torque_max_factor + (float)rand() * 2.0f * torque_max_factor / (float)RAND_MAX;
	float force_x = -1.0f * force_max_factor + (float)rand() * 2.0f * force_max_factor / (float)RAND_MAX;
	float force_y = -1.0f * force_max_factor + (float)rand() * 2.0f * force_max_factor / (float)RAND_MAX;
	float force_z = -1.0f * force_max_factor + (float)rand() * 2.0f * force_max_factor / (float)RAND_MAX;

	dBodyAddRelTorque(m_ODEBody, torque_x,torque_y, torque_z);
	dBodyAddRelForce(m_ODEBody, force_x, force_y, force_z);

	BuildPlanetoid();
}

CEnemyPlanetoid::~CEnemyPlanetoid()
{
	glDeleteLists(m_oglPlanetoid, 1);
	dGeomDestroy (m_ODEGeom);
	dBodyDestroy (m_ODEBody);
}

void CEnemyPlanetoid::OnPrepare()
{
	// world is main-menu, no planetoid will be destroyed

	if (m_pWorld->IsMenu() == false)
	{
		// calculate distance to player

		m_fPlayerDistance = (m_vecPosition - m_pWorld->m_pPlayer->m_vecPosition).Length();

		if (m_iHealth <= 0)
		{
			// planetoid is dead

			m_bDelete = true;

			// get linear velocity for velocity-calculations of explosion particles

			const float* vel = dBodyGetLinearVel(m_ODEBody);

			CVector velocity;
			velocity.x = vel[0];
			velocity.y = vel[1];
			velocity.z = vel[2];

			// create new particle effect and attach it to m_pParticles

			m_pExplosion = new CPlanetoidExplosion(m_pWorld, (int)(50.0f * m_fSize),
				m_vecPosition, velocity, m_fSize * 0.7f);
			m_pExplosion->AttachTo(m_pWorld->m_pParticles);
		}
	}
}

void CEnemyPlanetoid::OnCollision(CObject *collisionObject)
{
	if (!m_pWorld->IsMenu())
	{
		if ((collisionObject->m_iIdent == 1) || (collisionObject->m_iIdent == 2) ||
			(collisionObject->m_iIdent == 3))
		{
			// scale distance to player to interval [0; 255]

			int dist = (int)(255.0f / (float)m_pWorld->m_iArenaSize * m_fPlayerDistance);
			int channel = Mix_PlayChannel(-1, m_pWorld->m_pSampleHit, 0);
			Mix_UnregisterAllEffects(channel);

			// effect will scale volume depending on distance to player

			Mix_SetDistance(channel, dist);

			m_iHealth -= m_pWorld->m_iHealthLoss;
		}
	}
}

void CEnemyPlanetoid::OnAnimate(float deltaTime)
{
	// get position from ODE

	const float* pos = dBodyGetPosition(m_ODEBody);
	m_vecPosition.x = pos[0];
	m_vecPosition.y = pos[1];
	m_vecPosition.z = pos[2];

	// get rotation from ODE and transpose it for use with OpenGL

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
}

void CEnemyPlanetoid::OnDraw(CCamera *camera)
{
	glColor4fv(m_pWorld->m_faPlanetoidColor);

	// glPushMatrix();			// debug: show bounding sphere

	// outgoing from camera, translate and multiplicate matrix to render planetoid

	glTranslatef(-m_vecPosition.x - m_vecDisplacement.x,
		-m_vecPosition.y - m_vecDisplacement.y,
		-m_vecPosition.z - m_vecDisplacement.z);
	glMultMatrixf(m_faRotMatrix);

	if (m_pWorld->m_bPlanetoidTransparency == true)
	{
		glEnable(GL_BLEND);
		glDepthMask(FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}

	if (m_pWorld->m_bPlanetoidWireframe == true)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	
	if ((m_pWorld->m_bPlanetoidWireframe == false) && (m_pWorld->m_bPlanetoidTransparency == false))
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	glEnable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, m_pWorld->m_texPlanetoid.texID);
	glCallList(m_oglPlanetoid);			// draw planetoid
	glDisable(GL_LIGHTING);

	if (m_pWorld->m_bPlanetoidTransparency == true)
	{
		glDepthMask(TRUE);
		glDisable(GL_BLEND);
	}

	if (m_pWorld->m_bPlanetoidWireframe == true)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if ((m_pWorld->m_bPlanetoidWireframe == false) && (m_pWorld->m_bPlanetoidTransparency == false))
	{
		glDisable(GL_CULL_FACE);
	}

	/*// debug: show bounding
	glDepthMask(FALSE);
	glDisable(GL_LIGHTING);
	glPopMatrix();
	glColor4f(1.0f, 0.0f, 0.0f, 0.4f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	glTranslatef(-m_vecPosition.x, -m_vecPosition.y, -m_vecPosition.z);
	glMultMatrixf(m_faRotMatrix);
	GLUquadricObj* sph = gluNewQuadric();
	gluSphere(sph, m_fBoundingSphereRadius, 16, 16);
	gluDeleteQuadric(sph);
	glDisable(GL_BLEND);
	glDepthMask(TRUE);
	glEnable(GL_LIGHTING);
	*/
}

void CEnemyPlanetoid::BuildPlanetoid()
{
	// generate display list for rendering planetoid

	m_oglPlanetoid = glGenLists(1);

	int complexity = m_pPlanetoid->GetComplexity();
	float textureOffset = 1.0f / (float)complexity;
	CVector*** planetoid = m_pPlanetoid->GetPlanetoid();
	CVector*** normals = m_pPlanetoid->GetNormals();

	glNewList(m_oglPlanetoid, GL_COMPILE);

	for (int l = 0; l < 4; l++)
	{
		for (int j = 0; j < complexity; j++)
		{
			glBegin(GL_TRIANGLE_STRIP);

			for (int i = 0; i <= complexity; i++)
			{
				glNormal3f(normals[l][i][j].x, normals[l][i][j].y, normals[l][i][j].z);

				if (l%2 == 0)
				{
					if (m_pWorld->m_iPlanetoidTexTiling > 0)
					{
						glTexCoord2f(0.0f, i%2 == 0 ? 0.0f : (float)m_pWorld->m_iPlanetoidTexTiling);
					}
					else
					{
						glTexCoord2f((float)j * textureOffset, (float)i * textureOffset);
					}
				}
				else
				{
					if (m_pWorld->m_iPlanetoidTexTiling > 0)
					{
						glTexCoord2f(i%2 == 0 ? 0.0f : (float)m_pWorld->m_iPlanetoidTexTiling, 0.0f);
					}
					else
					{
						glTexCoord2f((float)i * textureOffset, (float)j * textureOffset);
					}
				}

				glVertex3f(planetoid[l][i][j].x, planetoid[l][i][j].y, planetoid[l][i][j].z);
				glNormal3f(normals[l][i][j+1].x, normals[l][i][j+1].y, normals[l][i][j+1].z);

				if (l%2 == 0)
				{
					if (m_pWorld->m_iPlanetoidTexTiling > 0)
					{
						glTexCoord2f((float)m_pWorld->m_iPlanetoidTexTiling, i%2 == 0 ? 0.0f : (float)m_pWorld->m_iPlanetoidTexTiling);
					}
					else
					{
						glTexCoord2f((float)(j+1) * textureOffset, (float)i * textureOffset);
					}
				}
				else
				{
					if (m_pWorld->m_iPlanetoidTexTiling > 0)
					{
						glTexCoord2f(i%2 == 0 ? 0.0f : (float)m_pWorld->m_iPlanetoidTexTiling, (float)m_pWorld->m_iPlanetoidTexTiling);
					}
					else
					{
						glTexCoord2f((float)i * textureOffset, (float)(j+1) * textureOffset);
					}
				}

				glVertex3f(planetoid[l][i][j+1].x, planetoid[l][i][j+1].y, planetoid[l][i][j+1].z);
			}

			glEnd();
		}
	}

	glEndList();

	m_fBoundingSphereRadius = m_pPlanetoid->GetBoundingSphereRadius();
	m_vecDisplacement = m_pPlanetoid->GetDisplacement();

	// Planetoid display list created, we don't need the data anymore

	delete m_pPlanetoid;
	m_pPlanetoid = NULL;
}
