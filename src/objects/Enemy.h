/*
 * Player.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef ENEMY_H_
#define ENEMY_H_

#include "../ARK.h"
#include "Object.h"
class EightWayMovementComponent;
class Player;

enum EnemyType : unsigned int {
	ET_MINI,
	ET_SMALL,
	ET_MED,
	ET_LARGE,
	ET_FLYING,
	ET_SPITTING
};

class Enemy : public Object {
	public:

		static Image* s_uiPipBig;
		static Image* s_uiPipSmall;

		static Image* s_enemyMiniFrame;
		static Image* s_enemySmallFrame;
		static Image* s_enemyMediumFrame;
		static Image* s_enemyLargeFrame;
		static AnimationFrameList* s_enemyFlyingFrames;
		static Image* s_enemySpittingFrame;
		static Image* s_enemySpittingAttackFrame;

		EnemyType m_type;

		Animation* m_animation;

		bool m_followPlayer;
		float m_followPlayerDistance;
		bool m_followVelocity;
		float m_targetAngleOffset;
		float m_bumpPower;

		bool m_damagable;

		float m_introTimer;
		float m_introDuration;

		int m_hitTimes;
		int m_hitHealth;
		float m_hitTimer;
		float m_hitDuration;

		int m_dropsCoinAmount;
		float m_dropsCoinChance;

		bool m_hopping;
		float m_enemyHopTimer;
		float m_enemyHopDuration;
		float m_enemyHopHeight;

		bool m_squidging;
		float m_squidgingTimer;
		float m_squidgingDuration;

		bool m_dropFire;
		float m_dropFireTimer;
		float m_dropFireDuration;

		bool m_spitFire;
		float m_spitFireTimer;
		float m_spitFireDuration;
		float m_spitFireCooldownTimer;
		float m_spitFireCooldownDuration;

		bool m_creatingChildren;
		vector<EnemyType> m_creatingChildrenPalette;
		float m_creatingChildrenTimer;
		float m_creatingChildrenDuration;


		float m_speedMin;
		float m_speedMax;
		float m_speed;

		Enemy();
		void checkPlayerCollisions(bool firstPass, bool movePlayer);
		void checkEnemyCollisions();

		virtual bool keepInBounds();

		float getScale();
		float getShadowScale();
		bool isDamagable();
		bool canHurtPlayer(Player* p);
		bool canMove();
		bool killableBySmartBomb();
		void reset();
		void start(EnemyType et);
		void hit();
		void smoke();
		void die();
		void killParticles(float startAngle, float power, float powerSpread);
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~Enemy();
};

#endif /* ENEMY_H_ */
