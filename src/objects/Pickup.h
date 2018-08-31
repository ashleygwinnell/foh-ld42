/*
 * Arrow.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef PICKUP_H_
#define PICKUP_H_

#include "../ARK.h"
#include "Object.h"

class Player;
enum PickupType : signed int {
	PU_COIN,
	PU_LADDER
};

class Pickup : public Object {
	public:
		static AnimationFrameList* s_animCoin;
		static Image* s_ladder;

		PickupType m_type;
		Animation* m_animation;
		bool m_grounded;
		bool m_tumbling;

		float m_rotation;

		float m_introTimer;
		float m_introDuration;

	public:
		Pickup();
		float getScale();
		virtual void reset();
		void collected(Player* player);
		void explode();
		virtual void start(PickupType type);
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~Pickup();
};

#endif /* PICKUP_H_ */
