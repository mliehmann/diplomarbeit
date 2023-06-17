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

#include "DiamondSquare.h"

CDiamondSquare::CDiamondSquare ()
{
	m_iIteration = 0;
	m_fSize = 0;
	m_fRoughness = 0;
	m_fInit = 0;
	m_iComplexity = 0;

	m_grid = NULL;
	m_normals = NULL;
}

CDiamondSquare::CDiamondSquare (int iIteration, float fSize, float fRoughness, float fInit, unsigned short iInitMode)
{
	m_grid = NULL;
	m_normals = NULL;

	SetupGrid (iIteration, fSize, fRoughness, fInit);
}

CDiamondSquare::~CDiamondSquare ()
{
	int i;

	// delete grid

	if (m_grid != NULL)
	{
		for (i = 0; i <= m_iComplexity; i++)
		{
			delete m_grid[i];
		}

		delete m_grid;
	}

	// delete normals

	if (m_normals != NULL)
	{
		for (i = 0; i <= m_iComplexity; i++)
		{
			delete m_normals[i];
		}

		delete m_normals;
	}
}

void CDiamondSquare::SetupGrid (int iIteration, float fSize, float fRoughness, float fInit, unsigned short iInitMode)
{
	int i,j;

	// delete grid

	if (m_grid != NULL)
	{
		for (i = 0; i <= m_iComplexity; i++)
		{
			delete m_grid[i];
		}

		delete m_grid;
	}

	// delete normals

	if (m_normals != NULL)
	{
		for (i = 0; i <= m_iComplexity; i++)
		{
			delete m_normals[i];
		}

		delete m_normals;
	}

	m_iIteration = iIteration;
	m_fSize = fSize;
	m_fRoughness = fRoughness;
	m_fInit = fInit;
	m_iInitMode = iInitMode;
	m_iComplexity = 1 << iIteration;

	m_grid = NULL;
	m_normals = NULL;

	// create terrain-grid and set all nodes to 0

	m_grid = new float*[m_iComplexity + 1];

	for (i = 0; i <= m_iComplexity; i++)
	{
		m_grid[i] = new float[m_iComplexity + 1];

		for (j = 0; j <= m_iComplexity; j++)
		{
			m_grid[i][j] = 0.0;
		}
	}
}

void CDiamondSquare::CalculateGrid ()
{
	if (m_grid == NULL)
	{
		return;
	}

	int i,j,k;

	// initialize 4 corners with values of interval [-m_fInit; +m_fInit]

	if (((m_iInitMode & LEFT_EDGE) != LEFT_EDGE) && ((m_iInitMode & TOP_EDGE) != TOP_EDGE))
	{
		m_grid[0][0] = (float)-1.0 * m_fInit + (float)rand() * (float)(2.0 * m_fInit / RAND_MAX);
	}

	if (((m_iInitMode & LEFT_EDGE) != LEFT_EDGE) && ((m_iInitMode & BOTTOM_EDGE) != BOTTOM_EDGE))
	{
		m_grid[0][m_iComplexity] = (float)-1.0 * m_fInit + (float)rand() * (float)(2.0 * m_fInit / RAND_MAX);
	}

	if (((m_iInitMode & RIGHT_EDGE) != RIGHT_EDGE) && ((m_iInitMode & TOP_EDGE) != TOP_EDGE))
	{
		m_grid[m_iComplexity][0] = (float)-1.0 * m_fInit + (float)rand() * (float)(2.0 * m_fInit / RAND_MAX);
	}

	if (((m_iInitMode & RIGHT_EDGE) != RIGHT_EDGE) && ((m_iInitMode & BOTTOM_EDGE) != BOTTOM_EDGE))
	{
		m_grid[m_iComplexity][m_iComplexity] = (float)-1.0 * m_fInit + (float)rand() * (float)(2.0 * m_fInit / RAND_MAX);
	}

	// use Diamond Square Algorithm to fill grid with depth information

	float rough = m_fRoughness;

	for (i = 1; i <= m_iIteration; i++)
	{
		int width = m_iComplexity / (1 << i);

		for (j = 0; j < 1 << (i - 1); j++)
		{
			for (k = 0; k < 1 << (i - 1); k++)
			{
				int x = width + j * width * 2;
				int y = width + k * width * 2;
				int tmp;

				// Diamond Step

				m_grid[x][y] = (m_grid[x - width][y - width] + m_grid[x + width][y - width] +
					m_grid[x + width][y + width] + m_grid[x - width][y + width]) / (float)4.0 +
					(-1.0f * rough + (float)rand() * 2.0f * rough / (float)RAND_MAX);

				// Square Step

				if (((m_iInitMode & LEFT_EDGE) != LEFT_EDGE) || (x - width != 0))
				{
					tmp = x - 2 * width;
					m_grid[x - width][y] = (m_grid[tmp < 0 ? x : abs(tmp)][y] + m_grid[x - width][y - width] +
						m_grid[x][y] + m_grid[x - width][y + width]) / (float)4.0 +
						(-1.0f * rough + (float)rand() * 2.0f * rough / (float)RAND_MAX);
				}

				if (((m_iInitMode & TOP_EDGE) != TOP_EDGE) || (y - width != 0))
				{
					tmp = y - 2 * width;
					m_grid[x][y - width] = (m_grid[x - width][y - width] + m_grid[x][tmp < 0 ? y : abs(tmp)] +
						m_grid[x + width][y - width] + m_grid[x][y]) / (float)4.0 +
						(-1.0f * rough + (float)rand() * 2.0f * rough / (float)RAND_MAX);
				}

				if (((m_iInitMode & RIGHT_EDGE) != RIGHT_EDGE) || (x + width != m_iComplexity))
				{
					tmp = x + 2 * width;
					m_grid[x + width][y] = (m_grid[x][y] + m_grid[x + width][y - width] +
						m_grid[tmp > m_iComplexity ? x : tmp][y] + m_grid[x + width][y + width]) / (float)4.0 +
						(-1.0f * rough + (float)rand() * 2.0f * rough / (float)RAND_MAX);
				}

				if (((m_iInitMode & BOTTOM_EDGE) != BOTTOM_EDGE) || (y + width != m_iComplexity))
				{
					tmp = y + 2 * width;
					m_grid[x][y + width] = (m_grid[x - width][y + width] + m_grid[x][y] +
						m_grid[x + width][y + width] + m_grid[x][tmp > m_iComplexity ? y : tmp]) / (float)4.0 +
						(-1.0f * rough + (float)rand() * 2.0f * rough / (float)RAND_MAX);
				}
			}
		}

		rough *= m_fRoughness;
	}
}

void CDiamondSquare::CalculateNormals ()
{
	if ((m_grid == NULL) || (m_normals != NULL))
	{
		return;
	}

	int i,j;

	// create normal-grid and set values to 0

	m_normals = new CVector*[m_iComplexity + 1];

	for (i = 0; i <= m_iComplexity; i++)
	{
		m_normals[i] = new CVector[m_iComplexity + 1];
	}

	/* create a temporal grid to save the normal vectors of the terrain-grid
	   (used to avoid calculating values twice or three times)*/

	CVector v1,v2;
	float grid_spacing = m_fSize / (float)m_iComplexity;
	CVector** tmp_normals = new CVector*[m_iComplexity * 2];

	for (i = 0; i < m_iComplexity; i++)
	{
		tmp_normals[i * 2] = new CVector[m_iComplexity];
		tmp_normals[i * 2 + 1] = new CVector[m_iComplexity];

		for (j = 0; j < m_iComplexity; j++)
		{
			v1.x = 0.0;
			v1.y = -1.0f * grid_spacing;
			v1.z = m_grid[i][j + 1] - m_grid[i][j];
			v2.x = grid_spacing;
			v2.y = 0.0;
			v2.z = m_grid[i + 1][j] - m_grid[i][j];
			tmp_normals[i * 2][j] = v1.CrossProduct(v2);
			tmp_normals[i * 2][j].Normalize();

			v1.x = 0.0;
			v1.y = grid_spacing;
			v1.z = m_grid[i + 1][j] - m_grid[i + 1][j + 1];
			v2.x = -1.0f * grid_spacing;
			v2.y = 0.0;
			v2.z = m_grid[i][j + 1] - m_grid[i + 1][j + 1];
			tmp_normals[i * 2 + 1][j] = v1.CrossProduct(v2);
			tmp_normals[i * 2 + 1][j].Normalize();
		}
	}

	// calculate inner grid

	for (i = 1; i < m_iComplexity; i++)
	{
		for (j = 1; j < m_iComplexity; j++)
		{
			m_normals[i][j] = (tmp_normals[i * 2 - 1][j - 1] + tmp_normals[i * 2][j - 1] +
				tmp_normals[i * 2 + 1][j - 1] + tmp_normals[i * 2 - 2][j] +
				tmp_normals[i * 2 - 1][j] + tmp_normals[i * 2][j]) / 6.0f;
		}
	}

	// calculate all 4 edges of the grid

	// upper edge

	for (i = 1;  i < m_iComplexity; i++)
	{
		m_normals[i][0] = (tmp_normals[i * 2 - 2][0] + tmp_normals[i * 2 - 1][0] +
			tmp_normals[i * 2][0]) / 3.0f;
	}

	// lower edge

	for (i = 1;  i < m_iComplexity; i++)
	{
		m_normals[i][m_iComplexity] = (tmp_normals[i * 2 - 1][m_iComplexity - 1] + tmp_normals[i * 2][m_iComplexity - 1] +
			tmp_normals[i * 2 + 1][m_iComplexity - 1]) / 3.0f;
	}

	// left edge

	for (j = 1;  j < m_iComplexity; j++)
	{
		m_normals[0][j] = (tmp_normals[0][j - 1] + tmp_normals[1][j - 1] +
			tmp_normals[0][j]) / 3.0f;
	}

	// right edge

	for (j = 1;  j < m_iComplexity; j++)
	{
		m_normals[m_iComplexity][j] = (tmp_normals[m_iComplexity * 2 - 1][j - 1] + tmp_normals[m_iComplexity * 2 - 2][j] +
			tmp_normals[m_iComplexity * 2 - 1][j]) / 3.0f;
	}

	// calculate the 4 corners

	// upper left

	m_normals[0][0] = tmp_normals[0][0];

	// lower left

	m_normals[0][m_iComplexity] = (tmp_normals[0][m_iComplexity - 1] + tmp_normals[1][m_iComplexity - 1]) / 2.0f;

	// upper right

	m_normals[m_iComplexity][0] = (tmp_normals[m_iComplexity * 2 - 2][0] + tmp_normals[m_iComplexity * 2 - 1][0]) / 2.0f;

	// lower right

	m_normals[m_iComplexity][m_iComplexity] = tmp_normals[m_iComplexity * 2 - 1][m_iComplexity - 1];

	// delete temporal normal-grid

	for (i = 0; i < m_iComplexity * 2; i++)
	{
		delete tmp_normals[i];
	}

	delete tmp_normals;
}

float** CDiamondSquare::GetGrid ()
{
	return m_grid;
}

CVector** CDiamondSquare::GetNormals ()
{
	return m_normals;
}

int CDiamondSquare::GetIteration ()
{
	return m_iIteration;
}

int CDiamondSquare::GetComplexity ()
{
	return m_iComplexity;
}

float CDiamondSquare::GetSize ()
{
	return m_fSize;
}

float CDiamondSquare::GetRoughness ()
{
	return m_fRoughness;
}

float CDiamondSquare::GetInit ()
{
	return m_fInit;
}