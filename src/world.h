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
	world.h

	Author: Markus Liehmann
	Date: 02/08/2004
	Description: The CWorld class represents the game world of the engine. The world represents
		one level of ArP, and can also be the main-menu. Game objects like player, planetoids etc.
		are held here. The level config will be read from the corresponding config file.
*/

#ifndef __MM_WORLD_H__
#define __MM_WORLD_H__

#include "camera.h"
#include "Arena.h"
#include "Player.h"
#include "EnemyPlanetoid.h"
#include "ConfigFile.h"
#include "gui.h"
#include <ode/ode.h>
#include <SDL_mixer.h>

class CPlayer;
class CEnemyPlanetoid;
class CArena;
class CGUI;

class CWorld
{
private:

	// callback for ODE function dSpaceCollide

	static void nearCallback (void *data, dGeomID o1, dGeomID o2);

	// Search object-tree for dead planetoids and delete them; create 2 new planetoids if the
	// dead planetoid class was either 2 or 3 (middle and big planetoids)

	void CheckDeadPlanetoids(CEnemyPlanetoid* current, float deltaTime);

	// when world is filled with planetoids, random positions are generated and
	// evaluated in this function (no overlapping planetoids, no planetoid near player)

	int CheckPosition(CVector pos, float size);

	void SetDefaultValues();
	void ReadLevelConfig(const char* strFileName);
	void ReadMainConfig();
	void WriteLog(const char* str);

	// time till level ends after world done

	float m_fLevelEndDelay;

	// m_bShowMenu: is ingame menu shown? (pause)
	// m_bIsMenu: is world menu-world? (main screen)

	bool m_bWorldDone, m_bPlayMusic, m_bShowMenu, m_bIsMenu, m_bGameWon;

protected:
	void OnPrepare();

public:
	CArena* m_pArena;
	CCamera* m_pCamera;
	CPlayer* m_pPlayer;
	CObject* m_pPlanetoids;				// parent object for planetoids
	CObject* m_pParticles;				// parent object for particles
	CGUI* m_pGUI;
	CConfigFile* m_pConfigFile;
	dWorldID m_ODEWorld;
	dSpaceID m_ODESpace;
	dJointGroupID m_ODEContactGroup;
	int m_iScreenWidth, m_iScreenHeight;
	int m_iArenaSize;
	int m_iCurrentPlanetoids;
	char m_caCrosshair[256];			// string for crosshair, read from main ArP config file
	int m_iFPS;							// frames per second, only shown if not negative (set through engine)

	// configuration values read from level-config-file

	Mix_Music* m_pMusic;
	Mix_Chunk* m_pSampleHit;
	Mix_Chunk* m_pSamplePlasma;
	Mix_Chunk* m_pSampleThrust;
	Mix_Chunk* m_pSamplePlasmaHit;

	CTexture m_texParticle;
	CTexture m_texPlasma;
	CTexture m_texFont;
	CTexture m_texArena;
	CTexture m_texPlanetoid;
	CTexture m_texSkybox[6];

	float m_faFontColor[4], m_faPlasmaColor[4], m_faArenaColor[4], m_faPlanetoidColor[4];

	float m_faLight0Color[3], m_faLight1Color[3], m_faLight2Color[3],
		m_faLight0Position[3], m_faLight1Position[3], m_faLight2Position[3],
		m_faLightAmbient[3];

	float m_fPlasmaForce, m_fPlanetoid1MaxForce, m_fPlanetoid2MaxForce, m_fPlanetoid3MaxForce,
		m_fPlasmaMass, m_fPlayerMass, m_fPlanetoid1Mass, m_fPlanetoid2Mass, m_fPlanetoid3Mass,
		m_fPlanetoid1MaxTorque, m_fPlanetoid2MaxTorque, m_fPlanetoid3MaxTorque,
		m_fPlanetoid1Roughness, m_fPlanetoid2Roughness, m_fPlanetoid3Roughness,
		m_fPlanetoid1MininumSize, m_fPlanetoid2MininumSize, m_fPlanetoid3MininumSize,
		m_fPlasmaFrequency, m_fPlasmaLifetime, m_fPlayerThrust, m_fPlanetoidExplAlpha,
		m_fBounce, m_fCoulombFriction,
		m_fMusicMaxVolume, m_fSoundMaxVolume, m_fMusicVolume, m_fSoundVolume;

	int m_iPlasmaHealth, m_iPlayerHealth, m_iPlanetoid1Health, m_iPlanetoid2Health,
		m_iPlanetoid3Health, m_iHealthLoss, m_iPlanetoid1Complexity, m_iPlanetoid2Complexity,
		m_iPlanetoid3Complexity, m_iPlanetoid1Quantity, m_iPlanetoid2Quantity, m_iPlanetoid3Quantity,
		m_iPlayerLives, m_iPlanetoidTexTiling;

	int m_iaPlanetoidExplColors[3];

	bool m_bArenaTransparency, m_bLight0Enabled, m_bLight1Enabled, m_bLight2Enabled,
		m_bPlanetoidTransparency, m_bPlanetoidWireframe, m_bPlasmaArenaBounce;

	// end configuration values

	CWorld();
	CWorld(CConfigFile* cf, const char* strFileName, CCamera *c, int w, int h);
	~CWorld();

	void LoadWorld();
	void Animate(float deltaTime);	// move objects (ODE dWorldStep), check for dead planetoids
	void Draw(CCamera *camera);		// draw world and GUI
	void Prepare();					// calls OnPrepare; perform collision detection (ODE dSpaceCollide)

	bool IsWorldDone();
	bool IsGameWon();
	bool IsMenuShown();				// ingame menu shown?
	bool IsMenu();					// is world menu-world (main-screen)?
	void QuitWorld();
	void ShowMenu();				// show ingame menu (pause game)
	void PlayMusic();				// start and stop music
	void Focus(int gain);			// lost/gained focus
};

#endif