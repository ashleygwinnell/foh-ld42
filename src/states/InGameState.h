
#ifndef ARK2D_DEFAULTGAME_INGAMESTATE_H_
#define ARK2D_DEFAULTGAME_INGAMESTATE_H_

#include <ARK2D/Core/State/GameState.h>

class DefaultGame;

class Object;
class BlankObject;
class Player;
class Enemy;
class Pickup;
class Powerup;
class Torch;
class Explosion;
class Projectile;
class MyParticle;
enum EnemyType : unsigned int ;
enum PowerupType : signed int ;
enum PickupType : signed int ;
enum ProjectileType : unsigned int ;


struct ShadowObject {
	Image* img;
	float x;
	float y;
	float sc;
	ShadowObject(Image* i, float xx, float yy) { img = i; x = xx; y = yy; sc = 1; }
	ShadowObject(Image* i, float xx, float yy, float scale) { img = i; x = xx; y = yy; sc = scale; }
};
struct LightObject {
	float x;
	float y;
	float radius;
	LightObject(float xx, float yy, float rr) { x = xx; y = yy; radius = rr; }
};

class SpawnEnemyEvent {
	public:
		EnemyType enemyType;
		float delay;
};

class SpawnPoint {
	public:
		static const unsigned int TYPE_POINT = 0;
		static const unsigned int TYPE_AREA_RECT = 1;
        static const unsigned int TYPE_AREA_CIRC = 2;
        static const unsigned int TYPE_LINE = 3;
		float x;
		float y;
		float MoveWithArenaMult;
		float currentTime;
		float startTime;
		float finishedAtTime;
		bool started;
		bool finished;
		vector<SpawnEnemyEvent> m_spawnEnemies;

		SpawnPoint();
		float getScale();
};

struct RotationEffect {
	float happeningTimer = 0;
	float happeningDuration = 0;
	bool halfWayEvent = 0;

	float translatePointStartX = 0;
	float translatePointStartY = 0;
	float translatePointX = 0;
	float translatePointY = 0;
	float alpha = 0;
};

class InGameState : public GameState {
	public:

		Pool<Player* >* m_players;
		Player* m_player;
		Pool<Projectile* >* m_projectiles;
		Pool<Enemy* >* m_enemies;
		Pool<Pickup* >* m_pickups;
		Pool<Powerup* >* m_powerups;
		Pool<MyParticle* >* m_particles;
		Pool<Torch* >* m_torches;

		//m_cameraShake->setSpeedMultiplier(1, 0.5f);


		RotationEffect m_rotationEffect;



		// Layers:
		// 	Enemy die.
		// 	Player die.
		//  Player shoot.
		// 	Player shoot (gas)
		// 	Enemy spawn.
		CameraShake* m_cameraShake;

		float m_introTimer;
		float m_introDuration;

		float m_introMessageTimer;
		float m_introMessageDuration;
		signed int m_introMessageIndex;
		vector<string> m_introMessages;

		int m_score;
		//int m_coins;
		int m_level;
		float m_maxLevels;
		static const signed int TUTORIAL_LEVEL = -1;

		bool m_firstPlay;
		unsigned int m_tutorialState;
		static const unsigned int TUTORIAL_STATE_START = 0;
		static const unsigned int TUTORIAL_STATE_MOVE = 1;
		static const unsigned int TUTORIAL_STATE_AIM = 2;
		static const unsigned int TUTORIAL_STATE_FIRE = 3;
		static const unsigned int TUTORIAL_STATE_FIRECO2 = 4;
		static const unsigned int TUTORIAL_STATE_TORCHES = 5;
		static const unsigned int TUTORIAL_STATE_LADDER = 6;


		bool m_win;
		bool m_died;
		float m_deadTimer;
		float m_deadDuration;

		float m_bombExplodedFilterTimer;
		float m_bombExplodedFilterDuration;

		float m_flashTimer;
		float m_flashDuration;


		Image* m_currentPowerupContainer;
		Image* m_currentPowerup;
		PowerupType m_currentPowerupType;

		Image* m_uiTutorialWASD;
		Image* m_uiTutorialMouseLeft;
		Image* m_uiTutorialMouseRight;
		Image* m_uiTutorialMouseOff;
		Animation* m_uiTutorialMouseAnimation;
		Animation* m_uiTutorialMouseRightAnimation;

		Image* swirlSpawnImage;
		Image* arenaBGSideLeft;
		Image* arenaBGSideRight;
		Image* arenaBGSideEdge;
		Image* arenaBGCentrePiece;
		Image* uiBottomBar;
		Image* uiBottomBarPieceEmpty;
		Image* uiBottomBarPieceFull;
		float arenaLeftX;
		float arenaRightX;
		float arenaTopY;
		float arenaBottomY;
		float arenaClosingSpeed = 1.0f;;

		float m_darknessAlpha;


		Color* colorRed;
		Color* colorOrange;
		Color* colorYellow;

		vector<SpawnPoint> m_spawns;

		float m_levelChangeTimer;
		float m_levelChangeDuration;
		bool m_levelChanged;

		float m_suicideTimer;
		float m_suicideDuration;

		float m_startWaveTimer;
		float m_startWaveDuration;

		bool m_paused;


		InGameState();
		void enter(GameContainer* container, StateBasedGame* game, GameState* from);
		void leave(GameContainer* container, StateBasedGame* game, GameState* to);

		void startWave(int level);
		int countEnemiesOfType(EnemyType et);
		void smartBomb();

		void onScoreChange();

		Enemy* spawnEnemy(EnemyType et);
		Enemy* spawnSmallEnemy();
		Enemy* spawnMiniEnemy();
		Enemy* spawnMediumEnemy();
		Enemy* spawnLargeEnemy();
		Enemy* spawnFlyingEnemy();
		Enemy* spawnSpittingEnemy();

		Projectile* startFireAt(float x, float y);

		int countFinishedSpawnPoints();
		int countPickupsOfType(PickupType pt);
		int countProjectilesOfType(ProjectileType pt);

		void spawnPowerup();
		void spawnSmoke(float x, float y, float radius, int num);
		void spawnSmoke(float x, float y, float radius, int num, Color* color);
		void spawnSmoke(float x, float y, float radius, int num, Color* color, float minAngle, float maxAngle);
		void spawnBlood(float x, float y, float radius, int num);
		void spawnBlood(float x, float y, float radius, int num, float minAngle, float maxAngle);
		unsigned int id();
		void reset();
		void start();

		static bool sortSceneZ(Object* first, Object* second);

		void init(GameContainer* container, StateBasedGame* game);
		void update(GameContainer* container, StateBasedGame* game, GameTimer* timer);
		void render(GameContainer* container, StateBasedGame* game, Renderer* r);

		void doCollisions();

		virtual ~InGameState();
};

#endif /* ARK2D_DEFAULTGAME_INGAMESTATE_H_ */
