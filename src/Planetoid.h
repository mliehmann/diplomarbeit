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
	Planetoid.h

	Author: Markus Liehmann
	Date: 02/08/2004
	Description: The CPlanetoid class represents the visual information for rendering a planetoid.
		The planetoid is calculated using an Octahedral Quaternary Triangular Mesh (OQTM) and 4
		DiamondSquare-calculated terrain-grids, which will be projected onto the OQTM using the
		function ProjectTerrain. The function GetDisplacement returns the displacement to zero, the
		function GetBoundingSphereRadius returns the average of all vector lengths in the planetoid
		(outgoing from displacement).
*/

#ifndef __MM_PLANETOID_H__
#define __MM_PLANETOID_H__

#include "vector.h"
#include "DiamondSquare.h"

class CPlanetoid
{
public:
	CPlanetoid ();
	CPlanetoid (int iIteration, float fRadius, float fRoughness, float fMinimumSize);
	~CPlanetoid ();
	void SetupPlanetoid (int iIteration, float fRadius, float fRoughness, float fMininumSize);
	void ProjectTerrain ();
	void CalculateNormals ();
	CVector*** GetPlanetoid ();
	CVector*** GetNormals ();
	int GetIteration ();
	int GetComplexity ();
	float GetMininumSize ();
	float GetRadius ();
	float GetRoughness ();
	float GetBoundingSphereRadius ();
	CVector GetDisplacement ();

private:
	void Subdivide (CVector** grid, CVector v1, CVector v2, CVector v3, int* t1, int* t2, int* t3, int depth);
	CVector*** m_planetoid;
	CVector*** m_normals;
	CVector m_vecDisplacement;
	int m_iIteration;
	int m_iComplexity;
	float m_fMininumSize;
	float m_fRadius;
	float m_fRoughness;
	float m_fBoundingSphereRadius;
};

#endif