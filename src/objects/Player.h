/*
 * Player.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "../ARK.h"
#include "Object.h"
class EightWayMovementComponent;
class Projectile;
class Pickup;
class Powerup;
class Enemy;


class Player : public Object {
	public:

		static Image* s_playerGibHead;
		static Image* s_playerGibLeg;

		static Image* s_shadow;

		static Image* s_player;
		static Image* s_playerWeapon;
		static Image* s_playerHP2;
		static Image* s_playerHP3;

		EightWayMovementComponent* m_movement;

		Animation* m_animationIdle;
		Animation* m_animationRun;

		Timer* m_fireTimer;
		Timer* m_fireWaterTimer;

		float m_timer;

		int m_fireAmmo;
		int m_fireMax;
		int m_waterAmmo;
		int m_waterMax;
		int m_gasAmmo;
		int m_gasMax;

		bool m_burnProtection;

		bool m_moving;
		bool m_grounded;
		bool m_facingLeft;
		float m_rotation;

		float m_introTimer;
		float m_introDuration;

		float m_lastFlameAtX;
		float m_lastFlameAtY;
		float m_flameEachDist;
		float m_weaponAngle;

		unsigned int m_lives;
		float m_lostLifeTimer;
		float m_lostLifeDuration;

		int m_dieReason;
		static const unsigned int DIE_NULL = 0;
		static const unsigned int DIE_FIRE = 1;
		static const unsigned int DIE_ENEMY = 3;

		Player();
		Vector2<float> getGunEndPos();
		void reset();
		void hitByProjectile(Projectile* pr);
		void hitByPickup(Pickup* pu);
		void hitByEnemy(Enemy* e);
		void die();
		void onLoseLife();
		Vector2<float> getHandPosition();
		bool isDamagable();
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~Player();
};

#endif /* PLAYER_H_ */
