/*
 * Pickup.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "Projectile.h"
#include "Player.h"
#include "../DefaultGame.h"
#include <ARK2D/Core/Tween/Timer.h>

AnimationFrameList* Projectile::s_flameAnim = NULL;
AnimationFrameList* Projectile::s_waterAnim = NULL;
AnimationFrameList* Projectile::s_gasAnim = NULL;
Image* Projectile::s_embersImage = NULL;
Image* Projectile::s_gasIcon = NULL;

Projectile::Projectile():
	Object()
	{

	DefaultGame* dg = DefaultGame::getInstance();

	if (s_flameAnim == NULL) {
		s_flameAnim = AnimationFrameList::create("sprites/fire/fire-*.png", 1, 10, dg->spritesheet, dg->desc);
		s_waterAnim = AnimationFrameList::create("sprites/fire/water-*.png", 1, 8, dg->spritesheet, dg->desc);
		s_gasAnim = AnimationFrameList::create("sprites/fire/gas-*.png", 1, 4, dg->spritesheet, dg->desc);

		s_embersImage = SpriteSheetStore::getImage("sprites/fire/embers.png");
		s_gasIcon = SpriteSheetStore::getImage("sprites/fire/gas-icon.png");
	}

	m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, 10, 10);
	m_bounds->setLocationByCenter(0, 0);

	m_velocity->set(0, 0);
	m_velocityMax->set(300, 300);
	m_acceleration->set(0, 0);

	m_animation = new Animation();
	m_animation->setFrameTime(0.25f);;

	reset();

}
// bool Projectile::isInGround() {
// 	return m_travelTimer >= m_travelDuration;
// }

void Projectile::reset() {
	Object::reset();

	m_velocity->set(0.0f, 0.0f);

	m_introTimer = 0.01f;
	m_introDuration = 0.175f;

	m_isFromPlayer = false;
	m_grounded = false;

	m_dist = 0;
	m_aliveTimer = 0.01f;

	m_fireUsed = false;

	m_hasLifetime = true;
	m_lifetimeTimer = 0.0f;
	m_lifetimeDuration = 30.0f;
}

void Projectile::start(ProjectileType type) {
	m_type = type;

	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = DefaultGame::getInstance()->stateInGame;

	if (type == PT_FLAME) {
		m_lightingRadius = 20.0f;
		m_isFromPlayer = true;

		m_animation->setFrames(s_flameAnim);
		m_animation->setFrameTime(0.05f);
		m_animation->setCurrentFrameIndex(MathUtil::randBetween(0, 10));

		m_bounds->asRectangle()->setSize(6, 6);
	}
	else if (type == PT_FLAME_EMBERS) {
		m_lightingRadius = 5.0f;
		m_animation->clear();
		m_animation->addFrame(s_embersImage);
		m_grounded = true;
		m_z = 0;

		m_hasLifetime = false;
	}
	else if (type == PT_WATER) {
		m_isFromPlayer = true;

		m_animation->setFrames(s_waterAnim);
		m_animation->setFrameTime(0.05f);
		m_animation->setCurrentFrameIndex(MathUtil::randBetween(0, 8));

		m_bounds->asRectangle()->setSize(6, 6);
	}
	else if (type == PT_CO2) {
		m_isFromPlayer = true;

		m_animation->setFrames(s_gasAnim);
		m_animation->setFrameTime(0.05f);
		m_animation->setCurrentFrameIndex(MathUtil::randBetween(0, 4));

		m_bounds->asRectangle()->setSize(6, 6);

	}
}
bool Projectile::canDamageEnemy() {
	return m_isFromPlayer || (!m_isFromPlayer && m_introTimer == 0.0);
}
void Projectile::update(GameContainer* container, GameTimer* timer) {
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

	int prevFrame = m_animation->getCurrentFrameIndex();
	m_animation->update(timer);

	if (m_type == PT_FLAME || m_type == PT_WATER || PT_CO2) {

		float duration = m_dist / m_speed;
		if (m_aliveTimer < duration) {
			m_aliveTimer += timer->getDelta();
			// m_dist
			// m_from

			float newX = m_from.getX();
			float newY = m_from.getY();
			float angle = MathUtil::anglef(m_from.getX(), m_from.getY(), m_to.getX(), m_to.getY());
			float thisDist = Easing::easebetween(Easing::QUADRATIC_OUT, m_aliveTimer, 0.0f, m_dist, duration);
			MathUtil::moveAngle<float>(newX, newY, angle, thisDist);
			m_bounds->setLocationByCenter(newX, newY);
		} else {
			m_bounds->setLocationByCenter(m_to.getX(), m_to.getY());
			//m_aliveTimer = 0.0f;
			//m_grounded = true;
		}

		if (!m_grounded) {
			m_zSpeed += m_zAcceleration * timer->getDelta();
			m_z -= m_zSpeed * timer->getDelta();
			if (m_z < 0) {
				m_z = 0;
				m_zSpeed *= -0.5f;
				if (m_zSpeed < 3) {
					m_z = 0;
					m_zSpeed = 0;
					m_zAcceleration = 0;
					m_grounded = true;
				}
			}
		}
		keepInBounds();


		//Object::move(1.0f);

		// m_travelRotation = MathUtil::anglef(0.0f, 0.0f, m_velocity->getX(), m_velocity->getY()) - 90;

		// if (m_bounds->getMaxY() >= dg->m_floor) {
		// 	m_bounds->setLocationByCenter(m_bounds->getCenterX(), dg->m_floor - (m_bounds->getHeight() * 0.5f));
		// 	m_travelRotation = 0;
		// 	m_velocity->set(0.0f, 0.0f);
		// }

		// if (prevFrame > m_animation->getCurrentFrameIndex()) {
		// 	setPendingRemoval(true);
		// }
	}


	if (m_hasLifetime && m_grounded) {
		m_lifetimeTimer += timer->getDelta();
		if (m_lifetimeTimer >= m_lifetimeDuration) {
			if (m_type == PT_FLAME) {
				reset();
				start(PT_FLAME_EMBERS);
				igs->spawnSmoke( m_bounds->getCenterX(), m_bounds->getCenterY(), m_radius, 4.0f, igs->colorRed );
				igs->spawnSmoke( m_bounds->getCenterX(), m_bounds->getCenterY(), m_radius, 3.0f, igs->colorOrange->pointer() );
				igs->spawnSmoke( m_bounds->getCenterX(), m_bounds->getCenterY(), m_radius, 2.0f, igs->colorYellow->pointer() );
            } else {
                setPendingRemoval(true);
            }
		}
	}


}

void Projectile::collision(Object* other) {
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;
}
float Projectile::getScale() {
	float scWidth = 1.0f;
	float scHeight = 1.0f;
	if (m_introTimer >= 0.01f) {
		//scWidth = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 1.25f, 1.0f, m_introDuration);
		//scHeight = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.75f, 1.0f, m_introDuration);

		scWidth = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
		scHeight = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}
	if (m_hasLifetime && m_grounded) {
		// if (m_lifetimeTimer >= m_lifetimeDuration - 0.5f) {
		// 	scWidth = Easing::easebetween(Easing::QUADRATIC_OUT, m_lifetimeTimer - (m_lifetimeDuration - 0.5f), 1.0f, 0.0f, 0.5);
		// }
	}
	return scWidth;
}

void Projectile::render(GameContainer* container, Renderer* r) {
	//r->setDrawColor(Color::green);
	//r->fillCircle(m_bounds->getCenterX(), m_bounds->getCenterY(), (m_bounds->getWidth()/2.0f), 10);
	r->setDrawColor(Color::white);
	float scWidth = getScale();
	float scHeight = getScale();


	Image* sprite = m_animation->getCurrentFrame();

	float width = sprite->getWidth();
	float height = sprite->getHeight();

    float rot = 0;//m_travelRotation;//MathUtil::anglef(0, 0, m_velocity->getX(), m_velocity->getY());
    if (!m_grounded) {
    	float duration = m_dist / m_speed;
    	float angle = MathUtil::forcePositiveAngle(MathUtil::anglef(m_from.getX(), m_from.getY(), m_to.getX(), m_to.getY()) - 90);
    	float thisAngle = angle;
    	if (angle > 180) {
    		thisAngle = Easing::easebetween(Easing::QUADRATIC_OUT, 8 - m_z, angle, 360, 8);
    	} else {
    		thisAngle = Easing::easebetween(Easing::QUADRATIC_OUT, 8 - m_z, angle, 0, 8);
    	}

    	//float thisAngle = angle;
    	rot = thisAngle;
    }

	sprite->setWidth(width*scWidth);
	sprite->setHeight(height*scHeight);
	sprite->setRotation(rot);
	sprite->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY() - m_z, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM);
	sprite->setWidth(width);
	sprite->setHeight(height);


}

Projectile::~Projectile() {

}
