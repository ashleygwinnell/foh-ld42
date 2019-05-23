/*
 * Pickup.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "Pickup.h"
#include "MyParticle.h"
#include "Enemy.h"
#include "Player.h"
#include "../DefaultGame.h"

#include "EightWayMovementComponent.h"
#include <ARK2D/Core/+.h>
#include <ARK2D/Audio/Sound.h>
#include <ARK2D/Util/CameraShake.h>

AnimationFrameList* Pickup::s_animCoin = NULL;
Image* Pickup::s_ladder = NULL;

Pickup::Pickup():
	Object()
	{

	DefaultGame* dg = DefaultGame::getInstance();

	if (s_animCoin == NULL) {
		s_animCoin = AnimationFrameList::create("sprites/coin-*.png", 1, 6, dg->spritesheet, dg->desc);
		s_ladder = SpriteSheetStore::getImage("sprites/ladder.png");

	}

	m_animation = new Animation();
	m_animation->setFrames(s_animCoin);
	m_animation->setFrameTime(0.1f);

    m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, 7.0f, 9.0f);
	m_bounds->setLocationByCenter(64, 32);

	m_velocity->set(0, 0);
	m_velocityMax->set(100, 100);
	m_acceleration->set(0, 0);

	reset();
}

void Pickup::reset() {
	Object::reset();
	m_grounded = false;
	m_tumbling = false;
	m_rotation = 0.0f;

	m_introTimer = 0.0f;
	m_introDuration = 0.5f;

	m_lightingRadius = 5;
}

float Pickup::getScale() {
	float scWidth = 1.0f;
	float scHeight = 1.0f;
	if (m_introTimer >= 0.01f) {
		//scWidth = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 1.25f, 1.0f, m_introDuration);
		//scHeight = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.75f, 1.0f, m_introDuration);

		scWidth = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
		scHeight = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}
	return scWidth;
}


void Pickup::start(PickupType type) {
	m_type = type;

	if (type == PU_COIN) {
		m_tumbling = false;
		m_animation->setFrames(s_animCoin);

		m_bounds->asRectangle()->setSize(7.0f, 9.0f);

		m_velocity->set(0.0f, 0.0f);
		MathUtil::moveAngle<float>(m_velocity, MathUtil::randBetweenf(-120, -60), MathUtil::randBetweenf(50.0f, 80.0f));

		m_introTimer = 0.01f;
	}
	else if (type == PU_LADDER) {
		m_lightingRadius = 10;

		m_tumbling = false;
		m_animation->clear();
		m_animation->addFrame(s_ladder);

		m_bounds->asRectangle()->setSize(4.0f, 4.0f);

		m_velocity->set(0.0f, 0.0f);

		m_introTimer = 0.01f;

		m_grounded = true;
	}


}

void Pickup::update(GameContainer* container, GameTimer* timer) {
	Object::update(container, timer);

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer >= m_introDuration) {
			m_introTimer = 0.0f;
		}
	}

	m_animation->update(timer);


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

	m_velocity->frictionX(200 * timer->getDelta());
	m_velocity->frictionY(200 * timer->getDelta());

    DefaultGame* dg = DefaultGame::getInstance();
	if (m_type == PU_COIN && dg->stateUpgrades->hasMagnet()) {

        InGameState* igs = dg->stateInGame;

		float angle = MathUtil::anglef(m_bounds->getCenterX(), m_bounds->getCenterY(), igs->m_player->m_bounds->getCenterX(), igs->m_player->m_bounds->getCenterY() );
		float distance = MathUtil::distance(m_bounds->getCenterX(), m_bounds->getCenterY(), igs->m_player->m_bounds->getCenterX(), igs->m_player->m_bounds->getCenterY() );

		float m_movementSpeedMin = 1.0f;
		float m_movementSpeedMax = 60.0f;
		float m_movementSpeedDistance = 100.0f;

		float movementSpeedMax = m_movementSpeedMax; // 250.0f
		if (dg->stateUpgrades->hasMagnet()) {
			//movementSpeedMax = m_movementSpeedMax * 1.4f; //350.0f;
		}
		// if (m_size == 1 && dg->stateUpgrades->getUpgradeById(Upgrade::UPGRADE_MISC_MAGNET_STRONG)->purchased) {
		// 	movementSpeedMax = m_movementSpeedMax * 2.0f * 1.4f; //350.0f;
		// }

		// was 250.
		// now 350
		// it should be an upgrade!
		float speed = Easing::easebetween(Easing::QUADRATIC_OUT, distance, m_movementSpeedMax, m_movementSpeedMin, m_movementSpeedDistance);

		m_velocity->set(0, 0);
		MathUtil::moveAngle<float>(m_velocity, angle, speed);
	}

	m_rotation = 0;
	// if (!m_grounded) {
	// 	//Object::gravity(1.0f);
	// 	if (m_tumbling) {
	// 		m_rotation += 360.0f * timer->getDelta();
	// 	}
	// } else {
	// 	m_rotation = 0.0f;
	// 	m_velocity->set(0.0f, 0.0f);
	// }

	Object::move(1.0f);

	keepInBounds();

	//DefaultGame* dg = DefaultGame::getInstance();
	//InGameState* igs = dg->stateInGame;

	// m_bounds->asRectangle()->setLocationByCenter(
	// 	m_bounds->getCenterX() + (m_velocity->getX() * timer->getDelta()),
	// 	m_bounds->getCenterY() + (m_velocity->getY() * timer->getDelta())
	// );

}

void Pickup::collision(Object* other) {

}
void Pickup::collected(Player* player) {

	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_coins += 1;



	//igs->onScoreChange();

	DefaultGame::getInstance()->m_sfxMenuBuy->play();

	explode();
}
void Pickup::explode() {
    InGameState* igs = DefaultGame::getInstance()->stateInGame;
	setPendingRemoval(true);
	// MyParticle* particle = igs->m_particles->get();
	// particle->reset();
	// particle->start(MyParticle::TYPE_PRESENT_COLLECT);
	// particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	// particle->m_animation->addFrame(m_sprite);

	MyParticle* particle;
	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	particle->m_animation->reset();
	particle->m_animation->addFrame(MyParticleSprites::s_cloud11);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	particle->m_animation->reset();
	particle->m_animation->addFrame(MyParticleSprites::s_cloud11);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY() - 4);
	particle->m_animation->reset();
	particle->m_animation->addFrame(MyParticleSprites::s_cloud11);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	// box bits
	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX() - 2, m_bounds->getCenterY() - 2);
	particle->m_animation->reset();
	particle->m_animation->addFrame(MyParticleSprites::s_cloud11);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX() + 2, m_bounds->getCenterY() - 2);
	particle->m_animation->reset();
	particle->m_animation->addFrame(MyParticleSprites::s_cloud11);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX() - 2, m_bounds->getCenterY() + 2);
	particle->m_animation->reset();
	particle->m_animation->addFrame(MyParticleSprites::s_cloud11);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX() + 2, m_bounds->getCenterY() + 2);
	particle->m_animation->reset();
	particle->m_animation->addFrame(MyParticleSprites::s_cloud11);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));
}

void Pickup::render(GameContainer* container, Renderer* r)
{
	InGameState* igs = DefaultGame::getInstance()->stateInGame;
	r->setDrawColor(Color::white);

	float scale = 1.0f;
	float rotation = m_rotation;

	if (m_introTimer > 0.0f) {
		scale = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}

	Image* img = m_animation->getCurrentFrame();
	img->setAlpha(1.0f);
	if (!m_grounded) {
		img->setRotation(rotation);
		img->drawCenteredScaled(float(m_bounds->getCenterX()), float(ceil(m_bounds->getCenterY())), scale, scale);
	} else {
		img->setRotation(rotation);
		img->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY(), Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, scale, scale);
	}

	//r->drawRect(m_bounds->getMinX(), m_bounds->getMinY(), m_bounds->getWidth(), m_bounds->getHeight());
}

Pickup::~Pickup() {

}
