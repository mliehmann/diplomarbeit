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

#include "Player.h"

CPlayer::CPlayer(CWorld* world)
{
	m_pWorld = world;
	m_fMass = m_pWorld->m_fPlayerMass;

	// player is a sphere in both ODE world and space

	m_ODEGeom = dCreateSphere (m_pWorld->m_ODESpace, 1.0);	// create ODE Geom
	dGeomSetData (m_ODEGeom, this);							// set this-pointer as Geom-data (needed for collision-callback)
	m_ODEBody = dBodyCreate (m_pWorld->m_ODEWorld);			// create ODE Body
	dGeomSetBody (m_ODEGeom, m_ODEBody);					// attach Body to Geom
	dBodySetPosition(m_ODEBody, 0.0, 0.0, 0.0);				// player starts in the middle of the arena
	dMass player_mass;
	float mass = m_fMass;
	dMassSetSphereTotal (&player_mass, mass, 1.0);			// set mass type sphere
	dBodySetMass (m_ODEBody, &player_mass);					// set player mass

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

	// set CObject data

	m_vecPosition = CVector(0.0f, 0.0f, 0.0f);
	m_fSize = 1.0f;
	m_iIdent = 1;

	// set start values

	m_bAccelerate = false;
	m_bBrake = false;
	m_bIsHit = false;
	m_fXDegree = 0.0f;
	m_fYDegree = 0.0f;
	m_fZDegree = 0.0f;
	m_fThrust = m_pWorld->m_fPlayerThrust;
	m_iHealth = m_pWorld->m_iPlayerHealth;
	m_fHitDelay = 1.0f;						// HUD shivers for 1 second after player is hit

	m_fFireDelay = 0.0f;

	m_bPlayThrust = false;

	m_iChannelThrust = -1;
}

CPlayer::~CPlayer()
{
	dGeomDestroy (m_ODEGeom);
	dBodyDestroy (m_ODEBody);
}

void CPlayer::OnPrepare()
{
	if ((m_bIsHit == true) && (m_fHitDelay <= 0.0f))
	{
		// reset hit

		m_bIsHit = false;
		m_fHitDelay = 1.0f;
	}
}

void CPlayer::OnCollision(CObject *collisionObject)
{
	if (collisionObject->m_iIdent == 2)
	{
		m_iHealth -= m_pWorld->m_iHealthLoss;

		// player hit, reset hit delay for HUD shiver

		m_bIsHit = true;
		m_fHitDelay = 1.0f;
	}
}

void CPlayer::OnAnimate(float deltaTime)
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

	if (m_bIsHit == true)
	{
		// player hit active - decrease hit delay

		m_fHitDelay -= deltaTime;
	}

	if ((m_bFireWeapon == true) && (m_fFireDelay >= m_pWorld->m_fPlasmaFrequency))
	{
		// plasma reloaded, fire weapon

		FireWeapon(deltaTime);
		m_fFireDelay = 0.0f;
	}

	m_fFireDelay += deltaTime;

	CVector vec;
	float thrust2 = 0.1f * m_fThrust;					// secondary thruster is 1/10 of main thruster
	float factor = thrust2 * 0.01f / deltaTime;			// scale factor for force and torque (mouse motion)

	if (m_fXDegree != 0.0f)
	{
		// calculate torque axis (matrix multiplikation)

		vec.x = factor * m_fXDegree * m_faRotMatrix[0];
		vec.y = factor * m_fXDegree * m_faRotMatrix[4];
		vec.z = factor * m_fXDegree * m_faRotMatrix[8];

		dBodyAddRelTorque(m_ODEBody, vec.x, vec.y, vec.z);
		m_fXDegree = 0.0f;
	}

	if (m_fYDegree != 0.0f)
	{
		// calculate torque axis (matrix multiplikation)

		vec.x = factor * m_fYDegree * m_faRotMatrix[1];
		vec.y = factor * m_fYDegree * m_faRotMatrix[5];
		vec.z = factor * m_fYDegree * m_faRotMatrix[9];

		dBodyAddRelTorque(m_ODEBody, vec.x, vec.y, vec.z);
		m_fYDegree = 0.0f;
	}

	if (m_fZDegree != 0.0f)
	{
		// calculate torque axis (matrix multiplikation)

		vec.x = thrust2 * m_fZDegree * m_faRotMatrix[2];
		vec.y = thrust2 * m_fZDegree * m_faRotMatrix[6];
		vec.z = thrust2 * m_fZDegree * m_faRotMatrix[10];

		dBodyAddRelTorque(m_ODEBody, vec.x, vec.y, vec.z);
		m_fZDegree = 0.0f;
	}

	if (m_bAccelerate == true)
	{
		// calculate force vector (matrix multiplikation)

		vec.x = m_fThrust * m_faRotMatrix[2];
		vec.y = m_fThrust * m_faRotMatrix[6];
		vec.z = m_fThrust * m_faRotMatrix[10];

		dBodyAddForce(m_ODEBody, vec.x, vec.y, vec.z);
	}

	if (m_bBrake == true)
	{
		// calculate torques for braking (stopping player rotation)

		CVector x_axis;
		x_axis.x = -thrust2 * m_faRotMatrix[0];
		x_axis.y = -thrust2 * m_faRotMatrix[4];
		x_axis.z = -thrust2 * m_faRotMatrix[8];

		CVector y_axis;
		y_axis.x = -thrust2 * m_faRotMatrix[1];
		y_axis.y = -thrust2 * m_faRotMatrix[5];
		y_axis.z = -thrust2 * m_faRotMatrix[9];

		CVector z_axis;
		z_axis.x = -thrust2 * m_faRotMatrix[2];
		z_axis.y = -thrust2 * m_faRotMatrix[6];
		z_axis.z = -thrust2 * m_faRotMatrix[10];

		// get current angular velocity from ODE

		const float* angularVel = dBodyGetAngularVel (m_ODEBody);

		// calculate minimum angular velocity that can be corrected by normal braking
		// equation: angVel = (torque * deltaTime) / (0.4 * mass * radius * radius)
		// player is represented by a sphere (therefore 0.4) and has radius 1

		float minAngularVel = (thrust2 * deltaTime) / (0.4f * m_fMass);

		int x_sgn = angularVel[0] > 0 ? 1 : angularVel[0] < 0 ? -1 : 0;
		int y_sgn = angularVel[1] > 0 ? 1 : angularVel[1] < 0 ? -1 : 0;
		int z_sgn = angularVel[2] > 0 ? 1 : angularVel[2] < 0 ? -1 : 0;

		// check if angular velocity is bigger than the mininum angular velocity

		if (fabs(angularVel[0]) > minAngularVel)
		{
			dBodyAddRelTorque(m_ODEBody, (float)x_sgn * x_axis.x, (float)x_sgn * x_axis.y, (float)x_sgn * x_axis.z);
		}
		else
		{
			// remove remaining angular velocity

			x_axis.x = -angularVel[0] * m_faRotMatrix[0] * m_fMass;
			x_axis.y = -angularVel[0] * m_faRotMatrix[4] * m_fMass;
			x_axis.z = -angularVel[0] * m_faRotMatrix[8] * m_fMass;
			dBodyAddRelTorque(m_ODEBody, x_axis.x, x_axis.y, x_axis.z);
		}

		if (fabs(angularVel[1]) > minAngularVel)
		{
			dBodyAddRelTorque(m_ODEBody, (float)y_sgn * y_axis.x, (float)y_sgn * y_axis.y, (float)y_sgn * y_axis.z);
		}
		else
		{
			// remove remaining angular velocity

			y_axis.x = -angularVel[1] * m_faRotMatrix[1] * m_fMass;
			y_axis.y = -angularVel[1] * m_faRotMatrix[5] * m_fMass;
			y_axis.z = -angularVel[1] * m_faRotMatrix[9] * m_fMass;
			dBodyAddRelTorque(m_ODEBody, y_axis.x, y_axis.y, y_axis.z);
		}

		if (fabs(angularVel[2]) > minAngularVel)
		{
			dBodyAddRelTorque(m_ODEBody, (float)z_sgn * z_axis.x, (float)z_sgn * z_axis.y, (float)z_sgn * z_axis.z);
		}
		else
		{
			// remove remaining angular velocity

			z_axis.x = -angularVel[2] * m_faRotMatrix[2] * m_fMass;
			z_axis.y = -angularVel[2] * m_faRotMatrix[6] * m_fMass;
			z_axis.z = -angularVel[2] * m_faRotMatrix[10] * m_fMass;
			dBodyAddRelTorque(m_ODEBody, z_axis.x, z_axis.y, z_axis.z);
		}
	}
}

void CPlayer::RotateX(float degree)
{
	m_fXDegree = degree;
}

void CPlayer::RotateY(float degree)
{
	m_fYDegree = degree;
}

void CPlayer::RotateZ(float degree)
{
	m_fZDegree = degree;
}

void CPlayer::AccelerateOn()
{
	if (m_bPlayThrust == false)
	{
		m_iChannelThrust = Mix_PlayChannel(-1, m_pWorld->m_pSampleThrust, -1);
		m_bPlayThrust = true;
	}

	m_bAccelerate = true;
}

void CPlayer::AccelerateOff()
{
	if (m_bPlayThrust == true)
	{
		Mix_HaltChannel(m_iChannelThrust);
		m_bPlayThrust = false;
	}

	m_bAccelerate = false;
}

void CPlayer::BrakeOn()
{
	m_bBrake = true;
}

void CPlayer::BrakeOff()
{
	m_bBrake = false;
}

void CPlayer::FireWeaponOn()
{
	m_bFireWeapon = true;
}

void CPlayer::FireWeaponOff()
{
	m_bFireWeapon = false;
}

void CPlayer::FireWeapon(float deltaTime)
{
	Mix_PlayChannel(-1, m_pWorld->m_pSamplePlasma, 0);

	// create plasma shot and attach it to player

	CPlasmaShot* plasma = new CPlasmaShot(m_pWorld, deltaTime);
	plasma->AttachTo(m_pWorld->m_pPlayer);
}

bool CPlayer::IsHit()
{
	return m_bIsHit;
}