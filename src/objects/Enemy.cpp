/*
 * Enemy.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "Enemy.h"
#include "../DefaultGame.h"

#include "Projectile.h"
#include "MyParticle.h"
#include "Pickup.h"
#include "Bomb.h"
#include "Player.h"
#include "EightWayMovementComponent.h"
//#include "../states/UpgradesState.h"

#include <ARK2D/Core/Tween/Timer.h>
#include <ARK2D/Util/CameraShake.h>

Image* Enemy::s_uiPipBig = NULL;
Image* Enemy::s_uiPipSmall = NULL;
Image* Enemy::s_enemyMiniFrame = NULL;
Image* Enemy::s_enemySmallFrame = NULL;
Image* Enemy::s_enemyMediumFrame = NULL;
Image* Enemy::s_enemyLargeFrame = NULL;
AnimationFrameList* Enemy::s_enemyFlyingFrames = NULL;
Image* Enemy::s_enemySpittingFrame = NULL;
Image* Enemy::s_enemySpittingAttackFrame = NULL;


Enemy::Enemy():
	Object()
	{

	DefaultGame* dg = DefaultGame::getInstance();

	if (s_enemySmallFrame == NULL) {

		s_uiPipBig = SpriteSheetStore::getImage("sprites/ui-health-pip-4.png");
		s_uiPipSmall = SpriteSheetStore::getImage("sprites/ui-health-pip-1.png");

		s_enemyMiniFrame = SpriteSheetStore::getImage("sprites/enemy-mini.png");
		s_enemySmallFrame = SpriteSheetStore::getImage("sprites/enemy-small.png");
		s_enemyMediumFrame = SpriteSheetStore::getImage("sprites/enemy-medium.png");
		s_enemyLargeFrame = SpriteSheetStore::getImage("sprites/enemy-large.png");
		s_enemyFlyingFrames = AnimationFrameList::create("sprites/enemy-flying-*.png", 1, 3, dg->spritesheet, dg->desc);
		s_enemySpittingFrame = SpriteSheetStore::getImage("sprites/enemy-spitting.png");
		s_enemySpittingAttackFrame = SpriteSheetStore::getImage("sprites/enemy-spitting-attack.png");
	}

	m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, 7, 7);


	m_velocity->set(0, 0);
	m_velocityMax->set(40, 40);
	m_acceleration->set(700, 700);

	m_animation = new Animation();
	m_animation->setFrameTime(0.5f);

	reset();
}

bool Enemy::canHurtPlayer(Player* p) {
	// if (m_type == ET_MOLE) {
	// 	return false;
	// }
	return true;
}
bool Enemy::canMove() {
	if (m_spitFireCooldownTimer > 0.0f) { return false; }
	return true;
}

void Enemy::reset() {
	Object::reset();

	DefaultGame* dg = DefaultGame::getInstance();
	GameContainer* container = ARK2D::getContainer();

	m_radius = 4;


	m_velocity->set(0, 0);
	m_velocityMax->set(40, 40);

	m_followPlayer = false;
	m_followPlayerDistance = 0;
	m_followVelocity = false;
	m_targetAngleOffset = -40.0f + MathUtil::randBetweenf(0.0f, 80.0f);;
	m_bumpPower = 100.0f;

	m_damagable = true;

	m_introTimer = 0.0f;
	m_introDuration = 0.3f;

	m_hitTimes = 0;
	m_hitHealth = 0;
	m_hitTimer = 0.0f;
	m_hitDuration = 0.5f;

	m_dropsCoinAmount = 0;
	m_dropsCoinChance = 1.0f;

	m_hopping = false;
	m_enemyHopTimer = 0;
	m_enemyHopDuration = 0;
	m_enemyHopHeight = 0;

	m_squidging = false;
	m_squidgingTimer = 0.0f;
	m_squidgingDuration = 0.0f;

	m_dropFire = false;
	m_dropFireTimer = 0;
	m_dropFireDuration = 0;

	m_spitFire = false;
	m_spitFireTimer = 0;
	m_spitFireDuration = 0;
	m_spitFireCooldownTimer = 0.0f;
	m_spitFireCooldownDuration = 0.0f;

	m_creatingChildren = false;
	m_creatingChildrenPalette.clear();
	m_creatingChildrenTimer = 0.0f;
	m_creatingChildrenDuration = 0.0f;

	m_speedMin = 0;
	m_speedMax = 0;
	m_speed = 0;
}

void Enemy::start(EnemyType et) {
	m_type = et;

    GameContainer* container = ARK2D::getContainer();
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	m_introTimer = 0.01f;

	m_bounds->asRectangle()->setSize(8, 6);
	m_bounds->setLocationByCenter(container->getWidth()*0.5f, container->getHeight()*0.5f);

	m_animation->clear();
	m_animation->setFrameTime(0.5f);

	m_followPlayer = true;
	m_followVelocity = false;

	m_hopping = true;
	m_enemyHopTimer = 0.01f;

	if (et == ET_MINI) {
		m_animation->addFrame(s_enemyMiniFrame);

		m_enemyHopDuration = 0.20f;
		m_enemyHopHeight = 1.0;

		m_speedMin = 12.0f;
		m_speedMax = 24.0f;

		m_hitHealth = 1;

		m_dropsCoinChance = 0.2f;
		m_dropsCoinAmount = 1;
	}
	else if (et == ET_SMALL) {
		m_animation->addFrame(s_enemySmallFrame);

		m_enemyHopDuration = 0.25f;
		m_enemyHopHeight = 1.5;

		m_speedMin = 20.0f;
		m_speedMax = 35.0f;

		m_hitHealth = 2;

		m_dropsCoinAmount = 1;
	}
	else if (et == ET_MED) {
		m_bounds->asRectangle()->setSize(10, 8);
		m_animation->addFrame(s_enemyMediumFrame);

		m_enemyHopDuration = 0.35f;
		m_enemyHopHeight = 2.5;

		m_speedMin = 15.0f;
		m_speedMax = 45.0f;

		m_hitHealth = 4;

		m_dropsCoinAmount = 2;
	}
	else if (et == ET_LARGE) {
		m_bounds->asRectangle()->setSize(10, 8);
		m_animation->addFrame(s_enemyLargeFrame);

		m_enemyHopDuration = 0.35f;
		m_enemyHopHeight = 3.0;

		m_speedMin = 20.0f;
		m_speedMax = 25.0f;

		m_hitHealth = 10;

		m_dropsCoinAmount = 5;

		m_creatingChildren = true;
		m_creatingChildrenPalette.push_back(ET_MINI);
		m_creatingChildrenTimer = 0.0f;
		m_creatingChildrenDuration = 2.5f;
	}
	else if (et == ET_FLYING) {
		m_followPlayer = false;
		m_followVelocity = true;
		m_animation->setFrames(s_enemyFlyingFrames);
		m_animation->setFrameTime(0.13333333f);

		m_enemyHopDuration = 0.4f;
		m_enemyHopHeight = 5.0f;

		m_speedMin = 30.0f;
		m_speedMax = 30.0f;

		m_hitHealth = 1;

		m_dropsCoinAmount = 1;

		m_dropFire = true;
		m_dropFireTimer = 1.5f;
		m_dropFireDuration = 3.0f;

		m_velocity->set(0, 0);
		MathUtil::moveAngle<float>(m_velocity, MathUtil::snap_return(MathUtil::randBetweenf(0.0f,360.0f), 90.0f) + 45.0f, m_speedMax);
		m_velocityMax->set(m_velocity);
	}
	else if (et == ET_SPITTING) {
		m_bounds->asRectangle()->setSize(10, 10);

		m_followPlayer = true;
		m_followPlayerDistance = 5.0f;
		m_followVelocity = false;
		m_animation->addFrame(s_enemySpittingFrame);

		m_hopping = false;

		m_squidging = true;
		m_squidgingTimer = 0.0f;
		m_squidgingDuration = 0.4f;

		m_speedMin = 0.5f;
		m_speedMax = 15.0f;

		m_hitHealth = 3;

		m_dropsCoinAmount = 1;

		m_spitFire = true;
		m_spitFireTimer = MathUtil::randBetweenf(1.5f, 2.5f);
		m_spitFireDuration = 3.0f;

		m_spitFireCooldownTimer = 0.0f;
		m_spitFireCooldownDuration = 0.25f;

	}

}
bool Enemy::keepInBounds() {
	bool b = Object::keepInBounds();
	if (m_followVelocity) {
		m_velocityMax->set(m_velocity);
	}
	return b;
}

void Enemy::smoke() {
    InGameState* igs = DefaultGame::getInstance()->stateInGame;
	igs->spawnSmoke(m_bounds->getCenterX(), m_bounds->getCenterY(), m_radius, 8.0f);
}
void Enemy::hit() {
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	m_hitTimes++;
	m_hitTimer = 0.001f;

	dg->stateInGame->m_cameraShake->start(0, float(m_hitTimes));

	if (m_hitTimes >= m_hitHealth &&
		(m_type == ET_SMALL || m_type == ET_MINI || m_type == ET_MED || m_type == ET_LARGE || m_type == ET_FLYING || m_type == ET_SPITTING)
		) {
		MyParticle* particle;
		particle = igs->m_particles->get();
		particle->reset();
		particle->m_animation->reset();
		particle->m_animation->addFrame( m_animation->getCurrentFrame() );
		particle->m_type = MyParticle::TYPE_PLAYER_GIB;
		particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
		particle->m_velocity->setX(-10 + MathUtil::randBetweenf(0.0f, 20.0f));
		particle->m_velocity->setY(-40);
        particle->m_rotateToVelocity = true;
        particle->m_rotationOffset = 90;

		die();

		if (m_type == ET_MINI) {
			dg->m_sfxEnemyDeathMini->play();
		} else if (m_type == ET_SMALL || m_type == ET_FLYING) {
			dg->m_sfxEnemyDeathSmall->play();
		} else if (m_type == ET_MED || m_type == ET_SPITTING) {
			dg->m_sfxEnemyDeathMedium->play();
		} else if (m_type == ET_LARGE) {
			dg->m_sfxEnemyDeathMedium->play();
		}
	}
}
bool Enemy::killableBySmartBomb() {
	// if (m_type == ET_SANTA) {
	// 	return false;
	// }
	return true;
}
void Enemy::die() {
	setPendingRemoval(true);
	// play sound.
	killParticles(-90, 30, 30);
}


void Enemy::killParticles(float startAngle, float power, float powerSpread) {
    DefaultGame* dg = DefaultGame::getInstance();
    InGameState* igs = dg->stateInGame;

	igs->spawnBlood(m_bounds->getCenterX(), m_bounds->getCenterY(), 1, 4);
}

bool Enemy::isDamagable() {
	return m_damagable && m_hitTimer == 0.0f;
}
void Enemy::update(GameContainer* container, GameTimer* timer) {
    DefaultGame* dg = DefaultGame::getInstance();
    InGameState* igs = dg->stateInGame;

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer >= m_introDuration) {
			m_introTimer = 0.0f;
		}
	}

	//if (m_type == ET_SMALL || ) {
		checkPlayerCollisions(true, true);
		checkEnemyCollisions();
	//}

	if (m_hopping) {
		m_enemyHopTimer += timer->getDelta();
		if (m_enemyHopTimer >= m_enemyHopDuration) {
			m_enemyHopTimer -= m_enemyHopDuration;
		}
	}
	if (m_squidging) {
		m_squidgingTimer += timer->getDelta();
		if (m_squidgingTimer >= m_squidgingDuration) {
			m_squidgingTimer -= m_squidgingDuration;
		}
	}
	if (m_dropFire) {
		m_dropFireTimer += timer->getDelta();
		if (m_dropFireTimer >= m_dropFireDuration) {
			m_dropFireTimer -= m_dropFireDuration;

			float curX = m_bounds->getCenterX();
			float curY = m_bounds->getMaxY();
			MathUtil::moveAngle<float>(curX, curY, m_velocity->angle(), -5.0f);
			Projectile* pr = igs->startFireAt(curX, curY);
			pr->m_isFromPlayer = false;
			pr->m_introDuration = 0.4f;
		}
	}
	if (m_spitFire) {
		if (m_spitFireCooldownTimer == 0.0f) {
			m_spitFireTimer += timer->getDelta();
			if (m_spitFireTimer >= m_spitFireDuration - 0.2f) {
				m_animation->clear();
				m_animation->addFrame(s_enemySpittingAttackFrame);
			}
			if (m_spitFireTimer >= m_spitFireDuration) {
				m_spitFireTimer -= m_spitFireDuration;

				m_spitFireCooldownTimer	= 0.01f;
				m_animation->clear();
				m_animation->addFrame(s_enemySpittingAttackFrame);


				float angleToPlayer = MathUtil::anglef(m_bounds->getCenterX(), m_bounds->getCenterY(), igs->m_player->m_bounds->getCenterX(), igs->m_player->m_bounds->getCenterY());
				float distToPlayer = MathUtil::distance(m_bounds->getCenterX(), m_bounds->getCenterY(), igs->m_player->m_bounds->getCenterX(), igs->m_player->m_bounds->getCenterY());

				// Fire a projectile (player func)
				Vector2<float> pos(m_bounds->getCenterX(), m_bounds->getCenterY());
				Projectile* p = igs->startFireAt(m_bounds->getCenterX(), m_bounds->getCenterY());
				p->m_grounded = false;
				p->m_isFromPlayer = false;
				p->m_hasLifetime = true;


				p->m_bounds->setLocationByCenter(pos.getX(), pos.getY());
				p->m_from.set(pos.getX(), pos.getY());
				float maxDist = 80;
				float thisAngle = angleToPlayer;////MathUtil::anglef(p->m_from.getX(), p->m_from.getY(), in->getMouseX(), in->getMouseY());
				float thisDist = distToPlayer; //MathUtil::distance(p->m_from.getX(), p->m_from.getY(), in->getMouseX(), in->getMouseY());
				p->m_dist = std::min<float>(maxDist, thisDist);


				float toX = p->m_from.getX();
                float toY = p->m_from.getY();
				MathUtil::moveAngle<float>(toX, toY, thisAngle, p->m_dist);
				p->m_to.set(toX, toY);

				p->m_speed = MathUtil::distance(p->m_from.getX(), p->m_from.getY(), toX, toY);

				p->m_z = m_bounds->getHeight();
				p->m_zSpeed = 0;
				p->m_zAcceleration = 30;

			}
		} else {
			m_spitFireCooldownTimer += timer->getDelta();
			if (m_spitFireCooldownTimer >= m_spitFireCooldownDuration) {
				m_spitFireCooldownTimer = 0.0f;

				m_animation->clear();
				m_animation->addFrame(s_enemySpittingFrame);
			}
		}
	}
	if (m_creatingChildren) {
		m_creatingChildrenTimer += timer->getDelta();
		if (m_creatingChildrenTimer >= m_creatingChildrenDuration) {
			m_creatingChildrenTimer -= m_creatingChildrenDuration;

			if (m_creatingChildrenPalette.size() > 0) {
				EnemyType type = m_creatingChildrenPalette[MathUtil::randBetween(0, m_creatingChildrenPalette.size())];
				Enemy* child = igs->spawnEnemy(type);
				child->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
			}
		}
	}


	m_animation->update(timer);

	if (canMove()) {
		Object::move(1.0f);
	}
	keepInBounds();

	if (m_hitTimer > 0.0f) {
		m_hitTimer += timer->getDelta();
		if (m_hitTimer >= m_hitDuration) {
			m_hitTimer = 0.0f;
		}
	}
}



void Enemy::checkPlayerCollisions(bool firstPass, bool movePlayer) {
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	// move towards player
  	if (m_followPlayer) {
        float angle = MathUtil::anglef(
                                       m_bounds->getCenterX(),
                                       m_bounds->getCenterY(),
                                       igs->m_player->m_bounds->getCenterX(),
                                       igs->m_player->m_bounds->getCenterY()
                                       );
        float distance = MathUtil::distance(
                                            m_bounds->getCenterX(),
                                            m_bounds->getCenterY(),
                                            igs->m_player->m_bounds->getCenterX(),
                                            igs->m_player->m_bounds->getCenterY()
                                            ) - igs->m_player->m_radius - m_radius + 1.0f;
	    float speed = std::max<float>(std::min<float>(distance, m_speedMax),m_speedMin);
	    //if (m_type == TYPE_LARGE) {
	    	//speed = std::max(std::min(distance, 100.0f),30.0f);
	    //}
	    if (distance < m_followPlayerDistance) {
	    	speed = 0;
	    }
	    m_speed = speed;

	    m_velocity->set(0.0f, 0.0f);
		MathUtil::moveAngle<float>(m_velocity, angle - 10 + MathUtil::randBetweenf(0.0f, 20.0f) + m_targetAngleOffset, speed);
		m_velocity->restrictMax(m_velocityMax);
	}
	else if (m_followVelocity) {
		// .. keep velocity.
		m_velocity->set(m_velocityMax);
		m_speed = MathUtil::distance(0,0,m_velocity->getX(), m_velocity->getY());
	}

	return;

	/*float result[2];
	bool collides = Shape<float>::collision_circleCircleVec(
		&result[0],
		m_bounds->getCenterX(), m_bounds->getCenterY(), m_radius,
		igs->m_player->m_bounds->getCenterX(), igs->m_player->m_bounds->getCenterY(), igs->m_player->m_radius
	);
	if (collides) {
		if (movePlayer) { // push player
			float newLocX = m_bounds->getCenterX();
			float newLocY = m_bounds->getCenterY();
			MathUtil::moveAngle<float>(newLocX, newLocY, angle, igs->m_player->m_radius + m_radius+0.1f);
			igs->m_player->m_bounds->setLocationByCenter(newLocX,newLocY);

			igs->m_player->m_velocity->set(0.0f, 0.0f);
			MathUtil::moveAngle<float>(igs->m_player->m_velocity, angle, m_bumpPower);
			//m_velocity->set(0.0f, 0.0f);

			if (firstPass) {
				PoolIterator<Enemy*> it = igs->m_enemies->newiteratorref();
				while(it.hasNext()) {
					Enemy* e = it.next();
					if (e == this) { continue; }
					e->checkPlayerCollisions(false, false);
				}
			}

		} else { // push enemy back
			float newLocX = igs->m_player->m_bounds->getCenterX();
			float newLocY = igs->m_player->m_bounds->getCenterY();
			MathUtil::moveAngle<float>(newLocX, newLocY, angle-180, igs->m_player->m_radius + m_radius);
			m_bounds->setLocationByCenter(newLocX,newLocY);
		}
	}*/
}
void Enemy::checkEnemyCollisions() {
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	// if collides with other enemies, move them out of the way too.
    float result[2];
	while (true) {
		bool didCollide = false;
		PoolIterator<Enemy*> it = igs->m_enemies->newiteratorref();
		while(it.hasNext()) {
			Enemy* e = it.next();
			if (e == this) { continue; }
			bool collides = Shape<float>::collision_circleCircleVec(
				&result[0],
				m_bounds->getCenterX(), m_bounds->getCenterY(), m_radius,
				e->m_bounds->getCenterX(), e->m_bounds->getCenterY(), e->m_radius
			);
			if (collides) {
				float angle = MathUtil::anglef(
					m_bounds->getCenterX(),
					m_bounds->getCenterY(),
					e->m_bounds->getCenterX(),
					e->m_bounds->getCenterY()
				);
				float newLocX = m_bounds->getCenterX();
				float newLocY = m_bounds->getCenterY();
				MathUtil::moveAngle<float>(newLocX, newLocY, angle, e->m_radius + m_radius + 0.02f);
				e->m_bounds->setLocationByCenter(newLocX, newLocY);
				m_velocity->set(0.0f, 0.0f);
				e->m_velocity->set(0.0f, 0.0f);
				didCollide = true;

				m_targetAngleOffset = -40.0f + MathUtil::randBetweenf(0.0f, 80.0f);
				e->m_targetAngleOffset = -40.0f + MathUtil::randBetweenf(0.0f, 80.0f);
			}
		}
		if (!didCollide) {
			break;
		}
	}
}


void Enemy::collision(Object* other) {

}

float Enemy::getScale() {
	float scale = 1.0f;
	if (m_introTimer > 0.0f) {
        scale = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}
	else if (m_hitTimer > 0.0f) {
        scale = Easing::easebetween(Easing::QUADRATIC_OUT, m_hitTimer, 1.35f, 1.0f, m_hitDuration);
	}
	return scale;
}
float Enemy::getShadowScale() {
	float sc = getScale();
	if (m_hopping) {
		sc *= Easing::easebetweenback(Easing::QUADRATIC_OUT, Easing::QUADRATIC_IN, m_enemyHopTimer, 1.0f, 0.8, m_enemyHopDuration);
	}
	return sc;
}

void Enemy::render(GameContainer* container, Renderer* r)
{
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;
	// if (igs->m_died) {
	// 	return;
	// }

	r->setDrawColor(Color::white);

	float scale = getScale();
	float scaleX = scale;
	float scaleY = scale;

	if (m_squidging) {
		scaleX = Easing::easebetweenback(Easing::QUADRATIC_OUT, Easing::QUADRATIC_IN, m_squidgingTimer, scale, scale*0.9f, m_squidgingDuration);
		scaleY = Easing::easebetweenback(Easing::QUADRATIC_OUT, Easing::QUADRATIC_IN, m_squidgingTimer, scale, scale*1.15f, m_squidgingDuration);
	}

	Image* img = m_animation->getCurrentFrame();

	if (m_velocity->getX() < 0) { img->flip(true,false); }

	float hopY = 0;
	if (m_hopping) {
		hopY = Easing::easebetweenback(Easing::QUADRATIC_OUT, Easing::QUADRATIC_IN, m_enemyHopTimer, 0.0f, m_enemyHopHeight, m_enemyHopDuration);
	}

	img->setRotation(0);
	//img->drawCenteredScaled(m_bounds->getCenterX(), m_bounds->getCenterY() - hopY - m_z, scaleX, scaleY);
	img->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY() - hopY - m_z, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, scaleX, scaleY);

	if (m_velocity->getX() < 0) { img->flip(true,false); }

	if (dg->stateUpgrades->hasHealthVision()) {

		r->setDrawColor("#cb2347", 1.0f);
		int healthRemaining = m_hitHealth - m_hitTimes;
		if (m_hitHealth > 1) {
			float numBigPips = floor(healthRemaining / 4);
			float numSmallPips = healthRemaining % 4;
			float numPipsTotal = numBigPips + numSmallPips;
			float width = (numBigPips * 4) + (numSmallPips*2) + (numPipsTotal-1);

			float curX = m_bounds->getCenterX() - (width * 0.5f)+1;
			float curY = m_bounds->getMaxY() - img->getHeight() - 4 - hopY - m_z;
			for(int i = 0; i < numBigPips; i++) {
				s_uiPipBig->drawCentered(curX, curY);
				curX += 5;
			}
			for(int i = 0; i < numSmallPips; i++) {
				s_uiPipSmall->drawCentered(curX, curY);
				curX += 3;
			}
		}

		r->setDrawColor(Color::white);

		// float startX = m_bounds->getCenterX() - 5;
		// float startY = m_bounds->getMinY() - 7 - hopY - m_z;

		// float healthPercentage = 1.0f - (m_hitTimes / m_hitHealth);
		// r->setDrawColor("#cb2347",1.0f);
		// r->fillRect(startX,startY, 10.0f * healthPercentage, 3.0f);

		// r->setDrawColor(Color::white);
		// r->setLineWidth(1);
		// r->drawRect(startX,startY, 10.0f, 3.0f);
	}

	/*
	if (m_type == ET_SANTA) {

		s_santaSleigh->drawAligned(m_bounds->getCenterX()-2, m_bounds->getMaxY()+5, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM);

		if (m_santaAlive) {
			Image* img = m_animation->getCurrentFrame();
			img->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY(), Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM);
		}
	}
	else if (m_type == ET_MOLE) {
		Image* img = m_animation->getCurrentFrame();
		img->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY(), Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM);
	}
	else if (m_type == ET_MISTLETOE || m_type == ET_CHRISTMASPUDDING) {
		float scale = 1.0f;
		if (m_introTimer > 0.0f) {
            scale = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
		}
		else if (m_hitTimer > 0.0f) {
            scale = Easing::easebetween(Easing::QUADRATIC_OUT, m_hitTimer, 1.35f, 1.0f, m_hitDuration);
		}

		Image* img = m_animation->getCurrentFrame();

		if (m_type == ET_CHRISTMASPUDDING) {
			img->setRotation(m_puddingRotation);
		}

		img->drawCenteredScaled(m_bounds->getCenterX(), m_bounds->getCenterY(), scale, scale);
	}
	*/

	if (DefaultGame::s_debug) {
		r->setDrawColor(Color::white);
		dg->font->drawString(StringUtil::appendf("", m_speed), m_bounds->getCenterX(), m_bounds->getMinY() - 1, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, 0.0f, 0.5f);
		//dg->font->drawString(StringUtil::appendf("", m_velocity->), m_bounds->getCenterX(), m_bounds->getMinY() - 1, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, 0.0f, 0.5f);

		r->setDrawColor(Color::cyan);
		float lineAngleRad = MathUtil::toRadians(m_velocity->angle());
		r->drawLine(m_bounds->getCenterX(), m_bounds->getCenterY(), m_bounds->getCenterX() + (5*cos(lineAngleRad)), m_bounds->getCenterY() + (5*sin(lineAngleRad)));
	}



}

Enemy::~Enemy() {

}
