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

#include "ConfigFile.h"

CConfigFile::CConfigFile()
{
	for (int i = 0; i < 256; i++)
	{
		m_caFilename[i] = 0;
		m_caToken[i] = 0;
		m_caValue[i] = 0;
	}
}

CConfigFile::CConfigFile(const char* filename)
{
	for (int i = 0; i < 256; i++)
	{
		m_caFilename[i] = 0;
		m_caToken[i] = 0;
		m_caValue[i] = 0;
	}

	SetFilename(filename);
}

void CConfigFile::SetFilename(const char* filename)
{
	strncpy(m_caFilename, filename, strlen(filename) + 1);
}

int CConfigFile::OpenConfig()
{
	m_pFile = fopen(m_caFilename, "rb");

	if (m_pFile == NULL)
	{
		return 0;
	}

	return 1;
}

void CConfigFile::CloseConfig()
{
	fclose(m_pFile);
}

int CConfigFile::ParseLine()
{
	int count = 0;
	int state = 0;		// 0: start, 1: read token, 2: read value, 3: read comment,
						// 4: read rest of line, 5: end
	char readChar = 0;
	bool read = false;
	int retValue = 0;

	while(state != 5)
	{
		fread(&readChar, 1, 1, m_pFile);

		if (feof(m_pFile) != 0)
		{
			// end of file reached

			readChar = 0;
		}

		switch(readChar)
		{
		case 0:
			if ((state != 2) && (state != 4))
			{
				// no correct state, reset strings

				m_caToken[0] = 0;
				m_caValue[0] = 0;
			}
			else if (state == 2)
			{
				if (count > 0)
				{
					// don't forget terminating string for state 2

					m_caValue[count] = 0;
				}
				else
				{
					// value holds no content, reset strings

					m_caToken[0] = 0;
					m_caValue[0] = 0;
				}
			}

			retValue = 0;
			state = 5;
			break;

		case '\n':
			if ((state != 2) && (state != 4))
			{
				// no correct state, reset strings

				m_caToken[0] = 0;
				m_caValue[0] = 0;
			}
			else if (state == 2)
			{
				if (count > 0)
				{
					// don't forget terminating string for state 2

					m_caValue[count] = 0;
				}
				else
				{
					// value holds no content, reset strings

					m_caToken[0] = 0;
					m_caValue[0] = 0;
				}
			}

			retValue = 1;
			state = 5;
			break;

		case '\r':
			// ignore '\r'

			read = false;
			break;

		case '\t':
			if ((state == 1) && (read == true))
			{
				// if state is 1, switch to state 2

				m_caToken[count] = 0;
				state = 2;
				count = 0;
			}
			else if ((state == 2) && (read == true))
			{
				// if state is 2, switch to state 4

				m_caValue[count] = 0;
				state = 4;
				count = 0;
			}

			read = false;
			break;

		case ' ':
			if ((state == 1) && (read == true))
			{
				// if state is 1, switch to state 2

				m_caToken[count] = 0;
				state = 2;
				count = 0;
			}
			else if ((state == 2) && (read == true))
			{
				// if state is 2, switch to state 4

				m_caValue[count] = 0;
				state = 4;
				count = 0;
			}

			read = false;
			break;

		case '#':
			// only set state 3 if state 4 is not reached

			if (state != 4)
			{
				state = 3;
			}

			read = false;
			break;

		default:
			if (state == 0)
			{
				// set state 1

				state = 1;
			}

			read = true;
		}

		switch(state)
		{
		case 1:
			if (read == true)
			{
				// read char

				m_caToken[count] = readChar;
				count++;
			}

			break;

		case 2:
			if (read == true)
			{
				// read char

				m_caValue[count] = readChar;
				count++;
			}

			break;
		}
	}

	return retValue;
}

char* CConfigFile::GetToken()
{
	return m_caToken;
}

char* CConfigFile::GetValue()
{
	return m_caValue;
}

char* CConfigFile::GetFilename()
{
	return m_caFilename;
}
