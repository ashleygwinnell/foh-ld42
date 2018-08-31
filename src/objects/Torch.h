/*
 * Torch.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef TORCH_H_
#define TORCH_H_

#include "../ARK.h"
#include "Object.h"

class Torch : public Object {
	public:

		static Image* s_torchStand;

		float m_introTimer;
		float m_introDuration;

		bool m_lit;

		Torch();
		float getScale();
		void reset();
		//void start(ProjectileType type);
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~Torch();
};

#endif /* TORCH_H_ */
