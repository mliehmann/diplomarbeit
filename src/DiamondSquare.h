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
	DiamondSquare.h

	Author: Markus Liehmann
	Date: 02/08/2004
	Description: The CDiamondSquare class represents a terrain-grid with 2^n * 2^n (n = Iteration + 1)
		nodes. Each node consists of a float value, the depth. The function CalculateGrid fills the
		grid with values using the Diamond Square Algorithm. Normals can be computed using the function
		CalculateNormals (the size of the grid, m_fSize, is only important if normals are needed).
*/

#ifndef __MM_DIAMOND_SQUARE_H__
#define __MM_DIAMOND_SQUARE_H__

#include "stdlib.h"
#include "vector.h"

class CDiamondSquare
{
public:

	/* InitMode tells CalculateGrid to leave certain edges untouched. These edges can then be
	   defined with any values.*/

	enum InitMode {TOP_EDGE = 0x0001,
		BOTTOM_EDGE = 0x0002,
		LEFT_EDGE = 0x0004,
		RIGHT_EDGE = 0x0008};

	CDiamondSquare ();
	~CDiamondSquare ();
	CDiamondSquare (int iIteration, float fSize, float fRoughness, float fInit, unsigned short iInitMode = 0);
	void SetupGrid (int iIteration, float fSize, float fRoughness, float fInit, unsigned short iInitMode = 0);
	void CalculateGrid ();
	void CalculateNormals ();
	float** GetGrid ();
	CVector** GetNormals ();
	int GetIteration ();
	int GetComplexity ();
	float GetSize ();
	float GetRoughness ();
	float GetInit ();

private:
	float** m_grid;
	CVector** m_normals;
	int m_iIteration;
	int m_iComplexity;
	int m_iInitMode;
	float m_fSize;
	float m_fRoughness;
	float m_fInit;
};

#endif