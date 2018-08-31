/*
 * BlankObject.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include "../ARK.h"
#include "Object.h"

enum ProjectileType : unsigned int {
	PT_FLAME,
	PT_FLAME_EMBERS,
	PT_WATER,
	PT_CO2
};

class Projectile : public Object {
	public:

		static AnimationFrameList* s_flameAnim;
		static AnimationFrameList* s_waterAnim;
		static AnimationFrameList* s_gasAnim;
		static Image* s_embersImage;
		static Image* s_gasIcon;

		Animation* m_animation;

		float m_introTimer; // squash & stretch them?
		float m_introDuration;

		ProjectileType m_type;

		Vector2<float> m_from;
		Vector2<float> m_to;
		float m_speed;
		float m_dist;
		float m_aliveTimer;

		bool m_isFromPlayer;
		bool m_grounded;
		bool m_fireUsed;

		float m_hasLifetime;
		float m_lifetimeTimer;
		float m_lifetimeDuration;

		Projectile();
		float getScale();
		void reset();
		void start(ProjectileType type);
		bool canDamageEnemy();
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~Projectile();
};

#endif /* PICKUP_H_ */
