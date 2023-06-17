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

#include "Planetoid.h"

CPlanetoid::CPlanetoid ()
{
	m_iIteration = 0;
	m_iComplexity = 0;
	m_fMininumSize = 0;
	m_fRadius = 0;
	m_fRoughness = 0;
	m_fBoundingSphereRadius = 0;
	m_planetoid = NULL;
	m_normals = NULL;
}

CPlanetoid::CPlanetoid (int iIteration, float fRadius, float fRoughness, float fMininumSize)
{
	m_fBoundingSphereRadius = 0;
	m_planetoid = NULL;
	m_normals = NULL;

	SetupPlanetoid (iIteration, fRadius, fRoughness, fMininumSize);
}

CPlanetoid::~CPlanetoid ()
{
	int i,l;

	// delete OQTM (Octahedral Quaternary Triangular Mesh)

	if (m_planetoid != NULL)
	{
		for (l = 0; l < 4; l++)
		{
			for (i = 0; i <= m_iComplexity; i++)
			{
				delete m_planetoid[l][i];
			}

			delete m_planetoid[l];
		}

		delete m_planetoid;
	}

	// delete normals

	if (m_normals != NULL)
	{
		for (l = 0; l < 4; l++)
		{
			for (i = 0; i <= m_iComplexity; i++)
			{
				delete m_normals[l][i];
			}

			delete m_normals[l];
		}

		delete m_normals;
	}
}

void CPlanetoid::SetupPlanetoid (int iIteration, float fRadius, float fRoughness, float fMininumSize)
{
	int i,l;

	// delete OQTM

	if (m_planetoid != NULL)
	{
		for (l = 0; l < 4; l++)
		{
			for (i = 0; i <= m_iComplexity; i++)
			{
				delete m_planetoid[l][i];
			}

			delete m_planetoid[l];
		}

		delete m_planetoid;
	}

	// delete normals

	if (m_normals != NULL)
	{
		for (l = 0; l < 4; l++)
		{
			for (i = 0; i <= m_iComplexity; i++)
			{
				delete m_normals[l][i];
			}

			delete m_normals[l];
		}

		delete m_normals;
	}

	m_iIteration = iIteration;
	m_iComplexity = 1 << m_iIteration;
	m_fMininumSize = fMininumSize;
	m_fRadius = fRadius;
	m_fRoughness = fRoughness;
	m_normals = NULL;
	m_planetoid = new CVector**[4];

	// compose OQTM (4 grids, filled later with values computed by the DiamondSquare Algorithm)

	for (l = 0; l < 4; l++)
	{
		m_planetoid[l] = new CVector*[m_iComplexity + 1];

		for (i = 0; i <= m_iComplexity; i++)
		{
			m_planetoid[l][i] = new CVector[m_iComplexity + 1];
		}
	}

	// corners of the OQTM (they form an octahedron -> Octahedral QTM!)

	m_planetoid[0][0][0].x = 0;
	m_planetoid[0][0][0].y = 1.0f;
	m_planetoid[0][0][0].z = 0;

	m_planetoid[0][m_iComplexity][0].x = 1.0f;
	m_planetoid[0][m_iComplexity][0].y = 0;
	m_planetoid[0][m_iComplexity][0].z = 0;

	m_planetoid[0][0][m_iComplexity].x = 0;
	m_planetoid[0][0][m_iComplexity].y = 0;
	m_planetoid[0][0][m_iComplexity].z = 1.0f;

	m_planetoid[0][m_iComplexity][m_iComplexity].x = 0;
	m_planetoid[0][m_iComplexity][m_iComplexity].y = -1.0f;
	m_planetoid[0][m_iComplexity][m_iComplexity].z = 0;


	m_planetoid[1][0][0].x = 0;
	m_planetoid[1][0][0].y = 1.0f;
	m_planetoid[1][0][0].z = 0;

	m_planetoid[1][m_iComplexity][0].x = 0;
	m_planetoid[1][m_iComplexity][0].y = 0;
	m_planetoid[1][m_iComplexity][0].z = -1.0f;

	m_planetoid[1][0][m_iComplexity].x = 1.0f;
	m_planetoid[1][0][m_iComplexity].y = 0;
	m_planetoid[1][0][m_iComplexity].z = 0;

	m_planetoid[1][m_iComplexity][m_iComplexity].x = 0;
	m_planetoid[1][m_iComplexity][m_iComplexity].y = -1.0f;
	m_planetoid[1][m_iComplexity][m_iComplexity].z = 0;


	m_planetoid[2][0][0].x = 0;
	m_planetoid[2][0][0].y = 1.0f;
	m_planetoid[2][0][0].z = 0;

	m_planetoid[2][m_iComplexity][0].x = -1.0f;
	m_planetoid[2][m_iComplexity][0].y = 0;
	m_planetoid[2][m_iComplexity][0].z = 0;

	m_planetoid[2][0][m_iComplexity].x = 0;
	m_planetoid[2][0][m_iComplexity].y = 0;
	m_planetoid[2][0][m_iComplexity].z = -1.0f;

	m_planetoid[2][m_iComplexity][m_iComplexity].x = 0;
	m_planetoid[2][m_iComplexity][m_iComplexity].y = -1.0f;
	m_planetoid[2][m_iComplexity][m_iComplexity].z = 0;


	m_planetoid[3][0][0].x = 0;
	m_planetoid[3][0][0].y = 1.0f;
	m_planetoid[3][0][0].z = 0;

	m_planetoid[3][m_iComplexity][0].x = 0;
	m_planetoid[3][m_iComplexity][0].y = 0;
	m_planetoid[3][m_iComplexity][0].z = 1.0f;

	m_planetoid[3][0][m_iComplexity].x = -1.0f;
	m_planetoid[3][0][m_iComplexity].y = 0;
	m_planetoid[3][0][m_iComplexity].z = 0;

	m_planetoid[3][m_iComplexity][m_iComplexity].x = 0;
	m_planetoid[3][m_iComplexity][m_iComplexity].y = -1.0f;
	m_planetoid[3][m_iComplexity][m_iComplexity].z = 0;

	/* each of the 4 parts of the OQTM consists of 2 triangles; these triangles will now
	   be subdivided rekursively for 'm_iIteration' times. The grid will be filled with the values of
	   the new coordinates at the position described by t1, t2 and t3.*/

	int t1[2], t2[2], t3[2];

	for (i = 0; i < 4; i++)
	{
		t1[0] = 0;
		t1[1] = 0;
		t2[0] = m_iComplexity;
		t2[1] = 0;
		t3[0] = 0;
		t3[1] = m_iComplexity;

		Subdivide (m_planetoid[i], m_planetoid[i][0][0], m_planetoid[i][m_iComplexity][0], m_planetoid[i][0][m_iComplexity],
			t1, t2, t3, m_iIteration);

		t1[0] = m_iComplexity;
		t1[1] = m_iComplexity;
		t2[0] = 0;
		t2[1] = m_iComplexity;
		t3[0] = m_iComplexity;
		t3[1] = 0;

		Subdivide (m_planetoid[i], m_planetoid[i][m_iComplexity][m_iComplexity], m_planetoid[i][0][m_iComplexity], m_planetoid[i][m_iComplexity][0],
			t1, t2, t3, m_iIteration);
	}
}

void CPlanetoid::Subdivide (CVector** grid, CVector v1, CVector v2, CVector v3, int* t1, int* t2, int* t3, int depth)
{
	if (depth == 0)
	{
		// max depth reached

		return;
	}

	CVector v12, v23, v31;
	int t12[2], t23[2], t31[2], i;

	// calculate vectors of new triangle (v12, v23, v31) and their grid-coordinates t12, t23, t31

		v12 = (v1 + v2) / 2.0f;
		v23 = (v2 + v3) / 2.0f;
		v31 = (v3 + v1) / 2.0f;

	for (i = 0; i < 2; i++)
	{
		t12[i] = (t1[i] + t2[i]) / 2;
		t23[i] = (t2[i] + t3[i]) / 2;
		t31[i] = (t3[i] + t1[i]) / 2;
	}

	// normalize vectors to scale them to unit sphere

	v12.Normalize();
	v23.Normalize();
	v31.Normalize();

	// transfer new vectors into grid

	grid[t12[0]][t12[1]] = v12;
	grid[t23[0]][t23[1]] = v23;
	grid[t31[0]][t31[1]] = v31;

	// subdivide the 4 new triangles

	Subdivide (grid, v1, v12, v31, t1, t12, t31, depth - 1);
	Subdivide (grid, v2, v23, v12, t2, t23, t12, depth - 1);
	Subdivide (grid, v3, v31, v23, t3, t31, t23, depth - 1);
	Subdivide (grid, v12, v23, v31, t12, t23, t31, depth - 1);
}

void CPlanetoid::ProjectTerrain ()
{
	if (m_planetoid == NULL)
	{
		return;
	}

	int i,j,l;

	// initialize 4 DiamondSquare-grids

	CDiamondSquare ds1, ds2, ds3, ds4;
	float*** grid = new float**[4];

	// calculate the 1. grid

	ds1.SetupGrid(m_iIteration, 1.0f, m_fRoughness, 0.0f);
	ds1.CalculateGrid();
	grid[0] = ds1.GetGrid ();

	ds2.SetupGrid(m_iIteration, 1.0f, m_fRoughness, 0.0f, CDiamondSquare::LEFT_EDGE |
		CDiamondSquare::BOTTOM_EDGE);
	grid[1] = ds2.GetGrid ();

	// initialize left and lower edge and calculate the 2. grid

	for (i = 0; i <= m_iComplexity; i++)
	{
		grid[1][0][i] = grid[0][i][0];
		grid[1][i][m_iComplexity] = grid[0][m_iComplexity][i];
	}

	ds2.CalculateGrid ();

	// initialize left and lower edge and calculate the 3. grid

	ds3.SetupGrid(m_iIteration, 1.0f, m_fRoughness, 0.0f, CDiamondSquare::LEFT_EDGE |
		CDiamondSquare::BOTTOM_EDGE);
	grid[2] = ds3.GetGrid ();

	for (i = 0; i <= m_iComplexity; i++)
	{
		grid[2][0][i] = grid[1][i][0];
		grid[2][i][m_iComplexity] = grid[1][m_iComplexity][i];
	}

	ds3.CalculateGrid ();

	// initialize all 4 edges and calculate the 4. grid

	ds4.SetupGrid(m_iIteration, 1.0f, m_fRoughness, 0.0f, CDiamondSquare::LEFT_EDGE |
		CDiamondSquare::BOTTOM_EDGE | CDiamondSquare::TOP_EDGE | CDiamondSquare::RIGHT_EDGE);
	grid[3] = ds4.GetGrid ();

	for (i = 0; i <= m_iComplexity; i++)
	{
		grid[3][0][i] = grid[2][i][0];
		grid[3][i][m_iComplexity] = grid[2][m_iComplexity][i];
		grid[3][i][0] = grid[0][0][i];
		grid[3][m_iComplexity][i] = grid[0][i][m_iComplexity];
	}

	ds4.CalculateGrid ();


	int count = 0;
	float max_dist = grid[0][0][0];
	float min_dist = grid[0][0][0];
	float min_size = m_fMininumSize * m_fRadius;
	float scale_factor;
	float tmp = 0.0f;
	CVector dist;

	// find min- and max-values

	for (l = 0; l < 4; l++)
	{
		for (i = 0; i <= m_iComplexity; i++)
		{
			for (j = 0; j <= m_iComplexity; j++)
			{
				if (grid[l][i][j] > max_dist)
				{
					max_dist = grid[l][i][j];
				}

				if (grid[l][i][j] < min_dist)
				{
					min_dist = grid[l][i][j];
				}
			}
		}
	}

	if (max_dist == 0)
	{
		max_dist = 1.0f;
	}

	scale_factor = (m_fRadius - min_size) / (max_dist - min_dist);

	// scale grids

	for (l = 0; l < 4; l++)
	{
		for (i = 0; i <= m_iComplexity; i++)
		{
			for (j = 0; j <= m_iComplexity; j++)
			{
				grid[l][i][j] = grid[l][i][j] * scale_factor + (m_fRadius + min_size) * 0.5f;
			}
		}
	}

	// calculate displacement-vector and project grids on OQTM

	for (l = 0; l < 4; l++)
	{
		for (i = 0; i <= m_iComplexity; i++)
		{
			for (j = 0; j <= m_iComplexity; j++)
			{
				m_planetoid[l][i][j] *= grid[l][i][j];

				switch(l)
				{
				case 0:
					count++;
					m_vecDisplacement += m_planetoid[l][i][j];
					break;
				case 1:
					if ((i != 0) && (j != m_iComplexity))
					{
						count++;
						m_vecDisplacement += m_planetoid[l][i][j];
					}
					break;

				case 2:
					if ((i != 0) && (j != m_iComplexity))
					{
						count++;
						m_vecDisplacement += m_planetoid[l][i][j];
					}
					break;

				case 3:
					if (!((i == 0) || (i == m_iComplexity)) && !((j == 0) || (j == m_iComplexity)))
					{
						count++;
						m_vecDisplacement += m_planetoid[l][i][j];
					}
					break;
				}
			}
		}
	}

	m_vecDisplacement /= (float)count;
	count = 0;

	// calculate bounding-sphere

	for (l = 0; l < 4; l++)
	{
		for (i = 0; i <= m_iComplexity; i++)
		{
			for (j = 0; j <= m_iComplexity; j++)
			{
				switch(l)
				{
				case 0:
					count++;
					dist = m_planetoid[l][i][j] - m_vecDisplacement;
					tmp = (float)sqrt(dist.x * dist.x +
						dist.y * dist.y +
						dist.z * dist.z);
					m_fBoundingSphereRadius += tmp;
					break;
				case 1:
					if ((i != 0) && (j != m_iComplexity))
					{
						count++;
						dist = m_planetoid[l][i][j] - m_vecDisplacement;
						tmp = (float)sqrt(dist.x * dist.x +
							dist.y * dist.y +
							dist.z * dist.z);
						m_fBoundingSphereRadius += tmp;
					}
					break;

				case 2:
					if ((i != 0) && (j != m_iComplexity))
					{
						count++;
						dist = m_planetoid[l][i][j] - m_vecDisplacement;
						tmp = (float)sqrt(dist.x * dist.x +
							dist.y * dist.y +
							dist.z * dist.z);
						m_fBoundingSphereRadius += tmp;
					}
					break;

				case 3:
					if (!((i == 0) || (i == m_iComplexity)) && !((j == 0) || (j == m_iComplexity)))
					{
						count++;
						dist = m_planetoid[l][i][j] - m_vecDisplacement;
						tmp = (float)sqrt(dist.x * dist.x +
							dist.y * dist.y +
							dist.z * dist.z);
						m_fBoundingSphereRadius += tmp;
					}
					break;
				}
			}
		}
	}

	m_fBoundingSphereRadius /= (float)count;

	delete grid;
}

void CPlanetoid::CalculateNormals ()
{
	if (m_planetoid == NULL || m_normals != NULL)
	{
		return;
	}

	int i,j,l;

	// create normal-grid and initialize with 0

	m_normals = new CVector**[4];

	for (l = 0; l < 4; l++)
	{
		m_normals[l] = new CVector*[m_iComplexity + 1];

		for (i = 0; i <= m_iComplexity; i++)
		{
			m_normals[l][i] = new CVector[m_iComplexity + 1];
		}
	}

	/* create temporal grids to save the normal vectors of the OQTM
	   (used to avoid calculating values twice or three times)*/

	CVector v1,v2;
	CVector*** tmp_normals = new CVector**[4];

	for (l = 0; l < 4; l++)
	{
		tmp_normals[l] = new CVector*[m_iComplexity * 2];

		for (i = 0; i < m_iComplexity; i++)
		{
			tmp_normals[l][i * 2] = new CVector[m_iComplexity];
			tmp_normals[l][i * 2 + 1] = new CVector[m_iComplexity];

			for (j = 0; j < m_iComplexity; j++)
			{
				v1 = m_planetoid[l][i][j + 1] - m_planetoid[l][i][j];

				v2 = m_planetoid[l][i + 1][j] - m_planetoid[l][i][j];

				tmp_normals[l][i * 2][j] = v1.CrossProduct(v2);
				tmp_normals[l][i * 2][j].Normalize();

				v1 = m_planetoid[l][i + 1][j] - m_planetoid[l][i + 1][j + 1];

				v2 = m_planetoid[l][i][j + 1] - m_planetoid[l][i + 1][j + 1];

				tmp_normals[l][i * 2 + 1][j] = v1.CrossProduct(v2);
				tmp_normals[l][i * 2 + 1][j].Normalize();
			}
		}
	}

	for (l = 0; l < 4; l++)
	{
		// calculate inner grid

		for (i = 1; i < m_iComplexity; i++)
		{
			for (j = 1; j < m_iComplexity; j++)
			{
				m_normals[l][i][j] = (tmp_normals[l][i * 2 - 1][j - 1] +
					tmp_normals[l][i * 2][j - 1] +
					tmp_normals[l][i * 2 + 1][j - 1] +
					tmp_normals[l][i * 2 - 2][j] +
					tmp_normals[l][i * 2 - 1][j] +
					tmp_normals[l][i * 2][j]) / 6.0f;
			}
		}

		// calculate all 4 edges of the grid

		// upper edge

		for (i = 1;  i < m_iComplexity; i++)
		{
			m_normals[l][i][0] = (tmp_normals[l][i * 2 - 2][0] +
				tmp_normals[l][i * 2 - 1][0] +
				tmp_normals[l][i * 2][0] +
				tmp_normals[(l + 1)%4][0][i - 1] +
				tmp_normals[(l + 1)%4][1][i - 1] +
				tmp_normals[(l + 1)%4][0][i]) / 6.0f;
		}

		// lower edge

		for (i = 1;  i < m_iComplexity; i++)
		{
			m_normals[l][i][m_iComplexity] = (tmp_normals[l][i * 2 - 1][m_iComplexity - 1] +
				tmp_normals[l][i * 2][m_iComplexity - 1] +
				tmp_normals[l][i * 2 + 1][m_iComplexity - 1] +
				tmp_normals[(l + 3)%4][m_iComplexity * 2 - 1][i - 1] +
				tmp_normals[(l + 3)%4][m_iComplexity * 2 - 2][i] +
				tmp_normals[(l + 3)%4][m_iComplexity * 2 - 1][i]) / 6.0f;
		}

		// left edge

		for (j = 1;  j < m_iComplexity; j++)
		{
			m_normals[l][0][j] = (tmp_normals[l][0][j - 1] +
				tmp_normals[l][1][j - 1] +
				tmp_normals[l][0][j] +
				tmp_normals[(l + 3)%4][j * 2 - 2][0] +
				tmp_normals[(l + 3)%4][j * 2 - 1][0] +
				tmp_normals[(l + 3)%4][j * 2][0]) / 6.0f;
		}

		// right edge

		for (j = 1;  j < m_iComplexity; j++)
		{
			m_normals[l][m_iComplexity][j] = (tmp_normals[l][m_iComplexity * 2 - 1][j - 1] +
				tmp_normals[l][m_iComplexity * 2 - 2][j] +
				tmp_normals[l][m_iComplexity * 2 - 1][j] +
				tmp_normals[(l + 1)%4][j * 2 - 1][m_iComplexity - 1] +
				tmp_normals[(l + 1)%4][j * 2][m_iComplexity - 1] +
				tmp_normals[(l + 1)%4][j * 2 + 1][m_iComplexity - 1]) / 6.0f;
		}

		// calculate the 4 corners

		// upper left

		m_normals[l][0][0] = (tmp_normals[l][0][0] +
			tmp_normals[(l + 1)%4][0][0] +
			tmp_normals[(l + 2)%4][0][0] +
			tmp_normals[(l + 3)%4][0][0]) / 4.0f;

		// lower left

		m_normals[l][0][m_iComplexity] = (tmp_normals[l][0][m_iComplexity - 1] +
			tmp_normals[l][1][m_iComplexity - 1] +
			tmp_normals[(l + 3)%4][m_iComplexity * 2 - 2][0] +
			tmp_normals[(l + 3)%4][m_iComplexity * 2 - 1][0]) / 4.0f;

		// upper right

		m_normals[l][m_iComplexity][0] = (tmp_normals[l][m_iComplexity * 2 - 2][0] +
			tmp_normals[l][m_iComplexity * 2 - 1][0] +
			tmp_normals[(l + 1)%4][0][m_iComplexity - 1] +
			tmp_normals[(l + 1)%4][1][m_iComplexity - 1]) / 4.0f;

		// lower right

		m_normals[l][m_iComplexity][m_iComplexity] = (tmp_normals[l][m_iComplexity * 2 - 1][m_iComplexity - 1] +
			tmp_normals[(l + 1)%4][m_iComplexity * 2 - 1][m_iComplexity - 1] +
			tmp_normals[(l + 2)%4][m_iComplexity * 2 - 1][m_iComplexity - 1] +
			tmp_normals[(l + 3)%4][m_iComplexity * 2 - 1][m_iComplexity - 1]) / 4.0f;
	}

	// delete temporal grids

	for (l = 0; l < 4; l++)
	{
		for (i = 0; i < m_iComplexity * 2; i++)
		{
			delete tmp_normals[l][i];
		}

		delete tmp_normals[l];
	}

	delete tmp_normals;
}

CVector*** CPlanetoid::GetPlanetoid ()
{
	return m_planetoid;
}

CVector*** CPlanetoid::GetNormals ()
{
	return m_normals;
}

int CPlanetoid::GetIteration ()
{
	return m_iIteration;
}

int CPlanetoid::GetComplexity ()
{
	return m_iComplexity;
}

float CPlanetoid::GetMininumSize ()
{
	return m_fMininumSize;
}

float CPlanetoid::GetRadius ()
{
	return m_fRadius;
}

float CPlanetoid::GetRoughness ()
{
	return m_fRoughness;
}

float CPlanetoid::GetBoundingSphereRadius ()
{
	return m_fBoundingSphereRadius;
}

CVector CPlanetoid::GetDisplacement ()
{
	return m_vecDisplacement;
}
