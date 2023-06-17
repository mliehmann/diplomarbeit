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

#include "ArPEngine.h"

CArPEngine::CArPEngine()
{
	srand( (unsigned)time( NULL ) );

	m_levellist = NULL;
	m_iLevels = 0;
	m_iCurrentLevel = 0;
	m_bMenuActive = true;
	m_bLevelContinue = false;

	m_pCamera = new CCamera;
	m_pWorld = NULL;
	m_pPlayer = NULL;
}

CArPEngine::CArPEngine(const char *strFileName) : CEngine(strFileName)
{
	srand( (unsigned)time( NULL ) );

	m_levellist = NULL;
	m_iLevels = 0;
	m_iCurrentLevel = 0;
	m_bMenuActive = true;
	m_bLevelContinue = false;

	ReadBindings();
	ReadLevelList();

	m_pCamera = new CCamera;
	m_pWorld = new CWorld(m_pConfigFile, NULL, m_pCamera, m_iWidth, m_iHeight); // start menu-world
	m_pPlayer = NULL;	// menu-world has no player
}

CArPEngine::~CArPEngine()
{
	delete m_pWorld;
	delete m_pCamera;
	m_pPlayer = NULL;

	for (int i = 0; i < m_iLevels; i++)
	{
		delete m_levellist[i];
	}

	delete m_levellist;
}

void CArPEngine::OnSetup()
{
	// delete old values

	delete m_pWorld;
	delete m_pCamera;

	for (int i = 0; i < m_iLevels; i++)
	{
		delete m_levellist[i];
	}

	delete m_levellist;

	// default values

	m_levellist = NULL;
	m_iLevels = 0;
	m_iCurrentLevel = 0;
	m_bMenuActive = true;
	m_bLevelContinue = false;

	// read key bindings and level list

	ReadBindings();
	ReadLevelList();

	m_pCamera = new CCamera;
	m_pWorld = new CWorld(m_pConfigFile, NULL, m_pCamera, m_iWidth, m_iHeight); // start menu-world
	m_pPlayer = NULL;	// menu-world has no player
}

void CArPEngine::OnPrepare()
{
	if (glGetError() != GL_NO_ERROR)
	{
		char sz[256];
		sprintf (sz, "OpenGL error occurred\n");
		throw sz;
	}

	// setup OpenGL for frame

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (m_pPlayer != NULL)
	{
		if (m_pWorld->IsWorldDone() == false)
		{
			// game is running, get player input

			if (m_baKeys[0] == true)
			{
				m_pPlayer->RotateZ(-1.0f);
			}

			if (m_baKeys[1] == true)
			{
				m_pPlayer->RotateZ(1.0f);
			}

			if (m_baKeys[2] == true)
			{
				m_pPlayer->AccelerateOn();
			}

			if (m_baKeys[2] == false)
			{
				m_pPlayer->AccelerateOff();
			}

			if (m_baKeys[3] == true)
			{
				m_pPlayer->BrakeOn();
			}

			if (m_baKeys[3] == false)
			{
				m_pPlayer->BrakeOff();
			}

			if (m_baKeys[4] == true)
			{
				m_pPlayer->FireWeaponOn();
			}

			if (m_baKeys[4] == false)
			{
				m_pPlayer->FireWeaponOff();
			}
		}
		else if (m_bLevelContinue == true)
		{
			// level finished, continue

			if (m_iCurrentLevel == m_iLevels)
			{
				// all levels complete, loading main menu

				delete m_pWorld;
				m_bMenuActive = true;
				m_bLevelContinue = false;
				m_iCurrentLevel = 0;

				m_pWorld = new CWorld(m_pConfigFile, NULL, m_pCamera, m_iWidth, m_iHeight); // start menu-world
				m_pPlayer = NULL;	// menu-world has no player
			}
			else if (m_pWorld->IsGameWon())
			{
				// level complete, loading next world

				delete m_pWorld;
				m_pWorld = new CWorld(m_pConfigFile, m_levellist[m_iCurrentLevel], m_pCamera, m_iWidth, m_iHeight);
				m_pPlayer = m_pWorld->m_pPlayer;
				m_iCurrentLevel++;
				m_bLevelContinue = false;
			}
			else
			{
				// level lost, loading main menu

				delete m_pWorld;
				m_bMenuActive = true;
				m_bLevelContinue = false;
				m_iCurrentLevel = 0;

				m_pWorld = new CWorld(m_pConfigFile, NULL, m_pCamera, m_iWidth, m_iHeight); // start menu-world
				m_pPlayer = NULL;	// menu-world has no player
			}
		}
	}
}

void CArPEngine::OnMouseDownL(float x, float y)
{
	if ((m_pPlayer != NULL) && (m_pWorld->IsWorldDone() == false))
	{
		m_baKeys[m_iBindLeft] = true;
	}
}

void CArPEngine::OnMouseUpL(float x, float y)
{
	if ((m_pPlayer != NULL) && (m_pWorld->IsWorldDone() == false))
	{
		m_baKeys[m_iBindLeft] = false;
	}
}

void CArPEngine::OnMouseDownR(float x, float y)
{
	if ((m_pPlayer != NULL) && (m_pWorld->IsWorldDone() == false))
	{
		m_baKeys[m_iBindRight] = true;
	}
}

void CArPEngine::OnMouseUpR(float x, float y)
{
	if ((m_pPlayer != NULL) && (m_pWorld->IsWorldDone() == false))
	{
		m_baKeys[m_iBindRight] = false;
	}
}

void CArPEngine::OnMouseDownM(float x, float y)
{
	if ((m_pPlayer != NULL) && (m_pWorld->IsWorldDone() == false))
	{
		m_baKeys[m_iBindMiddle] = true;
	}
}

void CArPEngine::OnMouseUpM(float x, float y)
{
	if ((m_pPlayer != NULL) && (m_pWorld->IsWorldDone() == false))
	{
		m_baKeys[m_iBindMiddle] = false;
	}
}

void CArPEngine::OnMouseMove(int deltaX, int deltaY)
{
	if ((m_pPlayer != NULL) && (m_pWorld->IsWorldDone() == false))
	{
		m_pPlayer->RotateY((float)deltaX * m_fMouseSensitivity);
		m_pPlayer->RotateX((float)deltaY * m_fMouseSensitivity);
	}
}

void CArPEngine::OnKeyDown(SDLKey key)
{
	
	if ((m_pPlayer != NULL) && (m_pWorld->IsWorldDone() == false))
	{
		if (key == roll_left)
		{
			m_baKeys[0] = true;
		}
		else if (key == roll_right)
		{
			m_baKeys[1] = true;
		}
		else if (key == thrust)
		{
			m_baKeys[2] = true;
		}
		else if (key == brake)
		{
			m_baKeys[3] = true;
		}
		else if (key == shoot)
		{
			m_baKeys[4] = true;
		}
		else if (key == increase_mousesens)
		{
			m_fMouseSensitivity += 0.05f;
		}
		else if (key == decrease_mousesens)
		{
			m_fMouseSensitivity -= 0.05f;
			if (m_fMouseSensitivity < 0.05f)
				m_fMouseSensitivity = 0.05f;
		}
		else if (key == SDLK_F1)
		{
			m_pWorld->ShowMenu();
		}
		else if (key == SDLK_F12)
		{
			m_pWorld->QuitWorld();
		}
	}

	if (key == music)
	{
		m_pWorld->PlayMusic();
	}

	if (key == SDLK_ESCAPE)
	{
		// post quit message

		SDL_Event event;
		event.type = SDL_QUIT;
		SDL_PushEvent(&event);
	}

	if ((key == SDLK_SYSREQ) || (key == SDLK_PRINT))
	{
		SaveScreenshot();
	}

	if (key == SDLK_F10)
	{
		if (m_bMenuActive == true)
		{
			// leave main menu and start game

			m_bMenuActive = false;
			delete m_pWorld;
			m_pWorld = new CWorld(m_pConfigFile, m_levellist[m_iCurrentLevel], m_pCamera, m_iWidth, m_iHeight);
			m_pPlayer = m_pWorld->m_pPlayer;
			m_iCurrentLevel++;
		}
		else if (m_pWorld->IsWorldDone() == true)
		{
			m_bLevelContinue = true;
		}
	}
}

void CArPEngine::OnKeyUp(SDLKey key)
{
	
	if ((m_pPlayer != NULL) && (m_pWorld->IsWorldDone() == false))
	{
		if (key == roll_left)
		{
			m_baKeys[0] = false;
		}
		else if (key == roll_right)
		{
			m_baKeys[1] = false;
		}
		else if (key == thrust)
		{
			m_baKeys[2] = false;
		}
		else if (key == brake)
		{
			m_baKeys[3] = false;
		}
		else if (key == shoot)
		{
			m_baKeys[4] = false;
		}
	}
}

void CArPEngine::OnFocus(int gain)
{
	m_pWorld->Focus(gain);
}

void CArPEngine::ReadBindings()
{
	// initialize bindings

	shoot = (SDLKey)0;
	thrust = (SDLKey)0;
	brake = (SDLKey)0;
	roll_left = (SDLKey)0;
	roll_right = (SDLKey)0;
	increase_mousesens = (SDLKey)0;
	decrease_mousesens = (SDLKey)0;
	music = (SDLKey)0;

	m_iBindLeft = 4;		// bind left mouse button to shoot (default)
	m_iBindRight = 2;		// bind right mouse button to thrust (default)
	m_iBindMiddle = 3;		// bind middle mouse button to brake (default)

	// read key bindings from config file

	int result = m_pConfigFile->OpenConfig();

	if (result == 0)
	{
		char sz[256];
		sprintf (sz, "Can't load config: %s\n", m_pConfigFile->GetFilename());
		throw sz;
	}

	do
	{
		result = m_pConfigFile->ParseLine();

		if (m_pConfigFile->GetToken()[0] == 0)
		{
			// no token

			continue;
		}

		if (strcmp(m_pConfigFile->GetToken(), "bind_shoot") == 0)
		{
			shoot = (SDLKey)atoi(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "bind_thrust") == 0)
		{
			thrust = (SDLKey)atoi(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "bind_brake") == 0)
		{
			brake = (SDLKey)atoi(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "bind_roll_left") == 0)
		{
			roll_left = (SDLKey)atoi(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "bind_roll_right") == 0)
		{
			roll_right = (SDLKey)atoi(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "bind_increase_mousesens") == 0)
		{
			increase_mousesens = (SDLKey)atoi(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "bind_decrease_mousesens") == 0)
		{
			decrease_mousesens = (SDLKey)atoi(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "bind_music") == 0)
		{
			music = (SDLKey)atoi(m_pConfigFile->GetValue());
		}
		else if (strcmp(m_pConfigFile->GetToken(), "mouse_button_left") == 0)
		{
			if (strcmp(m_pConfigFile->GetValue(), "shoot") == 0)
			{
				m_iBindLeft = 4;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "thrust") == 0)
			{
				m_iBindLeft = 2;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "brake") == 0)
			{
				m_iBindLeft = 3;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "roll_left") == 0)
			{
				m_iBindLeft = 0;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "roll_right") == 0)
			{
				m_iBindLeft = 1;
			}
		}
		else if (strcmp(m_pConfigFile->GetToken(), "mouse_button_right") == 0)
		{
			if (strcmp(m_pConfigFile->GetValue(), "shoot") == 0)
			{
				m_iBindRight = 4;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "thrust") == 0)
			{
				m_iBindRight = 2;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "brake") == 0)
			{
				m_iBindRight = 3;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "roll_left") == 0)
			{
				m_iBindRight = 0;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "roll_right") == 0)
			{
				m_iBindRight = 1;
			}
		}
		else if (strcmp(m_pConfigFile->GetToken(), "mouse_button_middle") == 0)
		{
			if (strcmp(m_pConfigFile->GetValue(), "shoot") == 0)
			{
				m_iBindMiddle = 4;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "thrust") == 0)
			{
				m_iBindMiddle = 2;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "brake") == 0)
			{
				m_iBindMiddle = 3;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "roll_left") == 0)
			{
				m_iBindMiddle = 0;
			}
			else if (strcmp(m_pConfigFile->GetValue(), "roll_right") == 0)
			{
				m_iBindMiddle = 1;
			}
		}
	}while(result == 1);

	m_pConfigFile->CloseConfig();
}

void CArPEngine::ReadLevelList()
{
	// read level list

	int i;
	int result;
	int count = 0;
	char levellistname[256];

	for (i = 0; i < 256; i++)
	{
		levellistname[i] = 0;
	}

	result = m_pConfigFile->OpenConfig();

	if (result == 0)
	{
		char sz[256];
		sprintf (sz, "Can't load config: %s\n", m_pConfigFile->GetFilename());
		throw sz;
	}

	// search for name of the level_list config file

	do
	{
		result = m_pConfigFile->ParseLine();

		if (m_pConfigFile->GetToken()[0] == 0)
		{
			// no token

			continue;
		}

		if (strcmp(m_pConfigFile->GetToken(), "level_list") == 0)
		{
			strncpy(levellistname, m_pConfigFile->GetValue(), strlen(m_pConfigFile->GetValue()) + 1);
		}

	}while(result == 1);

	m_pConfigFile->CloseConfig();

	if (levellistname[0] == 0)
	{
		char sz[256];
		sprintf (sz, "Can't find filename of level-list; check config-file");
		throw sz;
	}

	CConfigFile* cf = new CConfigFile(levellistname);

	if (cf->OpenConfig() == 0)
	{
		char sz[256];
		sprintf (sz, "Can't load level-list");
		throw sz;
	}

	// search for number of levels

	m_iLevels = 0;

	do
	{
		result = cf->ParseLine();

		if (cf->GetToken()[0] == 0)
		{
			// no token

			continue;
		}

		if (strcmp(cf->GetToken(), "level") == 0)
		{
			m_iLevels++;
		}

	}while(result == 1);

	if (m_iLevels == 0)
	{
		char sz[256];
		sprintf (sz, "Level-list contains no levels\n");
		throw sz;
	}

	// reset file pointer

	cf->CloseConfig();
	cf->OpenConfig();

	m_levellist = new char*[m_iLevels];

	for (i = 0; i < m_iLevels; i++)
	{
		m_levellist[i] = new char[256];
	}

	do
	{
		result = cf->ParseLine();

		if (cf->GetToken()[0] == 0)
		{
			// no token

			continue;
		}

		if (strcmp(cf->GetToken(), "level") == 0)
		{
			strncpy(m_levellist[count], cf->GetValue(), strlen(cf->GetValue()) + 1);
			count++;
		}
	}while (result == 1);

	if (count < m_iLevels)
	{
		char sz[256];
		sprintf (sz, "Number of levels doesn't match actual levels, check level-list\n");
		throw sz;
	}

	cf->CloseConfig();

	delete cf;
}

CCamera* CArPEngine::OnGetCamera()
{
	return m_pCamera;
}

CWorld* CArPEngine::OnGetWorld()
{
	return m_pWorld;
}

void CArPEngine::SaveScreenshot()
{
	unsigned char* imageData;
	imageData = (unsigned char*)malloc(m_iWidth * m_iHeight * 3);	// allocate memory for image data
	memset(imageData, 0, m_iWidth * m_iHeight * 3);					// clear imageData memory contents

	// read the image data from the window

	glReadPixels(0, 0, m_iWidth - 1, m_iHeight - 1, GL_RGB, GL_UNSIGNED_BYTE, imageData);

	unsigned char byteSkip;			// used for byte garbage data
	short int shortSkip;			// used for short int garbage data
	unsigned char imageType;		// type of image we're writing to file
	int colorMode;					// color mode of image
	unsigned char colorSwap;		// used for RGB-to-BGR conversion
	int imageIdx;					// counter for RGB-to-BGR conversion
	unsigned char bitDepth;			// bit depth of the image
	long imageSize;					// size of the image data
	FILE* filePtr;					// pointer to file
	int count = 0;
	char filename[256];
	sprintf(filename, "ArP%.4d.tga", count);

	// seek next free filename

	for(;;)
	{
		filePtr = fopen(filename, "r");

		if (filePtr == NULL)
		{
			break;
		}
		else
		{
			fclose(filePtr);
			count++;
			sprintf(filename, "ArP%.4d.tga", count);
		}
	}

	// create file for writing binary mode

	filePtr = fopen(filename, "wb");

	if (filePtr == 0)
	{
		fclose(filePtr);
		return;
	}

	imageType = 2;		// RGB, uncompressed
	bitDepth = 24;		// 24-bitdepth
	colorMode = 3;		// RGB color mode

	byteSkip = 0;		// garbage data for byte data
	shortSkip = 0;		// garbage data for short int data

	// write blank data

	fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);
	fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);

	// write imageType

	fwrite(&imageType, sizeof(unsigned char), 1, filePtr);

	// write blank data

	fwrite(&shortSkip, sizeof(short int), 1, filePtr);
	fwrite(&shortSkip, sizeof(short int), 1, filePtr);
	fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);
	fwrite(&shortSkip, sizeof(short int), 1, filePtr);
	fwrite(&shortSkip, sizeof(short int), 1, filePtr);

	// write image dimensions

	fwrite(&m_iWidth, sizeof(short int), 1, filePtr);
	fwrite(&m_iHeight, sizeof(short int), 1, filePtr);
	fwrite(&bitDepth, sizeof(unsigned char), 1, filePtr);

	// write 1 byte of blank data

	fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);

	// calculate the image size

	imageSize = m_iWidth * m_iHeight * colorMode;

	// change image data from RGB to BGR

	for(imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
	{
		colorSwap = imageData[imageIdx];
		imageData[imageIdx] = imageData[imageIdx + 2];
		imageData[imageIdx + 2] = colorSwap;
	}

	// write image data

	fwrite(imageData, sizeof(unsigned char), imageSize, filePtr);

	// close file

	fclose(filePtr);

	// free image data memory

	free(imageData);
}