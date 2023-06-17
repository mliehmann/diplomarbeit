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

#include "camera.h"

CCamera::CCamera()
{
	m_pObject = NULL;
}

CCamera::CCamera(CObject* obj)
{
	m_pObject = obj;
}

CCamera::~CCamera()
{
}

void CCamera::SetObject(CObject* obj)
{
	m_pObject = obj;
}

void CCamera::Animate()
{
	if (m_pObject == NULL)
	{
		return;
	}

	// rotate camera (load matrix from object) and move camera to the position of the object

	glLoadMatrixf(m_pObject->m_faRotMatrix);
	glTranslatef(m_pObject->m_vecPosition.x, m_pObject->m_vecPosition.y, m_pObject->m_vecPosition.z);
}