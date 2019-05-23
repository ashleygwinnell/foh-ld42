/*
 * Player.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "Player.h"
#include "../DefaultGame.h"

#include "Projectile.h"
#include "MyParticle.h"
#include "EightWayMovementComponent.h"
#include "Bomb.h"
#include "Enemy.h"
#include "Powerup.h"
//#include "../states/UpgradesState.h"

#include <ARK2D/Core/Tween/Timer.h>
#include <ARK2D/Util/CameraShake.h>

Image* Player::s_playerGibHead = NULL;
Image* Player::s_playerGibLeg = NULL;
Image* Player::s_playerWeapon = NULL;
Image* Player::s_playerHP2 = NULL;
Image* Player::s_playerHP3 = NULL;

Image* Player::s_shadow = NULL;

Player::Player():
	Object(),
	m_fireTimer(NULL),
	m_fireWaterTimer(NULL)
	{

	DefaultGame* dg = DefaultGame::getInstance();

	if (s_playerGibHead == NULL) {
		s_playerGibHead = SpriteSheetStore::getImage("sprites/player/player-gib-head.png");
		s_playerGibLeg = SpriteSheetStore::getImage("sprites/player/player-gib-leg.png");

		s_playerWeapon = SpriteSheetStore::getImage("sprites/player/player-weapon.png");
		s_playerWeapon->setCenterOfRotation(0, s_playerWeapon->getHeight()*0.5);

		s_shadow = SpriteSheetStore::getImage("sprites/player/player-shadow.png");

		s_playerHP2 = SpriteSheetStore::getImage("sprites/upgrades/hp-2-impl.png");
		s_playerHP3 = SpriteSheetStore::getImage("sprites/upgrades/hp-3-impl.png");
	}

	AnimationFrameList* idleFrames = AnimationFrameList::create("sprites/player/player-idle-*.png", 1, 10, dg->spritesheet, dg->desc);
	m_animationIdle = new Animation();
	m_animationIdle->setFrames(idleFrames);
	m_animationIdle->setFrameTime(0.06f);

	AnimationFrameList* runFrames = AnimationFrameList::create("sprites/player/player-walk-*.png", 1, 12, dg->spritesheet, dg->desc);
	m_animationRun = new Animation();
	m_animationRun->setFrames(runFrames);
	m_animationRun->setFrameTime(0.04f);

	m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, 8.0f, 8.0f);
	m_radius = 4;

	m_velocity->set(0, 0);
	m_acceleration->set(600, 600);

	m_fireTimer = new Timer(0.20f);
	m_fireWaterTimer = new Timer(0.20f);

	m_movement = new EightWayMovementComponent(this);
	m_movement->setController(true, 0, Gamepad::ANALOG_STICK_1_X);
	m_movement->setKeysArrowsWASD();

	reset();
}

void Player::hitByProjectile(Projectile* pr) {
	//pr->setPendingRemoval(true);
	m_lives--;
	onLoseLife();
	if (m_lives == 0) {
		die();
	}
}
void Player::hitByPickup(Pickup* pu) {
	m_lives--;
	onLoseLife();
	if (m_lives == 0) {
		die();
	}
}
void Player::hitByEnemy(Enemy* e) {
	m_lives--;
	onLoseLife();
	if (m_lives == 0) {
		die();
	}
	e->hit();
	//e->die();
}
void Player::onLoseLife() {
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;
	if (m_lives >= 1) { //} && igs->m_currentPowerupType == PT_LIVES_X2) {
		//dg->m_sfxPlayerHurt->play();
		dg->m_sfxPlayerDie->play();
		m_lostLifeTimer = 0.01f;
		m_lostLifeDuration = 1.0f;

		//igs->m_currentPowerupType = PT_NONE;
		//igs->m_currentPowerup = NULL;
	}
}

void Player::reset() {
	DefaultGame* dg = DefaultGame::getInstance();
	GameContainer* container = ARK2D::getContainer();

	Object::reset();
	m_lightingRadius = 35;

	m_bounds->setLocationByCenter(container->getWidth()*0.5, container->getHeight()*0.5);

	m_timer = 0.0f;

	m_lastFlameAtX = m_bounds->getCenterX();
	m_lastFlameAtY = m_bounds->getCenterY();
	m_flameEachDist = 10;

	m_fireAmmo = dg->stateUpgrades->getCurrentFireCapacity();
	m_fireMax = dg->stateUpgrades->getCurrentFireCapacity();
	m_waterAmmo = dg->stateUpgrades->getCurrentCO2Capacity();
	m_waterMax = dg->stateUpgrades->getCurrentCO2Capacity();

	m_gasAmmo = 3;
	m_gasMax = 3;

    m_burnProtection = false; //dg->stateUpgrades->getCurrentForUpgradeType(Upgrade::UPGRADE_TYPE_FEET) == Upgrade::UPGRADE_COLD_FEET;

	m_moving = false;
	m_grounded = true;
	m_facingLeft = false;
	m_rotation = 0.0f;
	m_weaponAngle = 0;

	m_introTimer = 0.01f;
	m_introDuration = 0.8f;

	m_lives = dg->stateUpgrades->getCurrentHP();
	m_lostLifeTimer = 0.0f;
	m_lostLifeDuration = 1.5f;

	m_velocity->set(0, 0);
	if (dg->stateUpgrades->getUpgradeById(Upgrade::UPGRADE_FAST_FEET) != NULL &&
		dg->stateUpgrades->getUpgradeById(Upgrade::UPGRADE_FAST_FEET)->purchased &&
		dg->stateUpgrades->getUpgradeById(Upgrade::UPGRADE_FAST_FEET)->equipped) {
		m_velocityMax->set(60, 60);
	} else {
		m_velocityMax->set(40, 40);
	}

	m_fireTimer->reset();
	m_fireWaterTimer->reset();

	m_dieReason = DIE_NULL;
}

bool Player::isDamagable() {
	return m_lostLifeTimer == 0.0f;
}

Vector2<float> Player::getGunEndPos() {
	Vector2<float> val(m_bounds->getCenterX(), m_bounds->getMinY());
	MathUtil::moveAngle<float>(&val, m_weaponAngle, s_playerWeapon->getWidth() * 0.75f);
    return val;
}

void Player::update(GameContainer* container, GameTimer* timer) {

	Object::update(container, timer);

	m_timer += timer->getDelta();

	// Movement
	//Object::gravity(1.0f);



	// Do friction towards gravity.
	// float frictionX = 0.0f;
	// float frictionY = 0.0f;
	// float frictionAngle = MathUtil::anglef(0.0f, 0.0f, m_velocity->getX(), m_velocity->getY(), true) - 180;
	// float currentAngle = MathUtil::anglef(0.0f, 0.0f, m_velocity->getX(), m_velocity->getY(), true);
	// MathUtil::moveAngle<float>(m_velocity, frictionAngle, 150.0f * timer->getDelta());
    DefaultGame* dg = DefaultGame::getInstance();
    InGameState* igs = DefaultGame::getInstance()->stateInGame;
	Input* in = ARK2D::getInput();
	//GameContainer* container = ARK2D::getContainer();
	//if (in->isKeyDown(Input::KEY_A) || in->isKeyDown(Input::KEY_LEFT)) {
	//	m_velocity->m_x -= m_acceleration->getX() * timer->getDelta();
	//}
	m_movement->update(container, timer);

	float angle = m_movement->m_lastAngle;
	bool keyDown = m_movement->m_stickPushing || m_movement->m_buttonPushing;

	if (m_movement->isLeftDown() && m_movement->isRightDown()) {

	} else if (m_movement->isLeftDown()) {
		//m_facingLeft = true;
	} else if (m_movement->isRightDown()) {
		//m_facingLeft = false;
	} else {

	}
	m_facingLeft = (in->getMouseX() <= m_bounds->getCenterX());

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer >= m_introDuration){
			m_introTimer = 0.0f;
		}
	}
	if (m_lostLifeTimer > 0.0f) {


		dg->hsvShader->set(
			0.005f * float(igs->m_level),
			Easing::easebetween(Easing::QUADRATIC_OUT, m_lostLifeTimer, 0.0f, 1.0f, m_lostLifeDuration),
			Easing::easebetween(Easing::QUADRATIC_IN, m_lostLifeTimer, 0.4f, 1.0f, m_lostLifeDuration)
		);

		m_lostLifeTimer += timer->getDelta();
		if (m_lostLifeTimer >= m_lostLifeDuration) {
			m_lostLifeTimer = 0.0f;
		}
	}

	float dist = MathUtil::distance(m_bounds->getCenterX(), m_bounds->getCenterY(), m_lastFlameAtX, m_lastFlameAtY);
	if (dist >= m_flameEachDist * 2) {

//        if (dg->stateUpgrades->getUpgradeById(Upgrade::UPGRADE_COLD_FEET)->purchased) {
//            // ...
//        }
//        else
        if (dg->stateUpgrades->getUpgradeById(Upgrade::UPGRADE_HOT_FEET)->purchased &&
        	dg->stateUpgrades->getUpgradeById(Upgrade::UPGRADE_HOT_FEET)->equipped) {

			// spawn flame
			float angle = MathUtil::anglef(m_lastFlameAtX, m_lastFlameAtY, m_bounds->getCenterX(), m_bounds->getCenterY());
			MathUtil::moveAngle<float>(m_lastFlameAtX, m_lastFlameAtY, angle, m_flameEachDist);

			Projectile* p = igs->m_projectiles->get();
			p->reset();
			p->start(PT_FLAME);
			p->m_bounds->setLocationByCenter(m_lastFlameAtX, m_lastFlameAtY);
			p->m_from.set(m_lastFlameAtX, m_lastFlameAtY);
			p->m_to.set(m_lastFlameAtX, m_lastFlameAtY);
			p->m_grounded = true;
			p->m_z = 0;
			p->m_speed = 0;
			p->m_lifetimeDuration = 3.0f;
			//p->m_bounds->setLocationByCenter(in->getMouseX(), in->getMouseY());
		}

	}


	m_weaponAngle = MathUtil::anglef(m_bounds->getCenterX(), m_bounds->getMinY(), in->getMouseX(), in->getMouseY());


		if (!igs->m_died && keyDown) {
			//m_animationIdle->setFrameTime(0.25f);
			m_moving = true;

			float distance = 1.0f;
			float maxAcceleration = 800.0f;

			m_acceleration->set(0.0f, 0.0f);
			MathUtil::moveAngle<float>(m_acceleration, angle, maxAcceleration);

			m_velocity->addX(m_acceleration->getX() * timer->getDelta());
			m_velocity->addY(m_acceleration->getY() * timer->getDelta());
			m_velocity->restrictLength(m_velocityMax->length());
		} else {
			//m_animationIdle->setFrameTime(0.5f);

			m_animationRun->setTime(0.0f);
			m_moving = false;
			m_acceleration->set(0.0f, 0.0f);

			m_velocity->frictionX(600 * timer->getDelta());
			m_velocity->frictionY(600 * timer->getDelta());

			//m_walkWobbleTimer = 0.0f;

			//m_animationIdle->setFrameTime(0.5f);
			//m_animationIdleBack->setFrameTime(0.5f);
		}

		//m_velocity->m_y = 0;


	if (!igs->m_died) {
		if (in->isKeyDown(Input::MOUSE_BUTTON_LEFT) || in->isKeyDown(Input::MOUSE_BUTTON_RIGHT)) {

			auto FireAProjectile = [this, igs, in](ProjectileType type)->void {
				Projectile* p = igs->m_projectiles->get();
				p->reset();
				p->start(type);
				Vector2<float> pos = getGunEndPos();

				p->m_bounds->setLocationByCenter(pos.getX(), pos.getY());
				p->m_from.set(pos.getX(), pos.getY());
				float maxDist = 80;
				float thisAngle = MathUtil::anglef(p->m_from.getX(), p->m_from.getY(), in->getMouseX(), in->getMouseY());
				float thisDist = MathUtil::distance(p->m_from.getX(), p->m_from.getY(), in->getMouseX(), in->getMouseY());
				p->m_dist = std::min<float>(maxDist, thisDist);


				float toX = p->m_from.getX();
                float toY = p->m_from.getY();
				MathUtil::moveAngle<float>(toX, toY, thisAngle, p->m_dist);
				p->m_to.set(toX, toY);

				p->m_speed = MathUtil::distance(p->m_from.getX(), p->m_from.getY(), toX, toY);

				p->m_z = m_bounds->getHeight();
				p->m_zSpeed = 0;
				p->m_zAcceleration = 30;
			};

			bool fireKey = in->isKeyDown(Input::MOUSE_BUTTON_LEFT) && !in->isKeyDown(Input::KEY_LSHIFT);
			bool co2Key = (in->isKeyDown(Input::MOUSE_BUTTON_LEFT) && in->isKeyDown(Input::KEY_LSHIFT)) || in->isKeyDown(Input::MOUSE_BUTTON_RIGHT);

			if (in->countGamepads() > 0) {
				if (in->isGamepadButtonDown(Gamepad::BUTTON_A) && !in->isGamepadButtonDown(Gamepad::BUTTON_B)) {
					fireKey = true;
				}
				if (in->isGamepadButtonDown(Gamepad::BUTTON_B) && !in->isGamepadButtonDown(Gamepad::BUTTON_A)) {
					co2Key = true;
				}
			}

			//float fireAmmo =

			if (m_fireTimer->update()) {
				if ((m_fireAmmo > 0 || m_fireMax == -1) && fireKey) {
					m_fireTimer->loop();
					FireAProjectile(PT_FLAME);
					m_fireAmmo--;


					dg->stateInGame->m_cameraShake->start(2, 0.25f);
					//m_waterAmmo++;

					dg->m_sfxFireFlames->play();
				}
				else if ((m_waterAmmo > 0 || m_waterMax == -1) && co2Key) {
					m_fireTimer->loop();
					FireAProjectile(PT_CO2);
					m_waterAmmo--;

					dg->stateInGame->m_cameraShake->start(3, 0.25f);

					dg->m_sfxFireGas->play();

					Vector2<float> pos = getGunEndPos();
					float thisAngle = MathUtil::anglef(pos.getX(), pos.getY(), in->getMouseX(), in->getMouseY());
					igs->spawnSmoke(pos.getX(), pos.getY(), 0, 5, Color::white.pointer(), thisAngle-5, thisAngle + 5);

				}
				else {
					m_fireTimer->setTime(m_fireTimer->getDuration());
				}
			}
			//dg->m_sfxPlayerThrow->play();
		} else {
			m_fireTimer->setTime(m_fireTimer->getDuration());
		}

		// m_fireWaterTimer

	}


	//if (in->isKeyPressed(Input::KEY_I)) {
		//m_numBoomerangs++;
	//}

	//m_velocity->set(0.0f, 0.0f);

	Object::move(1.0f);
	bool b = keepInBounds();
	if (b) {
		//igs->m_cameraShake->setSpeedMultiplier(1, 1.0f);
		//igs->m_cameraShake->start(1, 2.0f);
	}

	m_animationIdle->update(timer);
	m_animationRun->update(timer);

	// Set rotation of player for velocity
	//m_rotation = MathUtil::anglef(0,0, m_velocity->getX(), m_velocity->getY()) + 90;
}
void Player::collision(Object* other) {

}
void Player::die() {

	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	if (!igs->m_died) {
		dg->stateInGame->m_cameraShake->start(1, 1.0f);
		dg->m_sfxPlayerDie->play();

        if (dg->m_musicGame != NULL) {
            if (dg->m_musicGame->isPlaying()) {
                dg->m_musicGame->setLowpass(true);
                dg->m_musicGame->setLowpassFrequency(500.0f); //dg->m_musicGame->setLowpassFrequency(22100.0f);
            }
        }



		igs->spawnSmoke(m_bounds->getCenterX(), m_bounds->getCenterY(), 5, 10);
		//igs->spawnBlood(m_bounds->getCenterX(), m_bounds->getCenterY(), 5, 10, 190, 350);
		igs->m_died = true;

		// player giblets
		MyParticle* part;
		float strengthMin = 60.0f;
		float strengthMax = 120.0f;
		part = igs->m_particles->get();
		part->reset();
		part->m_animation->reset();
		part->m_animation->addFrame(s_playerGibHead);
		part->m_type = MyParticle::TYPE_PLAYER_GIB;
		part->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY()-4);
		MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(-120,-60), MathUtil::randBetweenf(strengthMin, strengthMax));

		part = igs->m_particles->get();
		part->reset();
		part->m_animation->reset();
		part->m_animation->addFrame(s_playerGibLeg);
		part->m_type = MyParticle::TYPE_PLAYER_GIB;
		part->m_bounds->setLocationByCenter(m_bounds->getCenterX() - 4, m_bounds->getCenterY() + 4);
		MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(-120,-60), MathUtil::randBetweenf(strengthMin, strengthMax));

		part = igs->m_particles->get();
		part->reset();
		part->m_animation->reset();
		part->m_animation->addFrame(s_playerGibLeg);
		part->m_type = MyParticle::TYPE_PLAYER_GIB;
		part->m_bounds->setLocationByCenter(m_bounds->getCenterX() + 4, m_bounds->getCenterY() + 4);
		MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(-120,-60), MathUtil::randBetweenf(strengthMin, strengthMax));
	}

}
Vector2<float> Player::getHandPosition() {
	//float facingMultiplier = (m_facingLeft)?1:-1;
	//float handX = m_bounds->getCenterX() + (6*facingMultiplier);
	//float handY = m_bounds->getCenterY() - 0.5f;

	float facingMultiplier = (m_facingLeft)?-1:1;
	float handX = m_bounds->getCenterX();
	float handY = m_bounds->getCenterY();
	MathUtil::moveAngle<float>(handX, handY, ((m_rotation+90)*facingMultiplier)-90, 6);

	return Vector2<float>(handX, handY);
}

void Player::render(GameContainer* container, Renderer* r)
{
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;
	if (igs->m_died) {
		return;
	}

	if (m_lostLifeTimer > 0.0f) {
		if (fmod(m_lostLifeTimer, 0.1f) < 0.05f) {
			return;
		}
	}

	float scale = 1.0f;

	auto getOrbitProgressX = [this](float timer, float duration, bool& front) -> float {
		float thisDuration = duration;
		float thisHalfDuration = thisDuration*0.5f;
		float thisTimer = fmod(timer, thisDuration);
		float progress;
		if (thisTimer <= thisHalfDuration) { // front?
			front = true;
			progress = Easing::easebetween(Easing::QUADRATIC_IN_OUT, thisTimer, -1.0f, 1.0f, thisHalfDuration);
		} else { // back?
			front = false;
			progress = Easing::easebetween(Easing::QUADRATIC_IN_OUT, thisTimer-thisHalfDuration, 1.0f, -1.0f, thisHalfDuration);
		}
		return progress;
	};
	auto getOrbitProgressY = [this](float timer, float duration) -> float {
		float thisDuration = duration;
		float thisHalfDuration = thisDuration*0.5f;
		float thisTimer = fmod(timer, thisDuration);
		float progress;
		if (thisTimer <= thisHalfDuration) { // front?
			progress = Easing::easebetweenback(Easing::QUADRATIC_OUT, Easing::QUADRATIC_IN, thisTimer, 0.0f, 1.0f, thisHalfDuration);
		} else {
			progress = Easing::easebetweenback(Easing::QUADRATIC_OUT, Easing::QUADRATIC_IN, thisTimer-thisHalfDuration, 0.0f, -1.0f, thisHalfDuration);
		}
		return progress;
	};
	auto doOrbitRenderIfFront = [getOrbitProgressX,getOrbitProgressY,this](Image* spr, float timer, float duration, float orbitX, float orbitY, bool f)->void {
		bool front;
		float px = getOrbitProgressX(timer, duration, front);
		float py = getOrbitProgressY(timer, duration);
		if (f == front) {
			spr->drawCentered(m_bounds->getCenterX() + (px*orbitX), m_bounds->getMinY() + (py*orbitY));
		}
    };

	if (m_lives >= 2) {
		doOrbitRenderIfFront(s_playerHP2, m_timer, 2.0f, 10, 3, false);
	}
	if (m_lives >= 3) {
		doOrbitRenderIfFront(s_playerHP3, m_timer+1.0f, 2.0f, 10, 3, false);
	}

	//pm->image->drawCenteredScaled(planetX + (pm->getX()*mult*scale), planetY + (pm->getY()*mult*scale), scale, scale);


	s_playerWeapon->setRotation(m_weaponAngle);
	s_playerWeapon->drawAligned(m_bounds->getCenterX(), m_bounds->getMinY(), Renderer::ALIGN_LEFT, Renderer::ALIGN_CENTER);
	//if (m_introTimer > 0.0f) {
		//scale = Easing::easebetween(Easing::QUADRATIC_IN, m_introTimer, 4.0f, 1.0f, m_introDuration);
	//}
//    if (m_bounceTimer->getTime() > 0.0f) {
//        scale *= Easing::easebetween(Easing::QUADRATIC_OUT, m_bounceTimer->getTime(), 1.2f, 1.0f, m_bounceTimer->getDuration());
//    }
	r->setDrawColor(Color::white);

	//float rot = m_rotation;
	if (m_grounded) {
		//rot = 0;
	}


	Image* img = NULL;
	if (m_moving) {
		img = m_animationRun->getCurrentFrame();
	} else {
		img = m_animationIdle->getCurrentFrame();
	}


	if (m_facingLeft) {
		img->flip(true, false);
		img->setRotation(m_rotation*-1);
	} else {
		img->setRotation(m_rotation);
	}

    float playerOffsetY = 0; //(m_boots != BOOTS_NONE)?-2:0;

	img->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY()-0.5f+playerOffsetY, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, scale,scale);

	/*Image* bootsSprite = NULL;
	if (m_boots == BOOTS_MOON) {
		bootsSprite = s_playerBootsMoon;
	} else if (m_boots == BOOTS_HOVER) {
		bootsSprite = s_playerBootsHover;
	}
	float bootsX = m_bounds->getCenterX();
	float bootsY = m_bounds->getCenterY();
	if (bootsSprite != NULL) {
		float facingMultiplier = (m_facingLeft)?-1:1;
		//if (!m_facingLeft) {
        	MathUtil::moveAngle<float>(bootsX, bootsY, (m_rotation*facingMultiplier)+90, (m_bounds->getHeight()*0.5f));
			bootsSprite->setRotation(m_rotation*facingMultiplier);
			bootsSprite->drawAligned(bootsX, bootsY, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, scale);
   //      } else {
   //      	MathUtil::moveAngle<float>(bootsX, bootsY, (m_rotation*-1)+90, (m_bounds->getHeight()*0.5f));
			// bootsSprite->setRotation(m_rotation*-1);
			// bootsSprite->drawAligned(bootsX, bootsY, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, scale);
   //      }
	}*/



	if (m_facingLeft) { img->flip(true, false); }

	if (m_lives >= 2) {
		doOrbitRenderIfFront(s_playerHP2, m_timer, 2.0f, 10, 3, true);
	}
	if (m_lives >= 3) {
		doOrbitRenderIfFront(s_playerHP3, m_timer+1.0f, 2.0f, 10, 3, true);
	}

	//m_face->setRotation(rot);
	//m_face->drawCenteredScaled(m_bounds->getCenterX(), m_bounds->getCenterY(), scale, scale);
	//r->fillCircle(m_bounds->getCenterX(), m_bounds->getCenterY(), (m_bounds->getWidth()/2.0f) * scale, 20);
}

Player::~Player() {
	delete m_fireTimer;
	delete m_fireWaterTimer;
}
