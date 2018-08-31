/*
 * Torch.cpp
 *
 *  Created on: 24 Aug 2018
 *      Author: Ashley
 */

#include "Torch.h"
#include "Player.h"
#include "../DefaultGame.h"
#include <ARK2D/Core/Tween/Timer.h>

Image* Torch::s_torchStand = NULL;

Torch::Torch():
	Object()
	{

	DefaultGame* dg = DefaultGame::getInstance();

	if (s_torchStand == NULL) {
		s_torchStand = SpriteSheetStore::getImage("sprites/fire/gas-icon.png");
	}

	m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, 10, 10);
	m_bounds->setLocationByCenter(0, 0);

	m_velocity->set(0, 0);
	m_velocityMax->set(300, 300);
	m_acceleration->set(0, 0);

	//m_animation = new Animation();
	//m_animation->setFrameTime(0.25f);;

	reset();

}


void Torch::reset() {
	Object::reset();

	m_velocity->set(0.0f, 0.0f);

	m_introTimer = 0.01f;
	m_introDuration = 0.25f;

	m_lit = false;
}

void Torch::update(GameContainer* container, GameTimer* timer) {
	//move(1.0f);

	Object::update(container, timer);

	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = DefaultGame::getInstance()->stateInGame;

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer >= m_introDuration) {
			m_introTimer = 0.0f;
		}
	}
}

void Torch::collision(Object* other) {
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;
}
float Torch::getScale() {
	float scWidth = 1.0f;
	float scHeight = 1.0f;
	if (m_introTimer >= 0.01f) {
		//scWidth = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 1.25f, 1.0f, m_introDuration);
		//scHeight = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.75f, 1.0f, m_introDuration);

		scWidth = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
		scHeight = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}
	//if (m_hasLifetime && m_grounded) {
		// if (m_lifetimeTimer >= m_lifetimeDuration - 0.5f) {
		// 	scWidth = Easing::easebetween(Easing::QUADRATIC_OUT, m_lifetimeTimer - (m_lifetimeDuration - 0.5f), 1.0f, 0.0f, 0.5);
		// }
	//}
	return scWidth;
}

void Torch::render(GameContainer* container, Renderer* r) {
	//r->setDrawColor(Color::green);
	//r->fillCircle(m_bounds->getCenterX(), m_bounds->getCenterY(), (m_bounds->getWidth()/2.0f), 10);
	r->setDrawColor(Color::white);
	float scWidth = getScale();
	float scHeight = getScale();

	Image* sprite = s_torchStand;// m_animation->getCurrentFrame();

	float width = sprite->getWidth();
	float height = sprite->getHeight();

	sprite->setWidth(width*scWidth);
	sprite->setHeight(height*scHeight);
	sprite->setRotation(0);
	sprite->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY(), Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM);
	sprite->setWidth(width);
	sprite->setHeight(height);

	// TODO draw flame

	if (m_lit) {

	}

}

Torch::~Torch() {

}
