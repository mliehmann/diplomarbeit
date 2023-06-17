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

#include "Arena.h"

CArena::CArena()
{
}

CArena::CArena(CWorld* world)
{
	m_pWorld = world;
	m_fWidth = (float)m_pWorld->m_iArenaSize;

	// CObject attributes

	m_vecPosition = CVector(0,0,0);

	for (int i = 0; i < 16; i++)
	{
		if (i%5 == 0)
		{
			m_faRotMatrix[i] = 1.0f;
		}
		else
		{
			m_faRotMatrix[i] = 0.0f;
		}
	}

	float half_width = m_fWidth * 0.5f;

	m_fSize = 0.0f;
	m_iIdent = 0;

	BuildArena();
}

CArena::~CArena()
{
}

void CArena::BuildArena()
{
	float half_width = m_fWidth / 2.0f;

	// create ODE-Geom-Planes

	m_ODEGeomPlanes[0] = dCreatePlane (m_pWorld->m_ODESpace, -1.0, 0.0, 0.0, -half_width);
	dGeomSetData (m_ODEGeomPlanes[0], this);
	m_ODEGeomPlanes[1] = dCreatePlane (m_pWorld->m_ODESpace, 1.0, 0.0, 0.0, -half_width);
	dGeomSetData (m_ODEGeomPlanes[1], this);
	m_ODEGeomPlanes[2] = dCreatePlane (m_pWorld->m_ODESpace, 0.0, -1.0, 0.0, -half_width);
	dGeomSetData (m_ODEGeomPlanes[2], this);
	m_ODEGeomPlanes[3] = dCreatePlane (m_pWorld->m_ODESpace, 0.0, 1.0, 0.0, -half_width);
	dGeomSetData (m_ODEGeomPlanes[3], this);
	m_ODEGeomPlanes[4] = dCreatePlane (m_pWorld->m_ODESpace, 0.0, 0.0, -1.0, -half_width);
	dGeomSetData (m_ODEGeomPlanes[4], this);
	m_ODEGeomPlanes[5] = dCreatePlane (m_pWorld->m_ODESpace, 0.0, 0.0, 1.0, -half_width);
	dGeomSetData (m_ODEGeomPlanes[5], this);

	// if lights are enabled, set their position and color

	if (m_pWorld->m_bLight0Enabled == true)
	{
		glEnable(GL_LIGHT0);
		m_faLight0Position[0] = m_pWorld->m_faLight0Position[0];
		m_faLight0Position[1] = m_pWorld->m_faLight0Position[1];
		m_faLight0Position[2] = m_pWorld->m_faLight0Position[2];
		m_faLight0Position[3] = 0.0f;
		m_faLight0Color[0] = m_pWorld->m_faLight0Color[0];
		m_faLight0Color[1] = m_pWorld->m_faLight0Color[1];
		m_faLight0Color[2] = m_pWorld->m_faLight0Color[2];
		m_faLight0Color[3] = 1.0f;
		glLightfv(GL_LIGHT0, GL_POSITION, m_faLight0Position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, m_faLight0Color);
		glLightfv(GL_LIGHT0, GL_SPECULAR, m_faLight0Color);
	}

	if (m_pWorld->m_bLight1Enabled == true)
	{
		glEnable(GL_LIGHT1);
		m_faLight1Position[0] = m_pWorld->m_faLight1Position[0];
		m_faLight1Position[1] = m_pWorld->m_faLight1Position[1];
		m_faLight1Position[2] = m_pWorld->m_faLight1Position[2];
		m_faLight1Position[3] = 0.0f;
		m_faLight1Color[0] = m_pWorld->m_faLight1Color[0];
		m_faLight1Color[1] = m_pWorld->m_faLight1Color[1];
		m_faLight1Color[2] = m_pWorld->m_faLight1Color[2];
		m_faLight1Color[3] = 1.0f;
		glLightfv(GL_LIGHT1, GL_POSITION, m_faLight1Position);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, m_faLight1Color);
		glLightfv(GL_LIGHT1, GL_SPECULAR, m_faLight1Color);
	}

	if (m_pWorld->m_bLight2Enabled == true)
	{
		glEnable(GL_LIGHT2);
		m_faLight2Position[0] = m_pWorld->m_faLight2Position[0];
		m_faLight2Position[1] = m_pWorld->m_faLight2Position[1];
		m_faLight2Position[2] = m_pWorld->m_faLight2Position[2];
		m_faLight2Position[3] = 0.0f;
		m_faLight2Color[0] = m_pWorld->m_faLight2Color[0];
		m_faLight2Color[1] = m_pWorld->m_faLight2Color[1];
		m_faLight2Color[2] = m_pWorld->m_faLight2Color[2];
		m_faLight2Color[3] = 1.0f;
		glLightfv(GL_LIGHT2, GL_POSITION, m_faLight2Position);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, m_faLight2Color);
		glLightfv(GL_LIGHT2, GL_SPECULAR, m_faLight2Color);
	}

	float lmodel_ambient[4];

	// set ambient light

	lmodel_ambient[0] = m_pWorld->m_faLightAmbient[0];
	lmodel_ambient[1] = m_pWorld->m_faLightAmbient[1];
	lmodel_ambient[2] = m_pWorld->m_faLightAmbient[2];
	lmodel_ambient[3] = 1.0f;
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}

void CArena::OnDraw(CCamera *camera)
{
	float half_width = m_fWidth / 2.0f;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_LIGHTING);

	// light and skybox move with camera

	// draw light

	glPushMatrix();
	glTranslatef(-camera->m_pObject->m_vecPosition.x, -camera->m_pObject->m_vecPosition.y, -camera->m_pObject->m_vecPosition.z);

	if (m_pWorld->m_bLight0Enabled == true)
	{
		glLightfv(GL_LIGHT0, GL_POSITION, m_faLight0Position);
	}

	if (m_pWorld->m_bLight1Enabled == true)
	{
		glLightfv(GL_LIGHT1, GL_POSITION, m_faLight1Position);
	}

	if (m_pWorld->m_bLight2Enabled == true)
	{
		glLightfv(GL_LIGHT2, GL_POSITION, m_faLight2Position);
	}

	// draw skybox

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDepthMask(FALSE);

	glBindTexture(GL_TEXTURE_2D, m_pWorld->m_texSkybox[0].texID);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(half_width, half_width, half_width);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(half_width, -half_width, half_width);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-half_width, -half_width, half_width);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-half_width, half_width, half_width);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_pWorld->m_texSkybox[1].texID);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(half_width, half_width, -half_width);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(half_width, -half_width, -half_width);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(half_width, -half_width, half_width);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(half_width, half_width, half_width);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_pWorld->m_texSkybox[2].texID);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-half_width, half_width, -half_width);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-half_width, -half_width, -half_width);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(half_width, -half_width, -half_width);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(half_width, half_width, -half_width);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_pWorld->m_texSkybox[3].texID);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-half_width, half_width, half_width);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-half_width, -half_width, half_width);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-half_width, -half_width, -half_width);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-half_width, half_width, -half_width);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_pWorld->m_texSkybox[4].texID);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-half_width, half_width, -half_width);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(half_width, half_width, -half_width);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(half_width, half_width, half_width);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-half_width, half_width, half_width);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_pWorld->m_texSkybox[5].texID);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-half_width, -half_width, half_width);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(half_width, -half_width, half_width);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(half_width, -half_width, -half_width);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-half_width, -half_width, -half_width);
	glEnd();

	glDepthMask(TRUE);
	glPopMatrix();

	// draw arena

	if (m_pWorld->m_bArenaTransparency == true)
	{
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}

	glBindTexture(GL_TEXTURE_2D, m_pWorld->m_texArena.texID);

	glColor4fv(m_pWorld->m_faArenaColor);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(half_width, half_width, half_width);
		glTexCoord2f(0.0f, 10.0f);
		glVertex3f(half_width, -half_width, half_width);
		glTexCoord2f(10.0f, 10.0f);
		glVertex3f(-half_width, -half_width, half_width);
		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(-half_width, half_width, half_width);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(half_width, half_width, -half_width);
		glTexCoord2f(0.0f, 10.0f);
		glVertex3f(half_width, -half_width, -half_width);
		glTexCoord2f(10.0f, 10.0f);
		glVertex3f(half_width, -half_width, half_width);
		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(half_width, half_width, half_width);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-half_width, half_width, -half_width);
		glTexCoord2f(0.0f, 10.0f);
		glVertex3f(-half_width, -half_width, -half_width);
		glTexCoord2f(10.0f, 10.0f);
		glVertex3f(half_width, -half_width, -half_width);
		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(half_width, half_width, -half_width);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-half_width, half_width, half_width);
		glTexCoord2f(0.0f, 10.0f);
		glVertex3f(-half_width, -half_width, half_width);
		glTexCoord2f(10.0f, 10.0f);
		glVertex3f(-half_width, -half_width, -half_width);
		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(-half_width, half_width, -half_width);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-half_width, half_width, -half_width);
		glTexCoord2f(0.0f, 10.0f);
		glVertex3f(half_width, half_width, -half_width);
		glTexCoord2f(10.0f, 10.0f);
		glVertex3f(half_width, half_width, half_width);
		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(-half_width, half_width, half_width);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-half_width, -half_width, half_width);
		glTexCoord2f(0.0f, 10.0f);
		glVertex3f(half_width, -half_width, half_width);
		glTexCoord2f(10.0f, 10.0f);
		glVertex3f(half_width, -half_width, -half_width);
		glTexCoord2f(10.0f, 0.0f);
		glVertex3f(-half_width, -half_width, -half_width);
	glEnd();

	if (m_pWorld->m_bArenaTransparency == true)
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
}