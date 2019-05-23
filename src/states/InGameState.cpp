
#include "InGameState.h"
#include "../objects/Player.h"
#include "../objects/Projectile.h"
#include "../objects/Enemy.h"
#include "../objects/Pickup.h"
#include "../objects/Powerup.h"
#include "../objects/MyParticle.h"
#include "../objects/Torch.h"
#include "../DefaultGame.h"

#include <ARK2D/Core/+.h>
#include <ARK2D/UI/+.h>
#include <ARK2D/Util/CameraShake.h>


SpawnPoint::SpawnPoint():
	x(0),
	y(0),
	MoveWithArenaMult(0.0f),
	currentTime(0.0f),
	startTime(1.0f),
	finishedAtTime(0.0f),
	started(false),
	finished(false)
	{

}
float SpawnPoint::getScale() {
	if (currentTime < startTime) {
		return 0.0f;
	} else if (currentTime >= startTime && currentTime <= startTime + 0.5f) {
		return Easing::easebetween(Easing::QUADRATIC_OUT, currentTime - startTime, 0.0f, 1.0f, 0.5);
	} else if (finished && currentTime >= finishedAtTime && currentTime <= finishedAtTime + 0.5f) {
		return Easing::easebetween(Easing::QUADRATIC_IN, currentTime - finishedAtTime, 1.0f, 0.0f, 0.5);
	}
	return 1.0f;
}



InGameState::InGameState():
	GameState(),
	m_rotationEffect()
	{

}

unsigned int InGameState::id() {
	return StatesList::STATE_INGAME;
}

void InGameState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {

}

void InGameState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {
	DefaultGame* dg = DefaultGame::getInstance();
	if (m_level > 0) m_firstPlay = false;
	m_introMessageTimer = 0;
	dg->hsvShader->set(1.0f, 1.0f, 1.0f);
}

void InGameState::init(GameContainer* container, StateBasedGame* game) {
	DefaultGame* dg = DefaultGame::getInstance();

	m_players = new Pool<Player*>(4);
	m_player = m_players->get();
	m_projectiles = new Pool<Projectile*>(5);
	m_enemies = new Pool<Enemy*>(20);
	m_pickups = new Pool<Pickup*>(10);
	m_powerups = new Pool<Powerup*>(10);
	m_particles = new Pool<MyParticle*>(30);

	m_cameraShake = new CameraShake(game, 5);

	//m_currentPowerupContainer = SpriteSheetStore::getImage("sprites/ui-corner-box.png");

	swirlSpawnImage = SpriteSheetStore::getImage("sprites/enemy-spawn.png");

	m_uiTutorialWASD = SpriteSheetStore::getImage("sprites/ui-tutorial-arrows.png");
	m_uiTutorialMouseLeft = SpriteSheetStore::getImage("sprites/ui-tutorial-mouse-left.png");
	m_uiTutorialMouseRight = SpriteSheetStore::getImage("sprites/ui-tutorial-mouse-right.png");
	m_uiTutorialMouseOff = SpriteSheetStore::getImage("sprites/ui-tutorial-mouse-off.png");

	m_uiTutorialMouseAnimation = new Animation();
	m_uiTutorialMouseAnimation->setFrameTime(0.25f);
	m_uiTutorialMouseAnimation->addFrame(m_uiTutorialMouseOff);
	m_uiTutorialMouseAnimation->addFrame(m_uiTutorialMouseLeft);

	m_uiTutorialMouseRightAnimation = new Animation();
	m_uiTutorialMouseRightAnimation->setFrameTime(0.25f);
	m_uiTutorialMouseRightAnimation->addFrame(m_uiTutorialMouseOff);
	m_uiTutorialMouseRightAnimation->addFrame(m_uiTutorialMouseRight);



	arenaBGSideLeft = SpriteSheetStore::getImage("sprites/arena/side-left.png");
	arenaBGSideRight = SpriteSheetStore::getImage("sprites/arena/side-right.png");
	arenaBGSideEdge = SpriteSheetStore::getImage("sprites/arena/side-edge.png");
	arenaBGCentrePiece = SpriteSheetStore::getImage("sprites/arena/side-middle.png");

	uiBottomBar = SpriteSheetStore::getImage("sprites/arena/ui-area.png");

	uiBottomBarPieceEmpty = SpriteSheetStore::getImage("sprites/ui-bar-empty.png");
	uiBottomBarPieceFull = SpriteSheetStore::getImage("sprites/ui-bar-full.png");

	colorRed = new Color("#cb2347");
	colorOrange = new Color("#e06548");
	colorYellow = new Color("#feff6e");

	m_level = 0;
	m_maxLevels = 10;

	m_startWaveTimer = 0.0f;
	m_startWaveDuration = 2.5f;

	m_paused = false;

	m_introMessageIndex = 0;
	m_introMessages = {
		"OH NO!",
		"THIS IS FINE...",
		"ANOTHER DAY AT THE OFFICE",
		"IT'S HOT!",
		"I DON'T GET PAID ENOUGH FOR THIS..."
	};

	m_firstPlay = true;

	start();
}

void InGameState::reset() {

	m_player->reset();
	m_projectiles->reset();
	m_enemies->reset();
	m_pickups->reset();
	m_powerups->reset();
	m_particles->reset();

	m_suicideTimer = 0.0f;
	m_suicideDuration = 3.0f;

	m_score = 0;
	//m_coins = 0;
	m_level = 0;

	m_introTimer = 0.01f;
	m_introDuration = 0.5f;

	m_introMessageTimer = 0.0f;
	m_introMessageDuration = 4.8f;



	m_introMessageIndex = (m_introMessageIndex + 1) % m_introMessages.size();

	m_deadTimer = 0.0f;
	m_deadDuration = 2.0f;

	m_bombExplodedFilterTimer = 0.0f;
	m_bombExplodedFilterDuration = 1.0f;

	m_flashTimer = 0.0f;
	m_flashDuration = 0.25f;

	m_levelChangeTimer = 0.0f;
	m_levelChangeDuration = 1.0f;
	m_levelChanged = false;

	m_currentPowerup = NULL;
	m_currentPowerupType = PT_NONE;

	m_darknessAlpha = 0.5f;

	m_rotationEffect.happeningTimer = 0.0f;
	m_rotationEffect.happeningDuration = 1.0f;
	m_rotationEffect.halfWayEvent = false;
}

void InGameState::onScoreChange() {




	if (m_score >= 12) {
        DefaultGame* dg = DefaultGame::getInstance();
		//smartBomb();

		/*int santas = countEnemiesOfType(ET_SANTA);
		if (santas > 0) {
			PoolIterator<Enemy*> it_enemies = m_enemies->newiteratorref();
			while(it_enemies.hasNext()){
				Enemy* e = it_enemies.next();
				if (e->m_type == ET_SANTA) {
					e->m_santaAlive = false;

					MyParticle* particle = m_particles->get();
					particle->reset();
					particle->m_type = MyParticle::TYPE_PLAYER_GIB;
					particle->m_bounds->setLocationByCenter(e->m_bounds->getCenterX(), e->m_bounds->getCenterY());
					particle->m_animation->reset();
					particle->m_animation->addFrame(Enemy::s_santaDying);
					particle->m_useGravity = true;
					particle->m_rotateToVelocity = true;
					particle->m_lifeScaleStart = 1.0f;
					particle->m_lifeScaleEnd = 0.0f;
					particle->m_lifetime = true;
					MathUtil::moveAngle<float>(particle->m_velocity, 90, MathUtil::randBetweenf(0, 1.0));
					dg->m_sfxBomb->play();

				}
			}
		}*/

		PoolIterator<Powerup*> it_powerups = m_powerups->newiteratorref();
		while(it_powerups.hasNext()){
			it_powerups.next()->setPendingRemoval(true);
		}

		PoolIterator<Projectile*> it_projectiles = m_projectiles->newiteratorref();
		while(it_projectiles.hasNext()){
			it_projectiles.next()->setPendingRemoval(true);
		}
		m_flashTimer = 0.01f;


		dg->m_musicCurrent->stop();
		//dg->enterState(dg->stateSummary);
		m_deadTimer = 0.01f;
		m_deadDuration = 4.0f;
		//m_win = true;
	}
}

void InGameState::start() {
	reset();
	m_died = false;
	m_win = false;

	m_introMessageTimer = 0.01f;

	DefaultGame* dg = DefaultGame::getInstance();
	m_level = (!dg->stateUpgrades->hasBasementKey())?0:4;
	startWave(m_level);
}

int InGameState::countPickupsOfType(PickupType pt) {

	int count = 0;
	PoolIterator<Pickup*> it = m_pickups->newiteratorref();
	while(it.hasNext()) {
		Pickup* e = it.next();
		if (!e->isPendingRemoval() && e->m_type == pt) {
			count++;
		}
	}
	return count;
}
int InGameState::countProjectilesOfType(ProjectileType pt) {
	int count = 0;
	PoolIterator<Projectile*> it = m_projectiles->newiteratorref();
	while(it.hasNext()) {
		Projectile* e = it.next();
		if (!e->isPendingRemoval() && e->m_type == pt) {
			count++;
		}
	}
	return count;
}
int InGameState::countEnemiesOfType(EnemyType et) {
	int count = 0;
	PoolIterator<Enemy*> it = m_enemies->newiteratorref();
	while(it.hasNext()) {
		Enemy* e = it.next();
		if (!e->isPendingRemoval() && e->m_type == et) {
			count++;
		}
	}
	return count;
}

Projectile* InGameState::startFireAt(float x, float y) {
	Projectile* p = m_projectiles->get();
	p->reset();
	p->start(PT_FLAME);
	p->m_bounds->setLocationByCenter(x, y);
	p->m_from.set(x, y);
	p->m_to.set(x, y);
	p->m_grounded = true;
	p->m_z = 0;
	p->m_speed = 0;
	p->m_lightingRadius = 10.0f;
	p->m_hasLifetime = false;
	return p;
};

void InGameState::startWave(int level) {

	m_startWaveTimer = 0.01f;

    GameContainer* container = ARK2D::getContainer();
	m_level = level;

	arenaLeftX = 22;
	arenaRightX = ARK2D::getContainer()->getWidth() - 22;

	arenaTopY = 35;
	arenaBottomY = 132;

	m_spawns.clear();

	m_player->m_bounds->setLocationByCenter(container->getWidth()*0.5f, container->getHeight()*0.5f);


	m_projectiles->reset();
	m_enemies->reset();
	m_pickups->reset();
	m_powerups->reset();
	m_particles->reset();

	float arenaWidth = arenaRightX - arenaLeftX;
	float arenaHeight = arenaBottomY - arenaTopY;
	float arenaCY = arenaTopY + (arenaHeight*0.5f);


    // TUTORIAL
    if (m_level == TUTORIAL_LEVEL) {
    	arenaClosingSpeed = 0;

    	Torch* t;
    	t = m_torches->get();
    	t->reset();
    	t->m_bounds->setLocationByCenter(arenaLeftX + 10, arenaTopY + 10);

    	// t = m_torches->get();
    	// t->reset();
    	// t->m_bounds->setLocationByCenter(arenaLeftX + 10, arenaBottomY - 10);

    	// t = m_torches->get();
    	// t->reset();
    	// t->m_bounds->setLocationByCenter(arenaRightX - 10, arenaTopY + 10);

    	t = m_torches->get();
    	t->reset();
    	t->m_bounds->setLocationByCenter(arenaRightX - 10, arenaBottomY - 10);
    }
    else if (m_level == 0) { // level 1. 4 mini enemies. topleft and bottomright  (4 shots)
		arenaClosingSpeed = 3;
		m_darknessAlpha = 0.5;

		float initDelay = (m_firstPlay)?4:0;

		SpawnPoint one;
		one.x = arenaLeftX + 20;
		one.y = arenaTopY + 20;
		one.MoveWithArenaMult = 1.0;
		one.startTime = initDelay;
		for(int i = 0; i < 2; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_MINI;
			ev.delay = 1.5f + (i * 1.0f);
			one.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(one);

		SpawnPoint two;
		two.x = arenaRightX - 20;
		two.y = arenaBottomY - 20;
		two.MoveWithArenaMult = -1.0;
		two.startTime = initDelay;
		for(int i = 0; i < 2; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_MINI;
			ev.delay = 2.0f + (i * 0.5f);
			two.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(two);
	}
	else if (m_level == 1) { // level 2, 4 'small' enemies,bottomleft,middletop,bottomright. (8 shots)
		arenaClosingSpeed = 4;
		m_darknessAlpha = 0.5;

		SpawnPoint one;
		one.x = arenaLeftX + 10;
        one.y = arenaBottomY - 10;
        one.MoveWithArenaMult = 1.0;
		for(int i = 0; i < 1; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_SMALL;
			ev.delay = 2.0f + (i * 1.0f);
			one.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(one);

		SpawnPoint two;
		two.x = container->getWidth() * 0.5f;
        two.y = arenaTopY + 10;
        two.MoveWithArenaMult = 0.0;
		for(int i = 0; i < 2; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_SMALL;
			ev.delay = 1.5f + (i * 5.0f);
			two.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(two);

		SpawnPoint thr;
		thr.x = arenaRightX - 10;
        thr.y = arenaBottomY - 10;
        thr.MoveWithArenaMult = -1.0;
		for(int i = 0; i < 1; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_SMALL;
			ev.delay = 2.5f + (i * 1.0f);
			thr.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(thr);

		startFireAt(arenaLeftX + 5, arenaCY);
		startFireAt(arenaRightX - 5, arenaCY);
	}
	else if (m_level == 2) { // level 3. 4 small from top right. one med from bottom left. two flying. (4+4+2 = 10 shots)

		arenaClosingSpeed = 5;
		m_darknessAlpha = 0.85f;

		SpawnPoint one;
		one.x = arenaLeftX + 20;
        one.y = arenaBottomY - 20;
        one.MoveWithArenaMult = 1.0;
		for(int i = 0; i < 1; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_MED;
			ev.delay = 3.5f + (i * 1.0f);
			one.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(one);

		SpawnPoint two;
		two.MoveWithArenaMult = -1.0;
		two.x = arenaRightX - 20;
		two.y = arenaTopY + 20;
		for(int i = 0; i < 4; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_SMALL;
			ev.delay = 1.0f + (i * 0.5f);
			two.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(two);

		SpawnPoint thr;
		thr.MoveWithArenaMult = -1.0;
		thr.x = arenaRightX - 10;
		thr.y = arenaBottomY - 10;
		for(int i = 0; i < 1; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_FLYING;
			ev.delay = 5.0f + (i * 0.5f);
			thr.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(thr);

		SpawnPoint four;
		four.MoveWithArenaMult = 1.0;
		four.x = arenaLeftX + 10;
		four.y = arenaTopY + 10;
		for(int i = 0; i < 1; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_FLYING;
			ev.delay = 5.0f + (i * 0.5f);
			four.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(four);


	}
	else if (m_level == 3) { // level 4. 5 on top line and 5 on bottom line. 4 flying. (10 + 4 = 14 shots)
		arenaClosingSpeed = 6;
		m_darknessAlpha = 0.85f;

		for(unsigned int j = 0; j < 5; j++) {

			SpawnPoint one;
			one.MoveWithArenaMult = 1.0;
			one.x = arenaLeftX + ((arenaWidth/6)*(j+1));
	        one.y = arenaBottomY - 10;
			for(int i = 0; i < 1; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_MINI;
				ev.delay = 1.5f + (i * 0.5f);
				one.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(one);


			SpawnPoint two;
			two.MoveWithArenaMult = -1.0;
			two.x = arenaRightX - ((arenaWidth/6)*(j+1));
	        two.y = arenaTopY + 10;
			for(int i = 0; i < 1; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_MINI;
				ev.delay = 2.5f + (i * 0.5f);
				two.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(two);
		}

		SpawnPoint thr;
		thr.MoveWithArenaMult = -1.0;
		thr.x = arenaRightX - 10;
		thr.y = arenaBottomY - 10;
		for(int i = 0; i < 2; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_FLYING;
			ev.delay = 4.0f + (i * 0.5f);
			thr.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(thr);

		SpawnPoint four;
		four.MoveWithArenaMult = 1.0;
		four.x = arenaLeftX + 10;
		four.y = arenaTopY + 10;
		for(int i = 0; i < 2; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_FLYING;
			ev.delay = 4.0f + (i * 0.5f);
			four.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(four);

	}
	else if (m_level == 4) { // level 5. fire lines across top and bottom. 6 small enemies., 1 spitting (6+6+3 = 15 shots.)

		arenaClosingSpeed = 5;
		m_darknessAlpha = 0.9f;

		int firesAcross = 8;
		for(unsigned int j = 0; j < firesAcross; j += 1) {
			startFireAt(arenaLeftX + ((arenaWidth/(firesAcross+1))*(j+1)), arenaTopY + 5);
			startFireAt(arenaLeftX + ((arenaWidth/(firesAcross+1))*(j+1)), arenaBottomY - 5);
		}

		// spawns 3 at left and 3 at right.
		for(unsigned int j = 0; j < 3; j++) {
			SpawnPoint one;
			one.x = arenaLeftX + 5;
	        one.y = arenaTopY + ((arenaHeight/4)*(j+1));
	        one.MoveWithArenaMult = 1.0f;
			for(int i = 0; i < 1; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_SMALL;
				ev.delay = 1.5f + (i * 0.5f);
				one.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(one);

			SpawnPoint two;
			two.x = arenaRightX - 5;
	        two.y = arenaBottomY - ((arenaHeight/4)*(j+1));
	        two.MoveWithArenaMult = -1.0;
			for(int i = 0; i < 1; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_SMALL;
				ev.delay = 3.0f + (i * 0.5f);
				two.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(two);
		}

		SpawnPoint thr;
		thr.x = container->getWidth() * 0.5f;
        thr.y = arenaCY;
        thr.MoveWithArenaMult = 0.0;
		for(int i = 0; i < 1; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_SPITTING;
			ev.delay = 5.0f + (i * 2.0f);
			thr.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(thr);
	}

	else if (m_level == 5) { // level 6. circle pit,
							 // lots of small from edges  6 mini (6), 2 med(8), 6 flying (4) = 20 shots
							 // coming into the fire lolz!

		arenaClosingSpeed = 5;
		m_darknessAlpha = 1.0f;

		float cx = arenaLeftX + (arenaWidth*0.5f);
		float cy = arenaTopY + (arenaHeight*0.5f);
		float r = arenaHeight * 0.55f;

		for(unsigned int t = 0; t < 360; t += 36) {
			float thisX = cx;
			float thisY = cy;
			MathUtil::moveAngle<float>(thisX, thisY, t, r);
			startFireAt(thisX, thisY);
		}

		SpawnPoint one; // bl
		one.x = arenaLeftX + 10;
        one.y = arenaBottomY - 10;
        one.MoveWithArenaMult = 1.0;
		for(int i = 0; i < 3; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_MINI;
			ev.delay = 0.5f + (i * 1.0f);
			one.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(one);

		SpawnPoint two; //tr
		two.MoveWithArenaMult = -1.0;
		two.x = arenaRightX - 10;
		two.y = arenaTopY + 10;
		for(int i = 0; i < 3; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_MINI;
			ev.delay = 1.0f + (i * 0.5f);
			two.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(two);

		SpawnPoint three; // tl
		three.x = arenaLeftX + 10;
        three.y = arenaTopY + 10;
        three.MoveWithArenaMult = 1.0;
		for(int i = 0; i < 1; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_MED;
			ev.delay = 4.5f + (i * 1.0f);
			three.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(three);

		SpawnPoint four; // br
		four.MoveWithArenaMult = -1.0;
		four.x = arenaRightX - 10;
		four.y = arenaBottomY - 10;
		for(int i = 0; i < 1; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_MED;
			ev.delay = 4.5f + (i * 0.5f);
			four.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(four);

		SpawnPoint flying1; // center left
		flying1.MoveWithArenaMult = 1.0;
		flying1.x = arenaLeftX + 5;
		flying1.y = arenaCY;
		for(int i = 0; i < 3; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_FLYING;
			ev.delay = 6.0f + (i * 1.0f);
			flying1.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(flying1);

		SpawnPoint flying2; // center right
		flying2.MoveWithArenaMult = -1.0;
		flying2.x = arenaRightX - 5;
		flying2.y = arenaCY;
		for(int i = 0; i < 3; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_FLYING;
			ev.delay = 7.0f + (i * 1.0f);
			flying2.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(flying2);
	}
	else if (m_level == 6) {  // level 7 - 6 flying . 6 mini., 4 sml(8), 1 medium(4) = 24 shots
		arenaClosingSpeed = 4;
		m_darknessAlpha = 1.0f;

		SpawnPoint one;
		one.x = container->getWidth() * 0.5f;
        one.y = arenaCY;
        one.MoveWithArenaMult = 0.0;
		for(int i = 0; i < 6; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_FLYING;
			ev.delay = 1.5f + (i * 1.0f);
			one.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(one);

		SpawnPoint two;
		two.x = arenaLeftX + 20;
        two.y = arenaCY;
        two.MoveWithArenaMult = 1.0;
		for(int i = 0; i < 3; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_MINI;
			ev.delay = 5.0f + (i * 1.0f);
			two.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(two);


		SpawnPoint three;
		three.x = arenaRightX - 20;
        three.y = arenaCY;
        three.MoveWithArenaMult = -1.0;
		for(int i = 0; i < 3; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_MINI;
			ev.delay = 5.0f + (i * 1.0f);
			three.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(three);

		SpawnPoint four;
		four.x = arenaRightX - 5;
        four.y = arenaBottomY - 5;
        four.MoveWithArenaMult = -1.0;
		for(int i = 0; i < 1; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_MED;
			ev.delay = 6.0f + (i * 1.0f);
			four.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(four);

		SpawnPoint five;
		five.x = arenaLeftX + 5;
        five.y = arenaTopY + 5;
        five.MoveWithArenaMult = 1.0;
		for(int i = 0; i < 4; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_SMALL;
			ev.delay = 6.0f + (i * 1.0f);
			five.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(five);

	}
	else if (m_level == 7) { // level 8. circle pit, 4 med (16) + 6 flying, 6 mini = 28 shots.

		arenaClosingSpeed = 4;
		m_darknessAlpha = 1.0f;

		float cx = arenaLeftX + (arenaWidth*0.5f);
		float cy = arenaTopY + (arenaHeight*0.5f);
		float r = arenaHeight * 0.6f;

		for(unsigned int t = 0; t < 360; t += 36) {
			float thisX = cx;
			float thisY = cy;
			MathUtil::moveAngle<float>(thisX, thisY, t, r);

			startFireAt(thisX, thisY);
		}

		for(unsigned int j = 0; j < 4; j++) {
			SpawnPoint one;
			one.x = cx;
	        one.y = cy;
	        MathUtil::moveAngle<float>(one.x, one.y, j * 90, arenaHeight * 0.45f);

			for(int i = 0; i < 1; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_MED;
				ev.delay = 2.0f + (2.0f*j) + (i * 1.2f);
				one.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(one);
		}

		SpawnPoint onebl;
		onebl.x = arenaLeftX + 10;
        onebl.y = arenaBottomY - 10;
        onebl.MoveWithArenaMult = 1.0;
		for(int i = 0; i < 2; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_FLYING;
			ev.delay = 5.0f + (i * 1.0f);
			onebl.m_spawnEnemies.push_back(ev);

			SpawnEnemyEvent ev2;
			ev2.enemyType = ET_MINI;
			ev2.delay = 7.0f + (i * 0.5f);
			onebl.m_spawnEnemies.push_back(ev2);
		}
		m_spawns.push_back(onebl);

		SpawnPoint onetr;
		onetr.x = arenaRightX - 10;
        onetr.y = arenaTopY + 10;
        onetr.MoveWithArenaMult = -1.0;
		for(int i = 0; i < 2; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_FLYING;
			ev.delay = 5.0f + (i * 1.0f);
			onetr.m_spawnEnemies.push_back(ev);

			SpawnEnemyEvent ev2;
			ev2.enemyType = ET_MINI;
			ev2.delay = 7.0f + (i * 0.5f);
			onetr.m_spawnEnemies.push_back(ev2);
		}
		m_spawns.push_back(onetr);

	}
	else if (m_level == 8) { // level 9, 4 spitting enemies (12), 2 med enemies(8), 6 flying enemies (6), 2 small (4) = 30 shots.

		arenaClosingSpeed = 4;
		m_darknessAlpha = 1.0f;

		startFireAt(arenaLeftX + 5, arenaTopY + 5);
		startFireAt(arenaRightX - 5, arenaBottomY - 5);

		SpawnPoint one;
		one.x = container->getWidth() * 0.5f;
        one.y = arenaTopY + 10;
        one.MoveWithArenaMult = 0.0;
		for(int i = 0; i < 4; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_SPITTING;
			ev.delay = 2.5f + (i * 2);
			one.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(one);

		SpawnPoint two;
		two.x = container->getWidth() * 0.5f;
        two.y = arenaBottomY - 10;
        two.MoveWithArenaMult = 0.0;
		for(int i = 0; i < 2; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_MED;
			ev.delay = 1.5f + (i * 2);
			two.m_spawnEnemies.push_back(ev);

			SpawnEnemyEvent ev2;
			ev2.enemyType = ET_SMALL;
			ev2.delay = 10.5f + (i * 2);
			two.m_spawnEnemies.push_back(ev2);
		}
		m_spawns.push_back(two);

		SpawnPoint thr; // bl
		thr.x = arenaLeftX + 10;
        thr.y = arenaCY;
        thr.MoveWithArenaMult = 1.0;
		for(int i = 0; i < 3; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_FLYING;
			ev.delay = 4.0f + (i * 2);
			thr.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(thr);

		SpawnPoint four; // br
		four.x = arenaRightX - 10;
        four.y = arenaCY;
        four.MoveWithArenaMult = -1.0;
		for(int i = 0; i < 3; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_FLYING;
			ev.delay = 5.5f + (i * 2);
			four.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(four);

	}
	else if (m_level == 9) { // level 10. last level!
		// 8 minis (8), 4 smalls (8), 2 meds (8), 8 flying(8), 4 slugs(12), 2 large(20) = total 64

		arenaClosingSpeed = 1;
		m_darknessAlpha = 1.1f;

		startFireAt(container->getWidth()*0.5f, arenaTopY + 5);
		startFireAt(container->getWidth()*0.5f, arenaBottomY - 5);
		startFireAt(arenaLeftX + 5, arenaCY);
		startFireAt(arenaRightX - 5, arenaCY);

		{ // 8 minis, lc and rc (4)
			SpawnPoint one;
			one.x = arenaLeftX + 10;
	        one.y = arenaCY;
	        one.MoveWithArenaMult = 1.0;
			for(int i = 0; i < 4; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_MINI;
				ev.delay = 2.5f + (i*0.5);
				one.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(one);

			SpawnPoint two;
			two.x = arenaRightX - 10;
	        two.y = arenaCY;
	        two.MoveWithArenaMult = -1.0;
			for(int i = 0; i < 4; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_MINI;
				ev.delay = 3.0f + (i*0.5);
				two.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(two);
		}
		{ // 4 smalls, topc, botc. (4)
			SpawnPoint one;
			one.x = container->getWidth()*0.5f;
	        one.y = arenaTopY + 10;
	        one.MoveWithArenaMult = 0.0;
			for(int i = 0; i < 2; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_SMALL;
				ev.delay = 5.0f + (i*0.5);
				one.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(one);

			SpawnPoint two;
			two.x = container->getWidth()*0.5f;
	        two.y = arenaBottomY - 10;
	        two.MoveWithArenaMult = 0.0;
			for(int i = 0; i < 2; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_SMALL;
				ev.delay = 6.0f + (i*0.5);
				two.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(two);
		}
		{ // 2 mediums, tl, br (8)
			SpawnPoint one;
			one.x = arenaLeftX + 10;
	        one.y = arenaTopY + 10;
	        one.MoveWithArenaMult = 1.0;
			for(int i = 0; i < 1; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_MED;
				ev.delay = 7.5f + (i*0.5);
				one.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(one);

			SpawnPoint two;
			two.x = arenaRightX - 10;
	        two.y = arenaBottomY - 10;
	        two.MoveWithArenaMult = -1.0;
			for(int i = 0; i < 1; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_MED;
				ev.delay = 8.0f + (i*0.5);
				two.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(two);
		}

		{ // 8 flying + 4 slugs, centre + cl, cr (8+12 = 20)
			SpawnPoint flying;
			flying.x = container->getWidth()*0.5f;
	        flying.y = arenaCY;
	        flying.MoveWithArenaMult = 0.0;
			for(int i = 0; i < 8; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_FLYING;
				ev.delay = 10.5f + (i*1.0);
				flying.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(flying);

			SpawnPoint slug1;
			slug1.x = arenaLeftX + 10;
            slug1.y = arenaCY;
	        slug1.MoveWithArenaMult = 1.0;
			for(int i = 0; i < 2; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_SPITTING;
				ev.delay = 11.5f + (i*2.5);
				slug1.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(slug1);

			SpawnPoint slug2;
			slug2.x = arenaRightX - 10;
            slug2.y = arenaCY;
	        slug2.MoveWithArenaMult = -1.0;
			for(int i = 0; i < 2; i++) {
				SpawnEnemyEvent ev;
				ev.enemyType = ET_SPITTING;
				ev.delay = 11.5f + (i*2.5);
				slug2.m_spawnEnemies.push_back(ev);
			}
			m_spawns.push_back(slug2);
		}

		// boss. (20)
		SpawnPoint finalboss;
		finalboss.x = container->getWidth()*0.5f;
        finalboss.y = arenaCY;
        finalboss.MoveWithArenaMult = 0.0;
		for(int i = 0; i < 2; i++) {
			SpawnEnemyEvent ev;
			ev.enemyType = ET_LARGE;
			ev.delay = 15.0f + (i*5);
			finalboss.m_spawnEnemies.push_back(ev);
		}
		m_spawns.push_back(finalboss);


	}
	// grid of fire? spawns interspersed.

    DefaultGame* dg = DefaultGame::getInstance();
	dg->hsvShader->set(0.005f * float(level), 1.0, 1.0f);
	//dg->hsvShader->set(0.1f * float(level), 1.0, 1.0f);

}

int InGameState::countFinishedSpawnPoints() {
	int count = 0;
	for(unsigned int i = 0; i < m_spawns.size(); i++) {
		if (m_spawns[i].finished) {
			count++;
		}
	}
	return count;
}

void InGameState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {
	DefaultGame* dg = DefaultGame::getInstance();
	Input* in = ARK2D::getInput();

	// if (!dg->m_musicCurrent->isPlaying() && m_deadTimer == 0.0f) {
	// 	dg->m_musicCurrent->play();
	// }

	if (in->isKeyPressed(Input::KEY_ESCAPE) ||
		in->isKeyPressed(Input::KEY_BACKSPACE) ||
		in->isGamepadButtonPressed(Gamepad::BUTTON_BACK)) {
		m_player->die();
	}

	if (in->isKeyPressed(Input::KEY_ENTER) || in->isGamepadButtonPressed(Gamepad::BUTTON_START)) {
		m_paused = !m_paused;
	}

	if (m_paused) { return; }

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer > m_introDuration) {
			m_introTimer = 0.0f;
		}
	}
	if (m_introMessageTimer > 0.0f) {
		m_uiTutorialMouseAnimation->update(timer);
		m_uiTutorialMouseRightAnimation->update(timer);
		m_introMessageTimer += timer->getDelta();
		if (m_introMessageTimer > m_introMessageDuration) {
			m_introMessageTimer = 0.0f;
		}
	}
	if (m_startWaveTimer > 0.0f) {
		m_startWaveTimer += timer->getDelta();
		if (m_startWaveTimer > m_startWaveDuration) {
			m_startWaveTimer = 0.0f;
		}
	}

	for(unsigned int i = 0; i < m_spawns.size(); i++) {
		SpawnPoint* sp = &m_spawns.at(i);
		sp->currentTime += timer->getDelta();
		if (sp->currentTime >= sp->startTime) {

			for(int j = 0; j < sp->m_spawnEnemies.size(); j++){
				SpawnEnemyEvent* ev = &sp->m_spawnEnemies.at(j);
				ev->delay -= timer->getDelta();
				if (ev->delay < 0.0f) {
					m_cameraShake->start(4, 0.1f);
					Enemy* e = spawnEnemy(ev->enemyType);
					e->m_bounds->setLocationByCenter(sp->x, sp->y);
					sp->m_spawnEnemies.erase(sp->m_spawnEnemies.begin() + j);
					break;
				}
			}

			if (sp->m_spawnEnemies.size() == 0 && sp->finishedAtTime <= 0.0f && !sp->finished) {
				sp->finishedAtTime = sp->currentTime;
			}
			if (sp->finishedAtTime > 1.0f && sp->currentTime >= sp->finishedAtTime + 1.0f) {
				sp->finished = true;
			}
		}
	}

	if (m_enemies->sizeActive() == 0 && countFinishedSpawnPoints() == m_spawns.size()) {

		if (countPickupsOfType(PU_LADDER) == 0) {
			Pickup* p = m_pickups->get();
			p->reset();
			p->start(PU_LADDER);
			p->m_bounds->setLocationByCenter(container->getWidth()*0.5, container->getHeight()*0.5);

			dg->m_sfxLadder->play();

			arenaClosingSpeed = 0;
		}
	}


	if (DefaultGame::s_debug) {
		if (in->isKeyPressed(Input::KEY_Y)) {
			spawnFlyingEnemy();
		}
		if (in->isKeyPressed(Input::KEY_T)) {
			spawnSpittingEnemy();
		}

		if (in->isKeyPressed(Input::KEY_U)) {
			spawnSmoke(container->getWidth()*0.5f, container->getHeight()*0.5f, 3.5f, 8.0f);
		}
		if (in->isKeyDown(Input::KEY_I)) {
			m_darknessAlpha = std::max(m_darknessAlpha - timer->getDelta(), 0.0f);
		} else if (in->isKeyDown(Input::KEY_O)) {
			m_darknessAlpha = std::min(m_darknessAlpha + timer->getDelta(), 1.0f);
		}

		if (in->isKeyPressed(Input::KEY_HYPHEN)) {
			smartBomb();
		}
		if (in->isKeyPressed(Input::KEY_R)) {
			m_projectiles->reset();
		}
	}


	arenaLeftX += arenaClosingSpeed * timer->getDelta();
	if (arenaLeftX > 80) {
		arenaLeftX = 80;
	}
	for(unsigned int i = 0; i < m_spawns.size(); i++) {
		if (m_spawns[i].MoveWithArenaMult != 0) {
			m_spawns[i].x += arenaClosingSpeed * timer->getDelta() * m_spawns[i].MoveWithArenaMult;
		}
	}
	arenaRightX -= arenaClosingSpeed * timer->getDelta();
	if (arenaRightX < container->getWidth() - 80) {
		arenaRightX = container->getWidth() - 80;
	}


	if (m_player->m_fireAmmo == 0 && m_player->m_waterAmmo == 0 && countPickupsOfType(PU_LADDER) == 0 && m_suicideTimer == 0.0f) {
		m_suicideTimer = 0.01f;
	}

	if (m_suicideTimer > 0.0f) {
		m_suicideTimer += timer->getDelta();
		if (m_suicideTimer >= m_suicideDuration) {
			m_player->die();
		}
	}



	if (m_flashTimer > 0.0f) {
		m_flashTimer += timer->getDelta();
		if (m_flashTimer >= m_flashDuration) {
			m_flashTimer = 0.0f;
		}
	}

	m_players->updateAll(container, timer);
	m_projectiles->updateAll(container, timer);
	m_enemies->updateAll(container, timer);
	m_pickups->updateAll(container, timer);
	m_powerups->updateAll(container, timer);
	m_particles->updateAll(container, timer);

	m_cameraShake->update(container, timer);



	if (m_rotationEffect.happeningTimer > 0.0f) {
		//levelTimer = 0.0f;
		m_rotationEffect.happeningTimer += timer->getDelta();
		if (m_rotationEffect.happeningTimer >= m_rotationEffect.happeningDuration*0.5f && !m_rotationEffect.halfWayEvent) {
			//startLevel(level + 1);
			//m_rotationEffect.halfWayEvent = true;
			//score += 5;
		}
		if (m_rotationEffect.happeningTimer >= m_rotationEffect.happeningDuration) {
			m_rotationEffect.happeningTimer = 0.0f;
			m_rotationEffect.halfWayEvent = false;
		}
	}

	if (!m_died) {
		/*
		if (countEnemiesOfType(ET_SANTA) == 0) {
			m_spawnSantaTimer += timer->getDelta();
			if (m_spawnSantaTimer >= m_spawnSantaDuration) {
				spawnSanta();
				m_spawnSantaTimer = 0.0f;
				m_spawnSantaDuration = MathUtil::randBetweenf(m_spawnSantaDurationMin, m_spawnSantaDurationMax);
			}
		}

		if (countEnemiesOfType(ET_MOLE) < m_spawnMoleMaxConcurrent) {
			m_spawnMoleTimer += timer->getDelta();
			if (m_spawnMoleTimer >= m_spawnMoleDuration) {
				spawnMole();
				m_spawnMoleTimer = 0.0f;
			}
		}

		if (countEnemiesOfType(ET_MISTLETOE) < m_spawnMistletoeMaxConcurrent) {
			m_spawnMistletoeTimer += timer->getDelta();
			if (m_spawnMistletoeTimer >= m_spawnMistletoeDuration) {
				spawnMistletoe();
				m_spawnMistletoeTimer = 0.0f;
			}
		}

		if (countEnemiesOfType(ET_CHRISTMASPUDDING) < m_spawnPuddingMaxConcurrent) {
			m_spawnPuddingTimer += timer->getDelta();
			if (m_spawnPuddingTimer >= m_spawnPuddingDuration) {
				spawnPudding();
				m_spawnPuddingTimer = 0.0f;
			}
		}

		m_spawnPowerupTimer += timer->getDelta();
		if (m_spawnPowerupTimer >= m_spawnPowerupDuration && m_powerups->sizeActive() == 0) {
			spawnPowerup();
			m_spawnPowerupTimer = 0.0f;
		}


		*/

		doCollisions();
	}

	if (m_levelChangeTimer > 0.0f) {
		m_levelChangeTimer += timer->getDelta();
		if (m_levelChangeTimer >= m_levelChangeDuration * 0.5f && !m_levelChanged) {
			m_levelChanged = true;
			startWave(m_level+1);
		}
		if (m_levelChangeTimer >= m_levelChangeDuration) {
			m_levelChangeTimer = 0.0f;
		}

	}

    m_projectiles->pruneAll();
	m_enemies->pruneAll();
	m_pickups->pruneAll();
	m_powerups->pruneAll();
	m_particles->pruneAll();

	if (m_died || m_win || m_deadTimer > 0.0f) {
		m_deadTimer += timer->getDelta();
		if (m_deadTimer >= m_deadDuration) {
			//dg->m_coins += m_coins;
			if (m_win) {
				dg->enterState(dg->stateSummary);
			} else {
				dg->enterState(dg->stateUpgrades);
			}

		}
	}


}
void InGameState::doCollisions() {

	auto fireBecomeSmoke = [this](Projectile* pr) -> void {
		pr->start(PT_FLAME_EMBERS);
		spawnSmoke( pr->m_bounds->getCenterX(), pr->m_bounds->getCenterY(), pr->m_radius, 8.0f, colorRed );
		spawnSmoke( pr->m_bounds->getCenterX(), pr->m_bounds->getCenterY(), pr->m_radius, 6.0f, colorOrange->pointer() );
		spawnSmoke( pr->m_bounds->getCenterX(), pr->m_bounds->getCenterY(), pr->m_radius, 4.0f, colorYellow->pointer() );
	};

	PoolIterator<Projectile*> it_projectiles = m_projectiles->newiteratorref();
	while (it_projectiles.hasNext()) {
		Projectile* pr = it_projectiles.next();
		if (!pr->isPendingRemoval()) {

			if (pr->m_type == PT_FLAME)
			{
				bool canCollidePlayer = (pr->m_grounded || (!pr->m_grounded && !pr->m_isFromPlayer)) && m_player->isDamagable();
				if (canCollidePlayer && pr->m_bounds->collides(m_player->m_bounds)) {

					if (!m_player->m_burnProtection) {
						m_player->hitByProjectile(pr);
					} else {
						fireBecomeSmoke(pr);
						spawnSmoke( m_player->m_bounds->getCenterX(), m_player->m_bounds->getMaxY(), m_player->m_bounds->getWidth()*0.5f, 8.0f, Color::white.pointer(), -95, -85);
					}
				}

				if (pr->m_grounded) {

					PoolIterator<Projectile*> it_projectilesAgain = m_projectiles->newiteratorref();
					while (it_projectilesAgain.hasNext()) {
						Projectile* prTwo = it_projectilesAgain.next();
						if (!prTwo->isPendingRemoval() && prTwo != pr && prTwo->m_type == PT_CO2) {

							if (pr->m_bounds->collides(prTwo->m_bounds)) {
								prTwo->setPendingRemoval(true);

								m_player->m_fireAmmo = std::min(m_player->m_fireAmmo + 1, m_player->m_fireMax);

								fireBecomeSmoke(pr);

								float Co2SmokeAngle = MathUtil::anglef( prTwo->m_from.getX(), prTwo->m_from.getY(), prTwo->m_to.getX(), prTwo->m_to.getY() );
								spawnSmoke( prTwo->m_bounds->getCenterX(), prTwo->m_bounds->getCenterY(), prTwo->m_radius, 8.0f, Color::white.pointer(),  Co2SmokeAngle - 5, Co2SmokeAngle + 5);
							}

						}
					}

				}
			}
		}
	}

	PoolIterator<Enemy*> it_enemies = m_enemies->newiteratorref();
	while (it_enemies.hasNext()) {
		Enemy* e = it_enemies.next();
		if (!e->isPendingRemoval()) {

			if (e->isDamagable())  {

				it_projectiles = m_projectiles->newiteratorref();
				while (it_projectiles.hasNext()) {
					Projectile* pr = it_projectiles.next();
					if (!pr->isPendingRemoval() && pr->m_type == PT_FLAME && pr->canDamageEnemy()) {

						if (e->m_bounds->collides(pr->m_bounds)) {
							e->hit();
							e->smoke();

                            DefaultGame* dg = DefaultGame::getInstance();


								for(int i = 0; i < e->m_dropsCoinAmount; i++) {
									if (MathUtil::randBetweenf(0.0f, 1.0f) <= e->m_dropsCoinChance) {
										if (dg->m_coins + countPickupsOfType(PU_COIN) < dg->stateUpgrades->getCurrentMaxWallet()) {
											Pickup* p = m_pickups->get();
											p->reset();
											p->start(PU_COIN);
											p->m_bounds->setLocationByCenter(e->m_bounds->getCenterX(), e->m_bounds->getCenterY());

											float angle = MathUtil::anglef(pr->m_from.getX(), pr->m_from.getY(), pr->m_to.getX(), pr->m_to.getY());
											p->m_velocity->set(0, 0);
											MathUtil::moveAngle<float>(p->m_velocity, MathUtil::randBetweenf(angle-10,angle+10), MathUtil::randBetweenf(pr->m_speed*0.9f, pr->m_speed*1.1f));
										}
									}
								}


							//if (pr->m_grounded) {
								m_player->m_fireAmmo = std::min(m_player->m_fireAmmo + 1, m_player->m_fireMax);

								pr->start(PT_FLAME_EMBERS);
								spawnSmoke( pr->m_bounds->getCenterX(), pr->m_bounds->getCenterY(), pr->m_radius, 8.0f, colorRed );
								spawnSmoke( pr->m_bounds->getCenterX(), pr->m_bounds->getCenterY(), pr->m_radius, 6.0f, colorOrange->pointer() );
								spawnSmoke( pr->m_bounds->getCenterX(), pr->m_bounds->getCenterY(), pr->m_radius, 4.0f, colorYellow->pointer() );
							//}
							break;
						}
					}
				}
			}

			if (m_player->isDamagable() && e->m_bounds->collides(m_player->m_bounds)) {
				m_player->hitByEnemy(e);
				break;
			}
		}
	}


	PoolIterator<Pickup*> it_pickups2 = m_pickups->newiteratorref();
	while (it_pickups2.hasNext()){
		Pickup* p = it_pickups2.next();
		if (!p->isPendingRemoval() && p->m_bounds->collides(m_player->m_bounds)) {
			if (p->m_type == PU_COIN) {
				p->collected(m_player);
			}

			if (p->m_type == PU_LADDER && p->m_introTimer == 0.0f && m_levelChangeTimer == 0.0f) {
				if (m_level + 1 >= m_maxLevels) {
					// WIN!
                    DefaultGame* dg = DefaultGame::getInstance();
					dg->enterState(dg->stateSummary);
				} else {
					m_rotationEffect.happeningTimer = 0.01f;
					m_rotationEffect.happeningDuration = 2.0f;

					m_levelChangeTimer = 0.01f;
					m_levelChangeDuration = 2.0f;
					m_levelChanged = false;

                       DefaultGame* dg = DefaultGame::getInstance();
					dg->m_sfxLadder->play();

				}
			}
		}
	}

}
Enemy* InGameState::spawnEnemy(EnemyType type) {
	if (type == ET_MINI) {
		return spawnMiniEnemy();
	}
	else if (type == ET_SMALL) {
		return spawnSmallEnemy();
	}
	else if (type == ET_MED) {
		return spawnMediumEnemy();
	}
	else if (type == ET_LARGE) {
		return spawnLargeEnemy();
	}
	else if (type == ET_FLYING) {
		return spawnFlyingEnemy();
	}
	else if (type == ET_SPITTING) {
		return spawnSpittingEnemy();
	}
	ARK2D::getLog()->e("spawning enemy not set up");
	return NULL;
}
Enemy* InGameState::spawnSmallEnemy() {
	Enemy* e = m_enemies->get();
	e->reset();
	e->start(ET_SMALL);
	return e;
}

Enemy* InGameState::spawnMiniEnemy() {
	Enemy* e = m_enemies->get();
	e->reset();
	e->start(ET_MINI);
	return e;
}
Enemy* InGameState::spawnMediumEnemy() {
	Enemy* e = m_enemies->get();
	e->reset();
	e->start(ET_MED);
	return e;
}
Enemy* InGameState::spawnLargeEnemy() {
	Enemy* e = m_enemies->get();
	e->reset();
	e->start(ET_LARGE);
	return e;
}
Enemy* InGameState::spawnFlyingEnemy() {
	Enemy* e = m_enemies->get();
	e->reset();
	e->start(ET_FLYING);
	return e;
}
Enemy* InGameState::spawnSpittingEnemy() {
	Enemy* e = m_enemies->get();
	e->reset();
	e->start(ET_SPITTING);
	return e;
}

void InGameState::spawnPowerup() {
	GameContainer* container = ARK2D::getContainer();
	Powerup* p = m_powerups->get();
	p->reset();
    p->startRandom();
    p->m_bounds->setLocationByCenter(MathUtil::randBetweenf(20.0f, container->getWidth() - 20), MathUtil::randBetweenf(20, container->getHeight() * 0.33f));

	//p->
}
void InGameState::spawnBlood(float x, float y, float radius, int num) {
	spawnBlood(x, y, radius, num, 0, 360);
}
void InGameState::spawnBlood(float x, float y, float radius, int num, float minAngle, float maxAngle) {
	MyParticle* part = NULL;

	for(int i = 0; i < num; i++) {
		float splashStrength = MathUtil::randBetweenf(30.0f, 60.0f);
		float tx = x;
		float ty = y;
		MathUtil::moveAngle<float>(tx, ty, MathUtil::randBetweenf(0,360), MathUtil::randBetweenf(0.0f, radius));

		Image* frame = MyParticleSprites::s_bloodSelection->at(MathUtil::randBetween(0, MyParticleSprites::s_bloodSelection->size()));
		part = m_particles->get();
		part->reset();
		part->m_animation->reset();
		part->m_animation->addFrame(frame);
		part->m_type = MyParticle::TYPE_BLOOD;
		part->m_color = Color::red.pointer();
		part->m_bounds->setLocationByCenter(tx, ty);
		MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(minAngle,maxAngle), splashStrength);
	}
}
void InGameState::spawnSmoke(float x, float y, float radius, int num) {
	spawnSmoke(x, y, radius, num, Color::white.pointer(), -95, -85);
}
void InGameState::spawnSmoke(float x, float y, float radius, int num, Color* color) {
	spawnSmoke(x, y, radius, num, color, -95, -85);
}
void InGameState::spawnSmoke(float x, float y, float radius, int num, Color* color, float minAngle, float maxAngle) {
	MyParticle* part = NULL;

	for(int i = 0; i < num; i++) {
		float splashStrength = MathUtil::randBetweenf(30.0f, 60.0f);
		float tx = x;
		float ty = y;
		MathUtil::moveAngle<float>(tx, ty, MathUtil::randBetweenf(0,360), MathUtil::randBetweenf(0.0f, radius));

		Image* frame = MyParticleSprites::s_bloodSelection->at(MathUtil::randBetween(0, MyParticleSprites::s_bloodSelection->size()));
		part = m_particles->get();
		part->reset();
		part->m_animation->reset();
		part->m_animation->addFrame(frame);
		part->m_type = MyParticle::TYPE_SMOKE;
		part->m_color = color;
		part->m_bounds->setLocationByCenter(tx, ty);
		part->m_lifetime = true;
		part->m_useGravity = false;
		part->m_lifeAlphaEnd = 1.0f;
		part->m_lifeScaleEnd = 0.0f;
		MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(minAngle,maxAngle), splashStrength);
	}
}

void InGameState::smartBomb() {
	m_flashTimer = 0.01f;

	PoolIterator<Enemy*> it_enemies  = m_enemies->newiteratorref();
	while (it_enemies.hasNext()) {
		Enemy* e = it_enemies.next();
		if (!e->isPendingRemoval() && e->killableBySmartBomb()) {
			e->die();
		}
	}

	PoolIterator<Projectile*> it_projectiles = m_projectiles->newiteratorref();
	while( it_projectiles.hasNext()) {
		Projectile* pr = it_projectiles.next();
		if (!pr->isPendingRemoval()) {
			if (pr->m_type == PT_FLAME) {
				pr->setPendingRemoval(true);
			}
		}
	}
}

bool InGameState::sortSceneZ(Object* first, Object* second) {
	return first->m_bounds->getMaxY() < second->m_bounds->getMaxY();
}

void InGameState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {

	DefaultGame* dg = DefaultGame::getInstance();
	//r->setDrawColor("#4e1353", 1.0f);
	r->setDrawColor("#a60e49", 1.0f);
	r->fillRect(0,0,container->getWidth(), container->getHeight());

	r->pushMatrix();
	r->translate(container->getWidth()*0.5f, container->getHeight()*0.5f);
	m_rotationEffect.alpha = 1.0f;
	if (m_rotationEffect.happeningTimer > 0.0f) {


		float rot = Easing::easebetween(Easing::QUADRATIC_IN_OUT, m_rotationEffect.happeningTimer, 0.0f, 360.0f, m_rotationEffect.happeningDuration);
		r->rotate(rot);

		float scaleAmount = 2.0f;
		float sc = Easing::easebetween(Easing::QUADRATIC_IN_OUT, m_rotationEffect.happeningTimer, 1.0f, scaleAmount, m_rotationEffect.happeningDuration/2);
		if (m_rotationEffect.happeningTimer >= m_rotationEffect.happeningDuration*0.5) {
			sc = Easing::easebetween(Easing::QUADRATIC_IN_OUT, m_rotationEffect.happeningTimer - (m_rotationEffect.happeningDuration*0.5), scaleAmount, 1.0f, m_rotationEffect.happeningDuration/2);
		}
		r->scale(sc, sc);


		m_rotationEffect.alpha = Easing::easebetween(Easing::QUADRATIC_OUT, m_rotationEffect.happeningTimer, 1.0f, 0.0f, m_rotationEffect.happeningDuration/2);
		if (m_rotationEffect.happeningTimer >= m_rotationEffect.happeningDuration*0.5) {
			m_rotationEffect.alpha = Easing::easebetween(Easing::QUADRATIC_IN, m_rotationEffect.happeningTimer - (m_rotationEffect.happeningDuration*0.5), 0.0f, 1.0f, m_rotationEffect.happeningDuration/2);
		}

		if (m_rotationEffect.happeningTimer >= m_rotationEffect.happeningDuration*0.5) {
			float newTeleportX = Easing::easebetween(Easing::QUADRATIC_IN_OUT, m_rotationEffect.happeningTimer - (m_rotationEffect.happeningDuration*0.5), m_rotationEffect.translatePointStartX, m_rotationEffect.translatePointX, m_rotationEffect.happeningDuration/2);
			float newTeleportY = Easing::easebetween(Easing::QUADRATIC_IN_OUT, m_rotationEffect.happeningTimer - (m_rotationEffect.happeningDuration*0.5), m_rotationEffect.translatePointStartY, m_rotationEffect.translatePointY, m_rotationEffect.happeningDuration/2);
//			m_teleportSprite->position.set(newTeleportX, newTeleportY, 0);
		}
	}
	r->translate(container->getWidth()*-0.5f, container->getHeight()*-0.5f);

	r->pushMatrix();
	r->translate(m_cameraShake->getXOffset(), m_cameraShake->getYOffset());

	r->setDrawColor(Color::white);
	arenaBGSideLeft->draw((int)arenaLeftX - 22, 0);
	arenaBGSideRight->draw((int)arenaRightX, 0);
	arenaBGCentrePiece->draw((int)arenaLeftX, 0, arenaRightX - arenaLeftX, 140);
	arenaBGSideEdge->draw((int)arenaLeftX - 22 - 100, 0, 101, 140);
	arenaBGSideEdge->draw((int)arenaRightX + 22, 0, 101, 140);


	// draw shadows first.
	vector<ShadowObject> objectsWithShadows;
	objectsWithShadows.push_back( ShadowObject(Player::s_shadow, m_player->m_bounds->getCenterX(),m_player->m_bounds->getMaxY()) );
	m_projectiles->each([&objectsWithShadows](Projectile* pr) -> void {
		objectsWithShadows.push_back( ShadowObject(Player::s_shadow, pr->m_bounds->getCenterX(), pr->m_bounds->getMaxY(), pr->getScale()) );
	});
	m_enemies->each([&objectsWithShadows](Enemy* pr) -> void {
		objectsWithShadows.push_back( ShadowObject(Player::s_shadow, pr->m_bounds->getCenterX(), pr->m_bounds->getMaxY(), pr->getShadowScale()) );
	});
	m_pickups->each([&objectsWithShadows](Pickup* pr) -> void {
		objectsWithShadows.push_back( ShadowObject(Player::s_shadow, pr->m_bounds->getCenterX(), pr->m_bounds->getMaxY(), pr->getScale()) );
	});

	r->setDrawColor(Color::black);
	for(unsigned int i = 0; i < objectsWithShadows.size(); i++) {
		ShadowObject* so = &objectsWithShadows[i];
		so->img->drawCenteredScaled(so->x, so->y - 1, so->sc, so->sc);
	}

	// draw spawn points
	r->setDrawColor(Color::white);
	for(unsigned int i = 0; i < m_spawns.size(); i++) {
		SpawnPoint* sp = &m_spawns.at(i);
		float sc = sp->getScale();
		swirlSpawnImage->setRotation( sp->currentTime * 90.0f );
        swirlSpawnImage->drawCenteredScaled(sp->x, sp->y, sc, sc);
	}

	r->setDrawColor(Color::white);

	vector<Object*> inOrder;
	inOrder.push_back(m_player);
	m_projectiles->each([&inOrder](Projectile* pr) -> void { inOrder.push_back( pr ); });
	m_enemies->each([&inOrder](Enemy* pr) -> void { inOrder.push_back( pr ); });
	m_particles->each([&inOrder](MyParticle* pr) -> void { inOrder.push_back( pr ); });
	m_pickups->each([&inOrder](Pickup* pr) -> void { inOrder.push_back( pr ); });
	std::sort(inOrder.begin(), inOrder.end(), sortSceneZ);

	for(unsigned int i = 0; i < inOrder.size(); i++) {
		r->setDrawColor(Color::white);
		inOrder[i]->render(container, r);

		if (DefaultGame::s_debug) {
			r->setDrawColor(Color::red);
			r->drawRect(inOrder[i]->m_bounds->getMinX(), inOrder[i]->m_bounds->getMinY(), inOrder[i]->m_bounds->getWidth(), inOrder[i]->m_bounds->getHeight());
		}
	}

	if (m_introMessageTimer > 0.0f && !m_firstPlay) {
		float ta = 1.0f;
		//float textPercentage = 1.0f;
		if (m_introMessageTimer < 0.5f) {
			ta = Easing::easebetween(Easing::QUADRATIC_OUT, m_introMessageTimer, 0.0f, 1.0f, 0.5f);
		} else if (m_introMessageTimer > m_introMessageDuration - 0.5f) {
			ta = Easing::easebetween(Easing::QUADRATIC_OUT, m_introMessageTimer - (m_introMessageDuration - 0.5f), 1.0f, 0.0f, 0.5f);
		}
		r->setDrawColor(Color::white, ta);

		// if (m_introMessageTimer < m_introMessageDuration * 0.7f) {
		// 	textPercentage = Easing::easebetween(Easing::LINEAR, m_introMessageTimer, 0.0f, 1.0f, m_introMessageDuration * 0.7f);
		// }

		string str = m_introMessages[m_introMessageIndex];
		str = str.substr(0, str.length() * ta);
		float strw = dg->font->getStringWidth(str) * 0.75f;
		MyTextBubble::draw(str, m_player->m_bounds->getCenterX(), m_player->m_bounds->getMinY() - 21, strw + 5);

		r->setDrawColor(Color::white);
	}




	// STENCIL BUFFER
	vector<LightObject> objectsWithLights;
	objectsWithLights.push_back( LightObject(m_player->m_bounds->getCenterX(), m_player->m_bounds->getCenterY(), m_player->m_lightingRadius * m_player->getLightingRadiusModifier()) );
	m_projectiles->each([&objectsWithLights](Projectile* pr) -> void {

		if (pr->m_type == PT_FLAME || pr->m_type == PT_FLAME_EMBERS) {
			objectsWithLights.push_back( LightObject(pr->m_bounds->getCenterX(), pr->m_bounds->getCenterY(), pr->m_lightingRadius * pr->getLightingRadiusModifier()) );
		}

	});
	m_pickups->each([&objectsWithLights](Pickup* pr) -> void {
		objectsWithLights.push_back( LightObject(pr->m_bounds->getCenterX(), pr->m_bounds->getCenterY(), pr->m_lightingRadius * pr->getLightingRadiusModifier()) );
	});


	auto drawStencilForAlphaAndMultiplier = [r,container,&objectsWithLights](float multiplier, float alpha, float flickerChance, float flickerMultiplier) -> void {
		r->setDrawColor(Color::white);
		r->enableStencil();
		r->startStencil();

		float flickerMultiplierUse = (MathUtil::randBetweenf(0.0f, 1.0f) < flickerChance)?flickerMultiplier:1.0f;

		r->fillRect(0,0,container->getWidth(), container->getHeight());
		r->inverseStencil();
		for(unsigned int i = 0; i < objectsWithLights.size(); i++) {
			LightObject* lo = &objectsWithLights[i];
			r->fillCircle(lo->x, lo->y, lo->radius*multiplier*flickerMultiplierUse, 30);
		}

		r->stopStencil();

		r->setDrawColor(Color::black, alpha);
		r->fillRect(0,0,container->getWidth(), container->getHeight());

		r->disableStencil();
	};

	// stop shake
	r->popMatrix();

	// stop rotation
	r->popMatrix();

	float useDarknessAlpha = m_darknessAlpha;
	if (dg->stateUpgrades->hasNightVision()) {
		useDarknessAlpha = m_darknessAlpha * 0.5;;
	}
	float sizeMult = std::max(1.0f, (1.0f - useDarknessAlpha) * 5.0f);

	drawStencilForAlphaAndMultiplier(1.0f*sizeMult, 0.5f * useDarknessAlpha, 0.0f, 1.0f);
	drawStencilForAlphaAndMultiplier(1.5f*sizeMult, 1.0f * useDarknessAlpha, 0.0f, 1.0f);


	if (m_levelChangeTimer > 0.0f) {
		float rectAlpha = 0.0f;
		if (m_levelChangeTimer <= m_levelChangeDuration * 0.5f) {
			rectAlpha = Easing::easebetween(Easing::QUADRATIC_OUT, m_levelChangeTimer, 0.0f, 1.0f, m_levelChangeDuration * 0.5f);
		} else if (m_levelChangeTimer > m_levelChangeDuration * 0.5f) {
			rectAlpha = Easing::easebetween(Easing::QUADRATIC_IN, m_levelChangeTimer - (m_levelChangeDuration * 0.5f), 1.0f, 0.0f, m_levelChangeDuration * 0.5f);
		}
		r->setDrawColorf(0.0f, 0.0f, 0.0f, rectAlpha);
		r->fillRect(0, 0, container->getWidth(), container->getHeight());
	}

	if (m_suicideTimer > 0.0f) {
		r->setDrawColor(Color::white);
		r->drawString("NO FIRE ~ NO CO2 ~ GIVE UP?", container->getWidth()*0.5, 10, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
	}

	if (m_startWaveTimer > 0.0f) {
		string floorStr = StringUtil::append("FLOOR -", m_level+1);
		if (m_level == TUTORIAL_LEVEL) {
			floorStr = "TUTORIAL";
		} else if (m_level == m_maxLevels-1) {
			floorStr = "FINAL FLOOR!";
		}//m_uiTutorialWASD

		r->setDrawColor(Color::white);
		r->drawString(floorStr, container->getWidth()*0.5, 10, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.75f);
	}


	r->setDrawColor(Color::white);
	uiBottomBar->draw(0, container->getHeight() - uiBottomBar->getHeight(), container->getWidth(), uiBottomBar->getHeight());

	Projectile::s_flameAnim->frames[0]->setRotation(0);
	Projectile::s_flameAnim->frames[0]->drawAligned(10, 156, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM);
	Projectile::s_gasIcon->drawAligned(container->getWidth() - 10, 151, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER);
	Pickup::s_animCoin->frames[0]->drawAligned((container->getWidth()*0.5f) - 1, 150, Renderer::ALIGN_RIGHT, Renderer::ALIGN_CENTER);

	dg->font->drawString(StringUtil::append("", dg->m_coins), (container->getWidth()*0.5f)+1, 151, Renderer::ALIGN_LEFT, Renderer::ALIGN_CENTER, 0.0f, 0.75f);

	if (m_player->m_fireMax == -1) {
		// ...
	}
	else {
		for(int i = 0; i < m_player->m_fireMax; i++) {
			r->setDrawColor(Color::white);
			uiBottomBarPieceEmpty->draw(20 + (4*i), 145);
			if (m_player->m_fireAmmo > i) {
				r->setDrawColor("#feff6e", 1.0f);
				uiBottomBarPieceFull->draw(20 + (4*i), 145);
			}
		}
	}

	if (m_player->m_waterMax == -1) {
		// ...
	}
	else {
		for(int i = 0; i < m_player->m_waterMax; i++) {
			r->setDrawColor(Color::white);
			uiBottomBarPieceEmpty->draw(container->getWidth() - 20 - 4 - (4*i), 145);
			if (m_player->m_waterAmmo > i) {
				r->setDrawColor("#ffffff", 1.0f);
				uiBottomBarPieceFull->draw(container->getWidth() - 20 - 4 - (4*i), 145);
			}
		}
	}

	// tutorial stuff
	if (m_introMessageTimer > 0.0f && m_level == 0 && m_firstPlay) {
		float wasdX = container->getWidth() * 0.3f;
		float fireX = container->getWidth() * 0.65f;
		float gasX = container->getWidth() * 0.75f;

		float wasdY = container->getHeight() * 0.5f;
		float fireY = container->getHeight() * 0.5f;

		if (m_introMessageTimer < m_introMessageDuration * 0.2f) {
			wasdY = fireY = Easing::easebetween(Easing::QUADRATIC_OUT, m_introMessageTimer, container->getHeight() * 1.25f, container->getHeight() * 0.5f, m_introMessageDuration*0.2f);
		} else if (m_introMessageTimer > m_introMessageDuration * 0.8f) {
			wasdY = fireY = Easing::easebetween(Easing::QUADRATIC_OUT, m_introMessageTimer - (m_introMessageDuration*0.8f), container->getHeight() * 0.5f, container->getHeight() * -0.25f, m_introMessageDuration*0.2f);
		}

		m_uiTutorialWASD->drawCentered(wasdX, wasdY);
		m_uiTutorialMouseAnimation->drawCentered(fireX, fireY);
		m_uiTutorialMouseRightAnimation->drawCentered(gasX, fireY);

		dg->font->drawString("MOVE", wasdX - 1, wasdY + 25, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.75f);
		dg->font->drawString("FIRE", fireX - 1, fireY + 25, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.75f);
		dg->font->drawString("CO2", gasX - 1, fireY + 25, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.75f);
	}

	if (m_paused) {
		r->setDrawColor("#000000",0.5f);
		r->fillRect(0,0,container->getWidth(), container->getHeight());
		r->setDrawColor(Color::white);
		dg->font->drawString("PAUSED", (container->getWidth()*0.5f) - 1, (container->getHeight()*0.5f) + 1, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);
	}

	/*r->pushMatrix();
	r->translate(m_cameraShake->getXOffset(), m_cameraShake->getYOffset());

	DefaultGame* dg = DefaultGame::getInstance();
	r->setDrawColor(Color::white);
	//r->drawString("In Game State", 20, 20);

	dg->renderBackground();


	m_player->render(container, r);
	m_projectiles->renderAll(container, r);
	m_enemies->renderAll(container, r);
	m_pickups->renderAll(container, r);
	m_powerups->renderAll(container, r);


	// STOP SHAKING IT
	r->popMatrix();



	// draw UI here
	float uiScale = 1.0f;
	if (m_introTimer > 0.0f) {
		uiScale = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}
	if (m_deadTimer > 0.0f) {
		uiScale = Easing::easebetween(Easing::QUADRATIC_OUT, m_deadTimer, 1.0f, 0.0f, m_deadDuration);
	}
	r->setDrawColorf(1.0f,1.0f,1.0f,uiScale);
	dg->font->drawString(Cast::toString<unsigned int>(m_score), (container->getWidth()*0.5f)-2, 20*uiScale, Renderer::ALIGN_CENTER,Renderer::ALIGN_CENTER, 0.0f, 2.0f * uiScale);

	if (m_currentPowerup != NULL) {
		m_currentPowerupContainer->drawCenteredScaled(20, 20*uiScale, uiScale, uiScale);
		m_currentPowerup->drawCenteredScaled(20, 20 * uiScale, uiScale, uiScale);
	}

	if (m_flashTimer > 0.0f) {
		float alpha = Easing::easebetween(Easing::QUADRATIC_OUT, m_flashTimer, 1.0f, 0.0f, m_flashDuration);
		r->setDrawColorf(1.0f, 1.0f, 1.0f, alpha);
		r->fillRect(0,0,container->getWidth(), container->getHeight());
	}*/

	//r->drawString("Game", 10, 10);
}

InGameState::~InGameState() {

}

