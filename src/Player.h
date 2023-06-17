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
	Player.h

	Author: Markus Liehmann
	Date: 02/08/2004
	Description: The CPlayer class is derived from CObject and represents the player. The player object
		reacts on user input and is also an object in the ODE world (just like the planetoids). It can
		rotate around the x-, y- and z-axis, accelerate and shoot.
*/

#ifndef __MM_PLAYER_H__
#define __MM_PLAYER_H__

#include "camera.h"
#include "object.h"
#include "Arena.h"
#include "world.h"
#include "PlasmaShot.h"
#include <ode/ode.h>

class CWorld;

class CPlayer : public CObject
{
private:
	bool m_bAccelerate;
	bool m_bBrake;
	bool m_bPlayThrust;
	bool m_bFireWeapon;
	bool m_bIsHit;
	int m_iChannelThrust;
	float m_fThrust;
	float m_fXDegree;
	float m_fYDegree;
	float m_fZDegree;
	float m_fHitDelay;
	float m_fMass;

protected:
	void OnCollision(CObject *collisionObject);
	void OnPrepare();
	void OnAnimate(float deltaTime);

public:
	CWorld* m_pWorld;
	dGeomID m_ODEGeom;
	dBodyID m_ODEBody;
	int m_iHealth;
	float m_fFireDelay;

	CPlayer(CWorld* world);
	~CPlayer();

	void RotateX(float degree);
	void RotateY(float degree);
	void RotateZ(float degree);
	void AccelerateOn();
	void AccelerateOff();
	void BrakeOn();
	void BrakeOff();
	void FireWeaponOn();
	void FireWeaponOff();
	void FireWeapon(float deltaTime);
	bool IsHit();
};

#endif