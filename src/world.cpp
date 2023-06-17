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

#include "world.h"

CWorld::CWorld()
{
}

CWorld::CWorld(CConfigFile* cf, const char* strFileName, CCamera* c, int w, int h)
{
	m_pConfigFile = cf;

	SetDefaultValues();		// setting world data members to default values
	ReadMainConfig();		// reading data from main config file

	m_iScreenWidth = w;
	m_iScreenHeight = h;
	m_bGameWon = false;
	m_bPlayMusic = true;
	m_bWorldDone = false;
	m_fLevelEndDelay = 0.0f;
	m_iArenaSize = 64;
	m_iFPS = -1;			// don't show FPS (activated through engine)
	m_pCamera = c;

	if (strFileName != NULL)
	{
		// world is real level, loading level config

		m_bIsMenu = false;
		m_bShowMenu = true;
		ReadLevelConfig(strFileName);
	}
	else
	{
		// world is just main menu, read menu config

		m_bShowMenu = false;
		m_bIsMenu = true;
		ReadLevelConfig("menu.cfg");
	}

	m_ODEWorld = dWorldCreate();					// create new ODE world
	m_ODESpace = dHashSpaceCreate (0);				// create new ODE space (multi-resolution hash table)
	m_ODEContactGroup = dJointGroupCreate (0);		// contact group for collision detection
	int tmp = (int)(log(m_iArenaSize) / log(2));	// calculating values for hash table
	dHashSpaceSetLevels (m_ODESpace, 0, tmp);		// set hash table levels, smallest cell is 2^0 = 1

	/* the order of objects attached to the arena ist very important for blending
	   (first draw all solid objects, then all transparent objects!);
	   plasma shots (attached to player) and particles have to be at the end of the list*/

	m_pArena = new CArena(this);
	m_pPlanetoids = new CObject();
	m_pPlanetoids->AttachTo(m_pArena);

	if (m_bIsMenu == true)
	{
		// menu-world for main screen does not need any player, attach camera to arena

		m_pPlayer = NULL;
		m_pCamera->SetObject(m_pArena);
	}
	else
	{
		// add player to arena and attach camera to player

		m_pPlayer = new CPlayer(this);
		m_pPlayer->AttachTo(m_pArena);
		m_pCamera->SetObject(m_pPlayer);
	}

	m_pGUI = new CGUI(this);
	m_pParticles = new CObject();
	m_pParticles->AttachTo(m_pArena);

	// set values for parent objects of planetoids and particles

	m_pPlanetoids->m_vecPosition = CVector(0.0f, 0.0f, 0.0f);
	m_pParticles->m_vecPosition = CVector(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 16; i++)
	{
		if (i%5 == 0)
		{
			m_pPlanetoids->m_faRotMatrix[i] = 1.0f;
			m_pParticles->m_faRotMatrix[i] = 1.0f;
		}
		else
		{
			m_pPlanetoids->m_faRotMatrix[i] = 0.0f;
			m_pParticles->m_faRotMatrix[i] = 0.0f;
		}
	}

	// parent objects have size 0 and IDs -1

	m_pPlanetoids->m_fSize = 0.0f;
	m_pParticles->m_fSize = 0.0f;
	m_pPlanetoids->m_iIdent = -1;
	m_pParticles->m_iIdent = -1;

	LoadWorld();

	char str[256];

	// set volume of music and sound channels

	int volume = (int)((float)MIX_MAX_VOLUME * m_fMusicVolume * m_fMusicMaxVolume);
	Mix_VolumeMusic(volume);
	sprintf(str, "Music volume set to %d.\n", volume);
	WriteLog(str);

	volume = (int)((float)MIX_MAX_VOLUME * m_fSoundVolume * m_fSoundMaxVolume);
	Mix_Volume(-1, volume);
	sprintf(str, "Sound volume set to %d.\n", volume);
	WriteLog(str);

	// play music, infinite loop

	Mix_PlayMusic(m_pMusic, -1);

	WriteLog("\n");

	// calculate total number of planetoids

	m_iCurrentPlanetoids = m_iPlanetoid1Quantity + m_iPlanetoid2Quantity + m_iPlanetoid3Quantity;

	/* initiate first ODE-Worldstep
	   (1 ms, important for torque- and force-values in config)*/

	dWorldStep(m_ODEWorld, 0.001f);
}

CWorld::~CWorld()
{
	delete m_pArena;			// deletes the whole tree (player, planetoids etc.)
	delete m_pGUI;
	dJointGroupDestroy (m_ODEContactGroup);
	dWorldDestroy (m_ODEWorld);
	dSpaceDestroy (m_ODESpace);
	dCloseODE();

	char str[256];
	sprintf(str, "Closing level...\n");
	WriteLog(str);

	// delete used textures

	if (m_texFont.texID != 0)
	{
		glDeleteTextures(1, &m_texFont.texID);
		sprintf(str, "Texture ID %d deleted.\n", m_texFont.texID);
		WriteLog(str);
	}

	if (m_texPlanetoid.texID != 0)
	{
		glDeleteTextures(1, &m_texPlanetoid.texID);
		sprintf(str, "Texture ID %d deleted.\n", m_texPlanetoid.texID);
		WriteLog(str);
	}

	if (m_texParticle.texID != 0)
	{
		glDeleteTextures(1, &m_texParticle.texID);
		sprintf(str, "Texture ID %d deleted.\n", m_texParticle.texID);
		WriteLog(str);
	}

	if (m_texPlasma.texID != 0)
	{
		glDeleteTextures(1, &m_texPlasma.texID);
		sprintf(str, "Texture ID %d deleted.\n", m_texPlasma.texID);
		WriteLog(str);
	}

	if (m_texArena.texID != 0)
	{
		glDeleteTextures(1, &m_texArena.texID);
		sprintf(str, "Texture ID %d deleted.\n", m_texArena.texID);
		WriteLog(str);
	}

	for (int i = 0; i < 6; i++)
	{
		if (m_texSkybox[i].texID != 0)
		{
			glDeleteTextures(1, &m_texSkybox[i].texID);
			sprintf(str, "Texture ID %d deleted.\n", m_texSkybox[i].texID);
			WriteLog(str);
		}
	}

	// halt all channels including music

	Mix_HaltChannel(-1);
	sprintf(str, "All channels halted.\n");
	WriteLog(str);

	Mix_HaltMusic();
	sprintf(str, "Music halted.\n");
	WriteLog(str);

	// free music and all chunks

	if (m_pMusic != NULL)
	{
		Mix_FreeMusic(m_pMusic);
		sprintf(str, "Music freed.\n");
		WriteLog(str);
	}

	if (m_pSampleHit != NULL)
	{
		Mix_FreeChunk(m_pSampleHit);
		sprintf(str, "Sound_hit freed.\n");
		WriteLog(str);
	}

	if (m_pSamplePlasma != NULL)
	{
		Mix_FreeChunk(m_pSamplePlasma);
		sprintf(str, "Sound_plasma freed.\n");
		WriteLog(str);
	}

	if (m_pSampleThrust != NULL)
	{
		Mix_FreeChunk(m_pSampleThrust);
		sprintf(str, "Sound_thrust freed.\n");
		WriteLog(str);
	}

	if (m_pSamplePlasmaHit != NULL)
	{
		Mix_FreeChunk(m_pSamplePlasmaHit);
		sprintf(str, "Sound_plasma_hit freed.\n");
		WriteLog(str);
	}

	WriteLog("\n\n");
}

void CWorld::LoadWorld()
{
	int result = 0;
	int i;

	// counter for quantity of planetoids, 700 is maximum; small planetoids count 1, medium 2 and big 6 units

	int count = 0;

	CVector pos;
	CObject* obj = NULL;
	CEnemyPlanetoid* pl;

	/* create planetoids of each class, get random positions for them, check those positions, and if
	   valid, attach them to the arena*/

	for (i = 0; i < m_iPlanetoid1Quantity; i++)
	{
		count++;

		if (count > 700)
		{
			break;
		}

		do
		{
			pos.x = (float)rand() * (float)m_iArenaSize * 0.75f / (float)RAND_MAX - (float)m_iArenaSize * 0.375f;
			pos.y = (float)rand() * (float)m_iArenaSize * 0.75f / (float)RAND_MAX - (float)m_iArenaSize * 0.375f;
			pos.z = (float)rand() * (float)m_iArenaSize * 0.75f / (float)RAND_MAX - (float)m_iArenaSize * 0.375f;

			result = CheckPosition(pos, 3.0f);
		}while (result != 1);

		pl = new CEnemyPlanetoid(this, &pos, 1, 0.001f);		// 0.001 for first worldstep (1 ms)
		pl->AttachTo(m_pPlanetoids);
	}

	for (i = 0; i < m_iPlanetoid2Quantity; i++)
	{
		count += 2;

		if (count > 700)
		{
			break;
		}

		do
		{
			pos.x = (float)rand() * (float)m_iArenaSize * 0.75f / (float)RAND_MAX - (float)m_iArenaSize * 0.375f;
			pos.y = (float)rand() * (float)m_iArenaSize * 0.75f / (float)RAND_MAX - (float)m_iArenaSize * 0.375f;
			pos.z = (float)rand() * (float)m_iArenaSize * 0.75f / (float)RAND_MAX - (float)m_iArenaSize * 0.375f;

			result = CheckPosition(pos, 4.0f);
		}while (result != 1);

		pl = new CEnemyPlanetoid(this, &pos, 2, 0.001f);		// 0.001 for first worldstep (1 ms)
		pl->AttachTo(m_pPlanetoids);
	}

	for (i = 0; i < m_iPlanetoid3Quantity; i++)
	{
		count += 6;

		if (count > 700)
		{
			break;
		}

		do
		{
			pos.x = (float)rand() * (float)m_iArenaSize * 0.75f / (float)RAND_MAX - (float)m_iArenaSize * 0.375f;
			pos.y = (float)rand() * (float)m_iArenaSize * 0.75f / (float)RAND_MAX - (float)m_iArenaSize * 0.375f;
			pos.z = (float)rand() * (float)m_iArenaSize * 0.75f / (float)RAND_MAX - (float)m_iArenaSize * 0.375f;

			result = CheckPosition(pos, 6.0f);
		}while (result != 1);

		pl = new CEnemyPlanetoid(this, &pos, 3, 0.001f);		// 0.001 for first worldstep (1 ms)
		pl->AttachTo(m_pPlanetoids);
	}
}

int CWorld::CheckPosition(CVector pos, float size)
{
	CEnemyPlanetoid* current = (CEnemyPlanetoid*)m_pPlanetoids->childNode;

	if (current == NULL)
	{
		// no child

		return 1;
	}

	if (pos.Length() <= size + 16.0f)
	{
		// no planetoid near the player (player starts in the center of the arena)

		return 0;
	}

	if (current->nextNode == current)
	{
		// only one child-node

		if ((current->m_vecPosition - pos).Length() <= (current->m_fSize + size))
		{
			return 0;
		}
	}

	do
	{
		if ((current->m_vecPosition - pos).Length() <= (current->m_fSize + size))
		{
			return 0;
		}

		current = (CEnemyPlanetoid*)current->nextNode;
	}while(!current->IsLastChild());

	// check last child

	if ((current->m_vecPosition - pos).Length() <= (current->m_fSize + size))
	{
		return 0;
	}

	return 1;
}

void CWorld::Prepare()
{
	OnPrepare();
}

void CWorld::Animate(float deltaTime)
{
	if (m_bShowMenu == false)
	{
		// only move objects if game is not paused (no ingame menu is shown)

		if (m_bWorldDone == false)
		{
			CEnemyPlanetoid* current = (CEnemyPlanetoid*)m_pPlanetoids->childNode;

			if (current == NULL)
			{
				return;
			}

			if (current->nextNode == current)
			{
				// only one child-node

				CheckDeadPlanetoids(current, deltaTime);
				m_pArena->Animate(deltaTime);
				dSpaceCollide (m_ODESpace,this,&nearCallback);	// perform collision detection
				dWorldStep(m_ODEWorld, deltaTime);
				dJointGroupEmpty (m_ODEContactGroup);
				return;
			}

			do
			{
				CheckDeadPlanetoids(current, deltaTime);
				current = (CEnemyPlanetoid*)current->nextNode;
			}while(!current->IsLastChild());

			CheckDeadPlanetoids(current, deltaTime);

			m_pArena->Animate(deltaTime);
			dSpaceCollide (m_ODESpace,this,&nearCallback);	// perform collision detection
			dWorldStep(m_ODEWorld, deltaTime);
			dJointGroupEmpty (m_ODEContactGroup);
		}
		else if (m_bIsMenu == false)
		{
			// world is done, world is no menu-world

			m_fLevelEndDelay += deltaTime;

			if (m_fLevelEndDelay < 1.0f)
			{
				// play last second of level

				m_pArena->Animate(deltaTime);
				dSpaceCollide (m_ODESpace,this,&nearCallback);	// perform collision detection
				dWorldStep(m_ODEWorld, deltaTime);
				dJointGroupEmpty (m_ODEContactGroup);
			}
		}
	}
}

void CWorld::CheckDeadPlanetoids(CEnemyPlanetoid* current, float deltaTime)
{
	// make sure to check only planetoids (ID 2)

	if ((current->m_iIdent == 2) && (current->m_iHealth <= 0))
	{
		m_iCurrentPlanetoids--;

		if (current->m_iClass == 2)
		{
			// planetoid class 2 (middle planetoid), destroy and create 2 class 1 planetoids

			m_iCurrentPlanetoids += 2;
			CVector pos;
			CEnemyPlanetoid* pl;

			pos.x = current->m_vecPosition.x + current->m_fSize * 0.5f;
			pos.y = current->m_vecPosition.y + current->m_fSize * 0.5f;
			pos.z = current->m_vecPosition.z + current->m_fSize * 0.5f;
			pl = new CEnemyPlanetoid(this, &pos, 1, deltaTime);
			pl->AttachTo(m_pPlanetoids);

			pos.x = current->m_vecPosition.x - current->m_fSize * 0.5f;
			pos.y = current->m_vecPosition.y - current->m_fSize * 0.5f;
			pos.z = current->m_vecPosition.z - current->m_fSize * 0.5f;
			pl = new CEnemyPlanetoid(this, &pos, 1, deltaTime);
			pl->AttachTo(m_pPlanetoids);
		}

		if (current->m_iClass == 3)
		{
			// planetoid class 3 (big planetoid), destroy and create 2 class 2 planetoids

			m_iCurrentPlanetoids += 2;
			CVector pos;
			CEnemyPlanetoid* pl;

			pos.x = current->m_vecPosition.x + current->m_fSize * 0.5f;
			pos.y = current->m_vecPosition.y + current->m_fSize * 0.5f;
			pos.z = current->m_vecPosition.z + current->m_fSize * 0.5f;
			pl = new CEnemyPlanetoid(this, &pos, 2, deltaTime);
			pl->AttachTo(m_pPlanetoids);

			pos.x = current->m_vecPosition.x - current->m_fSize * 0.5f;
			pos.y = current->m_vecPosition.y - current->m_fSize * 0.5f;
			pos.z = current->m_vecPosition.z - current->m_fSize * 0.5f;
			pl = new CEnemyPlanetoid(this, &pos, 2, deltaTime);
			pl->AttachTo(m_pPlanetoids);
		}
	}
}

void CWorld::nearCallback (void *data, dGeomID o1, dGeomID o2)
{
	const int N = 10;								// number of contact points
	CWorld* world = (CWorld*)data;					// get world pointer
	CObject* obj1 = (CObject*)dGeomGetData (o1);	// get pointer to first object from first geom
	CObject* obj2 = (CObject*)dGeomGetData (o2);	// get pointer to second object from second geom

	dContact contact[N];							// create contact points

	int n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));	// get contact points

	if (n > 0)
	{
		for (int i=0; i<n; i++)
		{
			contact[i].surface.mode = dContactBounce /*| dContactSlip1 | dContactSlip2 |
				dContactSoftERP | dContactSoftCFM | dContactApprox1*/;

			if (world->m_fCoulombFriction < 0.0f)
			{
				contact[i].surface.mu = dInfinity;
			}
			else
			{
				contact[i].surface.mu = world->m_fCoulombFriction;
			}

			//contact[i].surface.slip1 = slip1;
			//contact[i].surface.slip2 = slip2;
			contact[i].surface.bounce = world->m_fBounce;
			contact[i].surface.bounce_vel = 0.0f;
			//contact[i].surface.soft_erp = 0.5;
			//contact[i].surface.soft_cfm = soft;

			// create contact joint

			dJointID c = dJointCreateContact (world->m_ODEWorld,
				world->m_ODEContactGroup,
				&contact[i]);

			// attach bodies to joint

			dJointAttach (c, dGeomGetBody (o1), dGeomGetBody (o2));

			// call collision handler of the objects

			obj1->ProcessCollision(obj2);
			obj2->ProcessCollision(obj1);
		}
	}
}

void CWorld::Draw(CCamera* camera)
{
	m_pArena->Draw(camera);

	if (m_iFPS > 0)
	{
		// draw frames per second

		m_pGUI->DrawFPS(m_iFPS);
	}

	if (m_bIsMenu == true)
	{
		// draw main-menu

		m_pGUI->DrawMainMenu();
	}
	else if (m_bShowMenu == true)
	{
		// draw ingame-menu

		m_pGUI->DrawIngameMenu();
	}

	if (m_fLevelEndDelay >= 1.0f)
	{
		if (m_bGameWon == true)
		{
			// draw game won

			m_pGUI->DrawGameWon();
		}
		else
		{
			//draw game lost

			m_pGUI->DrawGameLost();
		}
	}
	else
	{
		// draw player HUD

		if (m_bIsMenu == false)
		{
			float displace_x = 0.0f;
			float displace_y = 0.0f;

			if (m_pPlayer->IsHit() == true)
			{
				// player hit, calculate random displacement for HUD

				displace_x = 10.0f * (float)rand() / (float)RAND_MAX;
				displace_y = 10.0f * (float)rand() / (float)RAND_MAX;
			}

			m_pGUI->DrawPlayerHUD(m_pPlayer->m_iHealth, m_pPlayer->m_fFireDelay, m_fPlasmaFrequency, displace_x, displace_y);

			if (m_bShowMenu == false)
			{
				m_pGUI->DrawCrosshair(displace_x, displace_y);
			}
		}
	}
}

void CWorld::OnPrepare()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pArena->Prepare();

	if (m_bIsMenu == false)
	{
		if (m_pPlayer->m_iHealth <= 0)
		{
			// player is dead, level lost

			m_bWorldDone = true;
			m_bGameWon = false;
		}
		else if (m_iCurrentPlanetoids == 0)
		{
			// all planetoids destroyed, level won

			m_bWorldDone = true;
			m_bGameWon = true;
		}
	}
}

bool CWorld::IsWorldDone()
{
	return ((m_bWorldDone == true) && (m_fLevelEndDelay >= 1.0f));
}

bool CWorld::IsGameWon()
{
	return m_bGameWon;
}

bool CWorld::IsMenuShown()
{
	return m_bShowMenu;
}

bool CWorld::IsMenu()
{
	return m_bIsMenu;
}

void CWorld::QuitWorld()
{
	m_bWorldDone = true;
}

void CWorld::ShowMenu()
{
	if (m_bIsMenu == false)
	{
		m_bShowMenu = m_bShowMenu == false ? true : false;
	}
}

void CWorld::Focus(int gain)
{
	if (m_bIsMenu == true)
	{
		// freeze scene if world is main menu and focus is lost; if gained, animate world again

		m_bShowMenu = gain == 0 ? true : false;
	}
	else
	{
		// pause game if focus is lost

		m_bShowMenu = true;
	}
}

void CWorld::PlayMusic()
{
	if (m_bPlayMusic == true)
	{
		Mix_PauseMusic();
		m_bPlayMusic = false;
	}
	else
	{
		Mix_ResumeMusic();
		m_bPlayMusic = true;
	}
}

void CWorld::SetDefaultValues()
{
	int i;

	m_pMusic = NULL;
	m_pSampleHit = NULL;
	m_pSamplePlasma = NULL;
	m_pSampleThrust = NULL;
	m_pSamplePlasmaHit = NULL;

	for (i = 0; i < 256; i++)
	{
		if (i == 0)
		{
			m_caCrosshair[i] = 'X';
		}
		else
		{
			m_caCrosshair[i] = 0;
		}
	}

	m_faFontColor[0] = 0.0f;
	m_faFontColor[1] = 0.0f;
	m_faFontColor[2] = 1.0f;
	m_faFontColor[3] = 0.8f;
	m_faPlasmaColor[0] = 0.0f;
	m_faPlasmaColor[1] = 1.0f;
	m_faPlasmaColor[2] = 0.0f;
	m_faPlasmaColor[3] = 0.5f;
	m_faArenaColor[0] = 0.0f;
	m_faArenaColor[1] = 1.0f;
	m_faArenaColor[2] = 0.0f;
	m_faArenaColor[3] = 0.5f;
	m_faPlanetoidColor[0] = 0.0f;
	m_faPlanetoidColor[1] = 0.3f;
	m_faPlanetoidColor[2] = 0.6f;
	m_faPlanetoidColor[3] = 0.5f;

	m_faLight0Color[0] = 1.0f;
	m_faLight0Color[1] = 1.0f;
	m_faLight0Color[2] = 1.0f;
	m_faLight1Color[0] = 1.0f;
	m_faLight1Color[1] = 1.0f;
	m_faLight1Color[2] = 1.0f;
	m_faLight2Color[0] = 1.0f;
	m_faLight2Color[1] = 1.0f;
	m_faLight2Color[2] = 1.0f;
	m_faLight0Position[0] = 1.0f;
	m_faLight0Position[1] = 1.0f;
	m_faLight0Position[2] = 1.0f;
	m_faLight1Position[0] = 1.0f;
	m_faLight1Position[1] = 1.0f;
	m_faLight1Position[2] = 1.0f;
	m_faLight2Position[0] = 1.0f;
	m_faLight2Position[1] = 1.0f;
	m_faLight2Position[2] = 1.0f;
	m_faLightAmbient[0] = 0.3f;
	m_faLightAmbient[1] = 0.3f;
	m_faLightAmbient[2] = 0.3f;

	m_fPlasmaForce = 1000.0f;
	m_fPlanetoid1MaxForce = 100.0f;
	m_fPlanetoid2MaxForce = 400.0f;
	m_fPlanetoid3MaxForce = 1600.0f;
	m_fPlasmaMass = 0.05f;
	m_fPlayerMass = 0.025f;
	m_fPlanetoid1Mass = 0.05f;
	m_fPlanetoid2Mass = 0.2f;
	m_fPlanetoid3Mass = 0.8f;
	m_fPlanetoid1MaxTorque = 12.5f;
	m_fPlanetoid2MaxTorque = 50.0f;
	m_fPlanetoid3MaxTorque = 800.0f;
	m_fPlanetoid1Roughness = 0.4f;
	m_fPlanetoid2Roughness = 0.4f;
	m_fPlanetoid3Roughness = 0.4f;
	m_fPlanetoid1MininumSize = 0.5f;
	m_fPlanetoid2MininumSize = 0.5f;
	m_fPlanetoid3MininumSize = 0.5f;
	m_fPlasmaFrequency = 1.0f;
	m_fPlasmaLifetime = 10.0f;
	m_fPlayerThrust = 0.5f;
	m_fPlanetoidExplAlpha = 1.0f;
	m_fBounce = 1.0f;
	m_fCoulombFriction = 0.0f;
	m_fMusicMaxVolume = 1.0f;
	m_fSoundMaxVolume = 1.0f;
	m_fMusicVolume = 1.0f;
	m_fSoundVolume = 1.0f;

	m_iPlasmaHealth = 50;
	m_iPlayerHealth = 200;
	m_iPlanetoid1Health = 50;
	m_iPlanetoid2Health = 50;
	m_iPlanetoid3Health = 50;
	m_iHealthLoss = 50;
	m_iPlanetoid1Complexity = 2;
	m_iPlanetoid2Complexity = 3;
	m_iPlanetoid3Complexity = 4;
	m_iPlanetoid1Quantity = 8;
	m_iPlanetoid2Quantity = 8;
	m_iPlanetoid3Quantity = 8;
	m_iPlanetoidTexTiling = 0;
	m_iaPlanetoidExplColors[0] = 0;
	m_iaPlanetoidExplColors[1] = 1;
	m_iaPlanetoidExplColors[2] = 2;
	m_bArenaTransparency = true;
	m_bLight0Enabled = true;
	m_bLight1Enabled = false;
	m_bLight2Enabled = false;
	m_bPlanetoidTransparency = false;
	m_bPlanetoidWireframe = false;
	m_bPlasmaArenaBounce = true;
}

void CWorld::ReadLevelConfig(const char* strFileName)
{
	// read level config file and fill world data members

	CConfigFile* cf = new CConfigFile(strFileName);
	int result = cf->OpenConfig();

	if (result == 0)
	{
		char sz[256];
		sprintf (sz, "Can't load level-config: %s\n", strFileName);
		throw sz;
	}

	char str[256];
	sprintf(str, "Loading level...\nLevel config '%s' opened.\n", strFileName);
    WriteLog(str);

	do
	{
		result = cf->ParseLine();

		if (cf->GetToken()[0] == 0)
		{
			// no token

			continue;
		}

		if (strcmp(cf->GetToken(), "music") == 0)
		{
			m_pMusic = Mix_LoadMUS(cf->GetValue());

			if (m_pMusic != NULL)
			{
				sprintf(str, "Music '%s' successfully loaded.\n", cf->GetValue());
				WriteLog(str);
			}
			else
			{
				sprintf(str, "Music '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}
		}
		else if (strcmp(cf->GetToken(), "sound_plasma") == 0)
		{
			m_pSamplePlasma = Mix_LoadWAV(cf->GetValue());

			if (m_pSamplePlasma != NULL)
			{
				sprintf(str, "Sound '%s' successfully loaded.\n", cf->GetValue());
				WriteLog(str);
			}
			else
			{
				sprintf(str, "Sound '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}
		}
		else if (strcmp(cf->GetToken(), "sound_hit") == 0)
		{
			m_pSampleHit = Mix_LoadWAV(cf->GetValue());

			if (m_pSampleHit != NULL)
			{
				sprintf(str, "Sound '%s' successfully loaded.\n", cf->GetValue());
				WriteLog(str);
			}
			else
			{
				sprintf(str, "Sound '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}
		}
		else if (strcmp(cf->GetToken(), "sound_thrust") == 0)
		{
			m_pSampleThrust = Mix_LoadWAV(cf->GetValue());

			if (m_pSampleThrust != NULL)
			{
				sprintf(str, "Sound '%s' successfully loaded.\n", cf->GetValue());
				WriteLog(str);
			}
			else
			{
				sprintf(str, "Sound '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}
		}
		else if (strcmp(cf->GetToken(), "sound_plasma_hit") == 0)
		{
			m_pSamplePlasmaHit = Mix_LoadWAV(cf->GetValue());

			if (m_pSamplePlasmaHit != NULL)
			{
				sprintf(str, "Sound '%s' successfully loaded.\n", cf->GetValue());
				WriteLog(str);
			}
			else
			{
				sprintf(str, "Sound '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}
		}
		else if (strcmp(cf->GetToken(), "texture_particle") == 0)
		{
			m_texParticle.LoadTexture(cf->GetValue());

			if (m_texParticle.data != NULL)
			{
				glGenTextures(1, &m_texParticle.texID);
				glBindTexture(GL_TEXTURE_2D, m_texParticle.texID);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texParticle.width, m_texParticle.height,
					0, GL_RGB, GL_UNSIGNED_BYTE, m_texParticle.data);

				sprintf(str, "Texture '%s' ID: %d.\n", cf->GetValue(), m_texParticle.texID);
				WriteLog(str);

				if (glGetError() == GL_NO_ERROR)
				{
					sprintf(str, "Texture '%s' successfully loaded.\n", cf->GetValue());
					WriteLog(str);
				}
				else
				{
					sprintf(str, "Texture '%s' not loaded (OpenGL error).\n", cf->GetValue());
					WriteLog(str);
				}
			}
			else
			{
				sprintf(str, "Texture '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}

			m_texParticle.FreeData();
		}
		else if (strcmp(cf->GetToken(), "texture_plasma") == 0)
		{
			m_texPlasma.LoadTexture(cf->GetValue());

			if (m_texPlasma.data != NULL)
			{
				glGenTextures(1, &m_texPlasma.texID);
				glBindTexture(GL_TEXTURE_2D, m_texPlasma.texID);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texPlasma.width, m_texPlasma.height,
					0, GL_RGB, GL_UNSIGNED_BYTE, m_texPlasma.data);

				sprintf(str, "Texture '%s' ID: %d.\n", cf->GetValue(), m_texPlasma.texID);
				WriteLog(str);

				if (glGetError() == GL_NO_ERROR)
				{
					sprintf(str, "Texture '%s' successfully loaded.\n", cf->GetValue());
					WriteLog(str);
				}
				else
				{
					sprintf(str, "Texture '%s' not loaded (OpenGL error).\n", cf->GetValue());
					WriteLog(str);
				}
			}
			else
			{
				sprintf(str, "Texture '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}

			m_texPlasma.FreeData();
		}
		else if (strcmp(cf->GetToken(), "texture_font") == 0)
		{
			m_texFont.LoadTexture(cf->GetValue());

			if (m_texFont.data != NULL)
			{
				glGenTextures(1, &m_texFont.texID);
				glBindTexture(GL_TEXTURE_2D, m_texFont.texID);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texFont.width, m_texFont.height,
					0, GL_RGB, GL_UNSIGNED_BYTE, m_texFont.data);

				sprintf(str, "Texture '%s' ID: %d.\n", cf->GetValue(), m_texFont.texID);
				WriteLog(str);

				if (glGetError() == GL_NO_ERROR)
				{
					sprintf(str, "Texture '%s' successfully loaded.\n", cf->GetValue());
					WriteLog(str);
				}
				else
				{
					sprintf(str, "Texture '%s' not loaded (OpenGL error).\n", cf->GetValue());
					WriteLog(str);
				}
			}
			else
			{
				sprintf(str, "Texture '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}

			m_texFont.FreeData();
		}
		else if (strcmp(cf->GetToken(), "texture_arena") == 0)
		{
			m_texArena.LoadTexture(cf->GetValue());

			if (m_texArena.data != NULL)
			{
				glGenTextures(1, &m_texArena.texID);
				glBindTexture(GL_TEXTURE_2D, m_texArena.texID);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texArena.width, m_texArena.height,
					0, GL_RGB, GL_UNSIGNED_BYTE, m_texArena.data);

				sprintf(str, "Texture '%s' ID: %d.\n", cf->GetValue(), m_texArena.texID);
				WriteLog(str);

				if (glGetError() == GL_NO_ERROR)
				{
					sprintf(str, "Texture '%s' successfully loaded.\n", cf->GetValue());
					WriteLog(str);
				}
				else
				{
					sprintf(str, "Texture '%s' not loaded (OpenGL error).\n", cf->GetValue());
					WriteLog(str);
				}
			}
			else
			{
				sprintf(str, "Texture '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}

			m_texArena.FreeData();
		}
		else if (strcmp(cf->GetToken(), "texture_planetoid") == 0)
		{
			m_texPlanetoid.LoadTexture(cf->GetValue());

			if (m_texPlanetoid.data != NULL)
			{
				glGenTextures(1, &m_texPlanetoid.texID);
				glBindTexture(GL_TEXTURE_2D, m_texPlanetoid.texID);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texPlanetoid.width, m_texPlanetoid.height,
					0, GL_RGB, GL_UNSIGNED_BYTE, m_texPlanetoid.data);

				sprintf(str, "Texture '%s' ID: %d.\n", cf->GetValue(), m_texPlanetoid.texID);
				WriteLog(str);

				if (glGetError() == GL_NO_ERROR)
				{
					sprintf(str, "Texture '%s' successfully loaded.\n", cf->GetValue());
					WriteLog(str);
				}
				else
				{
					sprintf(str, "Texture '%s' not loaded (OpenGL error).\n", cf->GetValue());
					WriteLog(str);
				}
			}
			else
			{
				sprintf(str, "Texture '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}

			m_texPlanetoid.FreeData();
		}
		else if (strcmp(cf->GetToken(), "texture_skybox_back") == 0)
		{
			m_texSkybox[0].LoadTexture(cf->GetValue());

			if (m_texSkybox[0].data != NULL)
			{
				glGenTextures(1, &m_texSkybox[0].texID);
				glBindTexture(GL_TEXTURE_2D, m_texSkybox[0].texID);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texSkybox[0].width, m_texSkybox[0].height,
					0, GL_RGB, GL_UNSIGNED_BYTE, m_texSkybox[0].data);

				sprintf(str, "Texture '%s' ID: %d.\n", cf->GetValue(), m_texSkybox[0].texID);
				WriteLog(str);

				if (glGetError() == GL_NO_ERROR)
				{
					sprintf(str, "Texture '%s' successfully loaded.\n", cf->GetValue());
					WriteLog(str);
				}
				else
				{
					sprintf(str, "Texture '%s' not loaded (OpenGL error).\n", cf->GetValue());
					WriteLog(str);
				}
			}
			else
			{
				sprintf(str, "Texture '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}

			m_texSkybox[0].FreeData();
		}
		else if (strcmp(cf->GetToken(), "texture_skybox_right") == 0)
		{
			m_texSkybox[1].LoadTexture(cf->GetValue());

			if (m_texSkybox[1].data != NULL)
			{
				glGenTextures(1, &m_texSkybox[1].texID);
				glBindTexture(GL_TEXTURE_2D, m_texSkybox[1].texID);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texSkybox[1].width, m_texSkybox[1].height,
					0, GL_RGB, GL_UNSIGNED_BYTE, m_texSkybox[1].data);

				sprintf(str, "Texture '%s' ID: %d.\n", cf->GetValue(), m_texSkybox[1].texID);
				WriteLog(str);

				if (glGetError() == GL_NO_ERROR)
				{
					sprintf(str, "Texture '%s' successfully loaded.\n", cf->GetValue());
					WriteLog(str);
				}
				else
				{
					sprintf(str, "Texture '%s' not loaded (OpenGL error).\n", cf->GetValue());
					WriteLog(str);
				}
			}
			else
			{
				sprintf(str, "Texture '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}

			m_texSkybox[1].FreeData();
		}
		else if (strcmp(cf->GetToken(), "texture_skybox_front") == 0)
		{
			m_texSkybox[2].LoadTexture(cf->GetValue());

			if (m_texSkybox[2].data != NULL)
			{
				glGenTextures(1, &m_texSkybox[2].texID);
				glBindTexture(GL_TEXTURE_2D, m_texSkybox[2].texID);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texSkybox[2].width, m_texSkybox[2].height,
					0, GL_RGB, GL_UNSIGNED_BYTE, m_texSkybox[2].data);

				sprintf(str, "Texture '%s' ID: %d.\n", cf->GetValue(), m_texSkybox[2].texID);
				WriteLog(str);

				if (glGetError() == GL_NO_ERROR)
				{
					sprintf(str, "Texture '%s' successfully loaded.\n", cf->GetValue());
					WriteLog(str);
				}
				else
				{
					sprintf(str, "Texture '%s' not loaded (OpenGL error).\n", cf->GetValue());
					WriteLog(str);
				}
			}
			else
			{
				sprintf(str, "Texture '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}

			m_texSkybox[2].FreeData();
		}
		else if (strcmp(cf->GetToken(), "texture_skybox_left") == 0)
		{
			m_texSkybox[3].LoadTexture(cf->GetValue());

			if (m_texSkybox[3].data != NULL)
			{
				glGenTextures(1, &m_texSkybox[3].texID);
				glBindTexture(GL_TEXTURE_2D, m_texSkybox[3].texID);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texSkybox[3].width, m_texSkybox[3].height,
					0, GL_RGB, GL_UNSIGNED_BYTE, m_texSkybox[3].data);

				sprintf(str, "Texture '%s' ID: %d.\n", cf->GetValue(), m_texSkybox[3].texID);
				WriteLog(str);

				if (glGetError() == GL_NO_ERROR)
				{
					sprintf(str, "Texture '%s' successfully loaded.\n", cf->GetValue());
					WriteLog(str);
				}
				else
				{
					sprintf(str, "Texture '%s' not loaded (OpenGL error).\n", cf->GetValue());
					WriteLog(str);
				}
			}
			else
			{
				sprintf(str, "Texture '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}

			m_texSkybox[3].FreeData();
		}
		else if (strcmp(cf->GetToken(), "texture_skybox_top") == 0)
		{
			m_texSkybox[4].LoadTexture(cf->GetValue());

			if (m_texSkybox[4].data != NULL)
			{
				glGenTextures(1, &m_texSkybox[4].texID);
				glBindTexture(GL_TEXTURE_2D, m_texSkybox[4].texID);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texSkybox[4].width, m_texSkybox[4].height,
					0, GL_RGB, GL_UNSIGNED_BYTE, m_texSkybox[4].data);

				sprintf(str, "Texture '%s' ID: %d.\n", cf->GetValue(), m_texSkybox[4].texID);
				WriteLog(str);

				if (glGetError() == GL_NO_ERROR)
				{
					sprintf(str, "Texture '%s' successfully loaded.\n", cf->GetValue());
					WriteLog(str);
				}
				else
				{
					sprintf(str, "Texture '%s' not loaded (OpenGL error).\n", cf->GetValue());
					WriteLog(str);
				}
			}
			else
			{
				sprintf(str, "Texture '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}

			m_texSkybox[4].FreeData();
		}
		else if (strcmp(cf->GetToken(), "texture_skybox_bottom") == 0)
		{
			m_texSkybox[5].LoadTexture(cf->GetValue());

			if (m_texSkybox[5].data != NULL)
			{
				glGenTextures(1, &m_texSkybox[5].texID);
				glBindTexture(GL_TEXTURE_2D, m_texSkybox[5].texID);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texSkybox[5].width, m_texSkybox[5].height,
					0, GL_RGB, GL_UNSIGNED_BYTE, m_texSkybox[5].data);

				sprintf(str, "Texture '%s' ID: %d.\n", cf->GetValue(), m_texSkybox[5].texID);
				WriteLog(str);

				if (glGetError() == GL_NO_ERROR)
				{
					sprintf(str, "Texture '%s' successfully loaded.\n", cf->GetValue());
					WriteLog(str);
				}
				else
				{
					sprintf(str, "Texture '%s' not loaded (OpenGL error).\n", cf->GetValue());
					WriteLog(str);
				}
			}
			else
			{
				sprintf(str, "Texture '%s' not loaded.\n", cf->GetValue());
				WriteLog(str);
			}

			m_texSkybox[5].FreeData();
		}
		else if (strcmp(cf->GetToken(), "font_color_r") == 0)
		{
			m_faFontColor[0] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "font_color_g") == 0)
		{
			m_faFontColor[1] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "font_color_b") == 0)
		{
			m_faFontColor[2] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "font_color_a") == 0)
		{
			m_faFontColor[3] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "plasma_color_r") == 0)
		{
			m_faPlasmaColor[0] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "plasma_color_g") == 0)
		{
			m_faPlasmaColor[1] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "plasma_color_b") == 0)
		{
			m_faPlasmaColor[2] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "plasma_color_a") == 0)
		{
			m_faPlasmaColor[3] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "arena_color_r") == 0)
		{
			m_faArenaColor[0] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "arena_color_g") == 0)
		{
			m_faArenaColor[1] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "arena_color_b") == 0)
		{
			m_faArenaColor[2] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "arena_color_a") == 0)
		{
			m_faArenaColor[3] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "planetoid_color_r") == 0)
		{
			m_faPlanetoidColor[0] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "planetoid_color_g") == 0)
		{
			m_faPlanetoidColor[1] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "planetoid_color_b") == 0)
		{
			m_faPlanetoidColor[2] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "planetoid_color_a") == 0)
		{
			m_faPlanetoidColor[3] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "planetoid_expl_alpha") == 0)
		{
			m_fPlanetoidExplAlpha = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light0_color_r") == 0)
		{
			m_faLight0Color[0] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light0_color_g") == 0)
		{
			m_faLight0Color[1] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light0_color_b") == 0)
		{
			m_faLight0Color[2] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light1_color_r") == 0)
		{
			m_faLight1Color[0] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light1_color_g") == 0)
		{
			m_faLight1Color[1] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light1_color_b") == 0)
		{
			m_faLight1Color[2] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light2_color_r") == 0)
		{
			m_faLight2Color[0] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light2_color_g") == 0)
		{
			m_faLight2Color[1] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light2_color_b") == 0)
		{
			m_faLight2Color[2] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light0_position_x") == 0)
		{
			m_faLight0Position[0] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light0_position_y") == 0)
		{
			m_faLight0Position[1] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light0_position_z") == 0)
		{
			m_faLight0Position[2] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light1_position_x") == 0)
		{
			m_faLight1Position[0] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light1_position_y") == 0)
		{
			m_faLight1Position[1] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light1_position_z") == 0)
		{
			m_faLight1Position[2] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light2_position_x") == 0)
		{
			m_faLight2Position[0] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light2_position_y") == 0)
		{
			m_faLight2Position[1] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light2_position_z") == 0)
		{
			m_faLight2Position[2] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light_ambient_r") == 0)
		{
			m_faLightAmbient[0] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light_ambient_g") == 0)
		{
			m_faLightAmbient[1] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "light_ambient_b") == 0)
		{
			m_faLightAmbient[2] = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "plasma_force") == 0)
		{
			m_fPlasmaForce = (float)atof(cf->GetValue());

			if (m_fPlasmaForce <= 0.0f)
			{
				m_fPlasmaForce = 1000.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_1_max_force") == 0)
		{
			m_fPlanetoid1MaxForce = (float)atof(cf->GetValue());

			if (m_fPlanetoid1MaxForce <= 0.0f)
			{
				m_fPlanetoid1MaxForce = 100.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_2_max_force") == 0)
		{
			m_fPlanetoid2MaxForce = (float)atof(cf->GetValue());

			if (m_fPlanetoid2MaxForce <= 0.0f)
			{
				m_fPlanetoid2MaxForce = 400.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_3_max_force") == 0)
		{
			m_fPlanetoid3MaxForce = (float)atof(cf->GetValue());

			if (m_fPlanetoid3MaxForce <= 0.0f)
			{
				m_fPlanetoid3MaxForce = 1600.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "plasma_mass") == 0)
		{
			m_fPlasmaMass = (float)atof(cf->GetValue());

			if (m_fPlasmaMass <= 0.0f)
			{
				m_fPlasmaMass = 0.05f;
			}
		}
		else if (strcmp(cf->GetToken(), "player_mass") == 0)
		{
			m_fPlayerMass = (float)atof(cf->GetValue());

			if (m_fPlayerMass <= 0.0f)
			{
				m_fPlayerMass = 0.025f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_1_mass") == 0)
		{
			m_fPlanetoid1Mass = (float)atof(cf->GetValue());

			if (m_fPlanetoid1Mass <= 0.0f)
			{
				m_fPlanetoid1Mass = 0.05f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_2_mass") == 0)
		{
			m_fPlanetoid2Mass = (float)atof(cf->GetValue());

			if (m_fPlanetoid2Mass <= 0.0f)
			{
				m_fPlanetoid2Mass = 0.2f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_3_mass") == 0)
		{
			m_fPlanetoid3Mass = (float)atof(cf->GetValue());

			if (m_fPlanetoid3Mass <= 0.0f)
			{
				m_fPlanetoid3Mass = 0.8f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_1_max_torque") == 0)
		{
			m_fPlanetoid1MaxTorque = (float)atof(cf->GetValue());

			if (m_fPlanetoid1MaxTorque <= 0.0f)
			{
				m_fPlanetoid1MaxTorque = 12.5f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_2_max_torque") == 0)
		{
			m_fPlanetoid2MaxTorque = (float)atof(cf->GetValue());

			if (m_fPlanetoid2MaxTorque <= 0.0f)
			{
				m_fPlanetoid2MaxTorque = 50.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_3_max_torque") == 0)
		{
			m_fPlanetoid3MaxTorque = (float)atof(cf->GetValue());

			if (m_fPlanetoid3MaxTorque <= 0.0f)
			{
				m_fPlanetoid3MaxTorque = 800.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_1_roughness") == 0)
		{
			m_fPlanetoid1Roughness = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "planetoid_2_roughness") == 0)
		{
			m_fPlanetoid2Roughness = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "planetoid_3_roughness") == 0)
		{
			m_fPlanetoid3Roughness = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "planetoid_1_min_size") == 0)
		{
			m_fPlanetoid1MininumSize = (float)atof(cf->GetValue());

			if (m_fPlanetoid1MininumSize < 0.0f)
			{
				m_fPlanetoid1MininumSize = 0.0f;
			}
			else if (m_fPlanetoid1MininumSize > 1.0f)
			{
				m_fPlanetoid1MininumSize = 1.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_2_min_size") == 0)
		{
			m_fPlanetoid2MininumSize = (float)atof(cf->GetValue());

			if (m_fPlanetoid2MininumSize < 0.0f)
			{
				m_fPlanetoid2MininumSize = 0.0f;
			}
			else if (m_fPlanetoid2MininumSize > 1.0f)
			{
				m_fPlanetoid2MininumSize = 1.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_3_min_size") == 0)
		{
			m_fPlanetoid3MininumSize = (float)atof(cf->GetValue());

			if (m_fPlanetoid3MininumSize < 0.0f)
			{
				m_fPlanetoid3MininumSize = 0.0f;
			}
			else if (m_fPlanetoid3MininumSize > 1.0f)
			{
				m_fPlanetoid3MininumSize = 1.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "plasma_frequency") == 0)
		{
			m_fPlasmaFrequency = (float)atof(cf->GetValue());

			if (m_fPlasmaFrequency <= 0.0f)
			{
				m_fPlasmaFrequency = 1.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "plasma_lifetime") == 0)
		{
			m_fPlasmaLifetime = (float)atof(cf->GetValue());
		}
		else if (strcmp(cf->GetToken(), "player_thrust") == 0)
		{
			m_fPlayerThrust = (float)atof(cf->GetValue());

			if (m_fPlayerThrust <= 0.0f)
			{
				m_fPlayerThrust = 0.5f;
			}
		}
		else if (strcmp(cf->GetToken(), "bounce") == 0)
		{
			m_fBounce = (float)atof(cf->GetValue());

			if (m_fBounce < 0.0f)
			{
				m_fBounce = 1.0f;
			}
			else if (m_fBounce > 1.0f)
			{
				m_fBounce = 1.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "coulomb_friction") == 0)
		{
			if (strcmp(cf->GetValue(), "infinity") == 0)
			{
				m_fCoulombFriction = -1.0f;
			}
			else
			{
				m_fCoulombFriction = (float)atof(cf->GetValue());
			}
		}
		else if (strcmp(cf->GetToken(), "music_volume") == 0)
		{
			m_fMusicVolume = (float)atof(cf->GetValue());

			if (m_fMusicVolume < 0.0f)
			{
				m_fMusicVolume = 0.0f;
			}
			else if (m_fMusicVolume > 1.0f)
			{
				m_fMusicVolume = 1.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "sound_volume") == 0)
		{
			m_fSoundVolume = (float)atof(cf->GetValue());

			if (m_fSoundVolume < 0.0f)
			{
				m_fSoundVolume = 0.0f;
			}
			else if (m_fSoundVolume > 1.0f)
			{
				m_fSoundVolume = 1.0f;
			}
		}
		else if (strcmp(cf->GetToken(), "plasma_health") == 0)
		{
			m_iPlasmaHealth = atoi(cf->GetValue());

			if (m_iPlasmaHealth <= 0)
			{
				m_iPlasmaHealth = 50;
			}
		}
		else if (strcmp(cf->GetToken(), "player_health") == 0)
		{
			m_iPlayerHealth = atoi(cf->GetValue());

			if (m_iPlayerHealth <= 0)
			{
				m_iPlayerHealth = 200;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_1_health") == 0)
		{
			m_iPlanetoid1Health = atoi(cf->GetValue());

			if (m_iPlanetoid1Health <= 0)
			{
				m_iPlanetoid1Health = 50;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_2_health") == 0)
		{
			m_iPlanetoid2Health = atoi(cf->GetValue());

			if (m_iPlanetoid2Health <= 0)
			{
				m_iPlanetoid2Health = 50;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_3_health") == 0)
		{
			m_iPlanetoid3Health = atoi(cf->GetValue());

			if (m_iPlanetoid3Health <= 0)
			{
				m_iPlanetoid3Health = 50;
			}
		}
		else if (strcmp(cf->GetToken(), "health_loss") == 0)
		{
			m_iHealthLoss = atoi(cf->GetValue());

			if (m_iHealthLoss < 0)
			{
				m_iHealthLoss = 50;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_1_complexity") == 0)
		{
			m_iPlanetoid1Complexity = atoi(cf->GetValue());

			if (m_iPlanetoid1Complexity <= 0)
			{
				m_iPlanetoid1Complexity = 2;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_2_complexity") == 0)
		{
			m_iPlanetoid2Complexity = atoi(cf->GetValue());

			if (m_iPlanetoid2Complexity <= 0)
			{
				m_iPlanetoid2Complexity = 3;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_3_complexity") == 0)
		{
			m_iPlanetoid3Complexity = atoi(cf->GetValue());

			if (m_iPlanetoid3Complexity <= 0)
			{
				m_iPlanetoid3Complexity = 4;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_1_quantity") == 0)
		{
			m_iPlanetoid1Quantity = atoi(cf->GetValue());

			if (m_iPlanetoid1Quantity <= 0)
			{
				m_iPlanetoid1Quantity = 0;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_2_quantity") == 0)
		{
			m_iPlanetoid2Quantity = atoi(cf->GetValue());

			if (m_iPlanetoid2Quantity <= 0)
			{
				m_iPlanetoid2Quantity = 0;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_3_quantity") == 0)
		{
			m_iPlanetoid3Quantity = atoi(cf->GetValue());

			if (m_iPlanetoid3Quantity <= 0)
			{
				m_iPlanetoid3Quantity = 0;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_tex_tiling") == 0)
		{
			m_iPlanetoidTexTiling = atoi(cf->GetValue());

			if (m_iPlanetoidTexTiling <= 0)
			{
				m_iPlanetoidTexTiling = 0;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_expl_colors") == 0)
		{
			if (strcmp(cf->GetValue(), "rg") == 0)
			{
				m_iaPlanetoidExplColors[0] = 0;
				m_iaPlanetoidExplColors[1] = 1;
				m_iaPlanetoidExplColors[2] = 2;
			}
			else if (strcmp(cf->GetValue(), "gr") == 0)
			{
				m_iaPlanetoidExplColors[0] = 1;
				m_iaPlanetoidExplColors[1] = 0;
				m_iaPlanetoidExplColors[2] = 2;
			}
			else if (strcmp(cf->GetValue(), "rb") == 0)
			{
				m_iaPlanetoidExplColors[0] = 0;
				m_iaPlanetoidExplColors[1] = 2;
				m_iaPlanetoidExplColors[2] = 1;
			}
			else if (strcmp(cf->GetValue(), "br") == 0)
			{
				m_iaPlanetoidExplColors[0] = 2;
				m_iaPlanetoidExplColors[1] = 0;
				m_iaPlanetoidExplColors[2] = 1;
			}
			else if (strcmp(cf->GetValue(), "gb") == 0)
			{
				m_iaPlanetoidExplColors[0] = 1;
				m_iaPlanetoidExplColors[1] = 2;
				m_iaPlanetoidExplColors[2] = 0;
			}
			else if (strcmp(cf->GetValue(), "bg") == 0)
			{
				m_iaPlanetoidExplColors[0] = 2;
				m_iaPlanetoidExplColors[1] = 1;
				m_iaPlanetoidExplColors[2] = 0;
			}
		}
		else if (strcmp(cf->GetToken(), "arena_transparency") == 0)
		{
			if (strcmp(cf->GetValue(), "true") == 0)
			{
				m_bArenaTransparency = true;
			}
			else
			{
				m_bArenaTransparency = false;
			}
		}
		else if (strcmp(cf->GetToken(), "light0_enabled") == 0)
		{
			if (strcmp(cf->GetValue(), "true") == 0)
			{
				m_bLight0Enabled = true;
			}
			else
			{
				m_bLight0Enabled = false;
			}
		}
		else if (strcmp(cf->GetToken(), "light1_enabled") == 0)
		{
			if (strcmp(cf->GetValue(), "true") == 0)
			{
				m_bLight1Enabled = true;
			}
			else
			{
				m_bLight1Enabled = false;
			}
		}
		else if (strcmp(cf->GetToken(), "light2_enabled") == 0)
		{
			if (strcmp(cf->GetValue(), "true") == 0)
			{
				m_bLight2Enabled = true;
			}
			else
			{
				m_bLight2Enabled = false;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_transparency") == 0)
		{
			if (strcmp(cf->GetValue(), "true") == 0)
			{
				m_bPlanetoidTransparency = true;
			}
			else
			{
				m_bPlanetoidTransparency = false;
			}
		}
		else if (strcmp(cf->GetToken(), "planetoid_wireframe") == 0)
		{
			if (strcmp(cf->GetValue(), "true") == 0)
			{
				m_bPlanetoidWireframe = true;
			}
			else
			{
				m_bPlanetoidWireframe = false;
			}
		}
		else if (strcmp(cf->GetToken(), "plasma_arena_bounce") == 0)
		{
			if (strcmp(cf->GetValue(), "true") == 0)
			{
				m_bPlasmaArenaBounce = true;
			}
			else
			{
				m_bPlasmaArenaBounce = false;
			}
		}

	}while(result == 1);

	cf->CloseConfig();
	delete cf;
}

void CWorld::ReadMainConfig()
{
	// read from main config file

	int result = m_pConfigFile->OpenConfig();

	if (result == 0)
	{
		char sz[256];
		sprintf (sz, "Can't open: %s", m_pConfigFile->GetFilename());
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

		if (strcmp(m_pConfigFile->GetToken(), "crosshair") == 0)
		{
			strncpy(m_caCrosshair, m_pConfigFile->GetValue(), strlen(m_pConfigFile->GetValue()) + 1);
		}
		else if (strcmp(m_pConfigFile->GetToken(), "music_max_volume") == 0)
		{
			m_fMusicMaxVolume = (float)atof(m_pConfigFile->GetValue());

			if (m_fMusicMaxVolume < 0.0f)
			{
				m_fMusicMaxVolume = 0.0f;
			}
			else if (m_fMusicMaxVolume > 1.0f)
			{
				m_fMusicMaxVolume = 1.0f;
			}
		}
		else if (strcmp(m_pConfigFile->GetToken(), "sound_max_volume") == 0)
		{
			m_fSoundMaxVolume = (float)atof(m_pConfigFile->GetValue());

			if (m_fSoundMaxVolume < 0.0f)
			{
				m_fSoundMaxVolume = 0.0f;
			}
			else if (m_fSoundMaxVolume > 1.0f)
			{
				m_fSoundMaxVolume = 1.0f;
			}
		}
	}while(result == 1);

	m_pConfigFile->CloseConfig();
}

void CWorld::WriteLog(const char* str)
{
	FILE* log;
	log = fopen("log.txt", "a");
	fprintf(log, str);
	fclose(log);
}