
#include "SummaryState.h"
#include "../DefaultGame.h"
#include "../objects/Player.h"

#include <ARK2D/Util/CameraShake.h>

SummaryState::SummaryState():
    GameState()
{
}

unsigned int SummaryState::id() {
	return StatesList::STATE_SUMMARY;
}

void SummaryState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {
	m_outroTimer = 0.0f;
	m_introTimer = 0.01f;

	DefaultGame* dg = DefaultGame::getInstance();
    if (dg->m_musicCurrent != NULL) {
        if (!dg->m_musicCurrent->isPlaying()) {
            dg->m_musicCurrent->play();
        }
        if (dg->m_musicCurrent->isPlaying()) {
            dg->m_musicCurrent->setLowpass(true);
            dg->m_musicCurrent->setLowpassFrequency(500.0f);
        }
    }

    dg->m_save->set("game_complete", true);
    dg->m_stats->submit("Game Complete", 1);

    if (m_finalWin) {
        if (dg->m_musicCurrent != NULL) {
            dg->m_musicCurrent->stop();
        }
		dg->m_sfxWin->play();
		//m_santaDuration = 7.0f;
	}
}
void SummaryState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {
}

void summaryEvent_retry() {
    DefaultGame* dg = DefaultGame::getInstance();
    if (dg->stateSummary->m_introTimer > 0.0f || dg->stateSummary->m_outroTimer > 0.0f) { return; }

    dg->stateInGame->reset();
	dg->stateInGame->start();
    dg->stateSummary->m_outroTo = dg->stateInGame;
	dg->stateSummary->m_outroTimer = 0.01f;

    dg->m_sfxMenuSelect->play();
}
void summaryEvent_menu() {
    DefaultGame* dg = DefaultGame::getInstance();
    if (dg->stateSummary->m_introTimer > 0.0f ||
    	dg->stateSummary->m_outroTimer > 0.0f) { return; }

    dg->stateSummary->m_outroTo = dg->stateCredits;
	dg->stateSummary->m_outroTimer = 0.01f;

    dg->m_sfxMenuSelect->play();
}
void SummaryState::init(GameContainer* container, StateBasedGame* game) {
	DefaultGame* dg = DefaultGame::getInstance();

	iconNext = SpriteSheetStore::getImage("sprites/ui-icon-play.png");

	m_buttonNext = new ARK::UI::Button();
	m_buttonNext->setLocation(0, 0);
	m_buttonNext->setSize(24, 24);
	m_buttonNext->setEvent((void*) &summaryEvent_menu);
	m_buttonNext->setLocationByCenter(container->getWidth()-20, container->getHeight()-20);

	m_introTimer = 0.01f;
	m_introDuration = 0.5f;

	m_outroTimer = 0.0f;
	m_outroDuration = 0.5f;;
	m_outroTo = NULL;

	m_finalWin = false;

	m_timer = 0.0f;

	m_cameraShake = new CameraShake(game, 3);


	AnimationFrameList* idleFrames = AnimationFrameList::create("sprites/player/player-idle-*.png", 1, 10, dg->spritesheet, dg->desc);
	//AnimationFrameList* runFrames = AnimationFrameList::create("sprites/player/player-walk-*.png", 1, 12, dg->spritesheet, dg->desc);
	m_playerAnimation = new Animation();
	m_playerAnimation->setFrames(idleFrames);
	m_playerAnimation->setFrameTime(0.06f);



}

void SummaryState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {

	DefaultGame* dg = DefaultGame::getInstance();
	Input* in = ARK2D::getInput();
	if (in->isKeyPressed(Input::KEY_ENTER) ||
        in->isGamepadButtonPressed(Gamepad::BUTTON_START) ||
        in->isGamepadButtonPressed(Gamepad::BUTTON_A)) {
		//dg->enterState(dg->stateMenu);
		m_buttonNext->doEvent();
	}

	if (!m_finalWin) {
        if (dg->m_musicCurrent != NULL) {
            if (!dg->m_musicCurrent->isPlaying()) {
                dg->m_musicCurrent->play();
                dg->m_musicCurrent->setLowpass(true);
            }
            dg->m_musicCurrent->setLowpassFrequency(500.0f);
        }
	}

	m_playerAnimation->update(timer);

	m_timer += timer->getDelta();

	if (m_introTimer > 0.0f) {

		m_introTimer += timer->getDelta();
		if (m_introTimer > m_introDuration) {
			m_introTimer = 0.0f;
		}
	}
	if (m_outroTimer > 0.0f) {
		m_outroTimer += timer->getDelta();
		if (m_outroTimer > m_outroDuration) {
			dg->enterState(m_outroTo);//,
			//dg->enterState(m_outroTo,
              //             NULL,
				//new FadeToColourTransition(0.25f, new Color(*Tile::s_bgColor)),
			//	new FadeFromColourTransition(0.5f, new Color(*Color::black.constpointer()))
			//);
			return;
		}
	}



	if (in->isKeyPressed(Input::KEY_R)) {
		m_introTimer = 0.01f;


	}

	m_cameraShake->update(container, timer);

	m_buttonNext->update();


}

float SummaryState::getSantaX() {
    return 0; //Easing::easebetween(Easing::QUADRATIC_IN_OUT, m_santaTimer, -20, ARK2D::getContainer()->getWidth()+40, m_santaDuration);
}

void SummaryState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {

	DefaultGame* dg = DefaultGame::getInstance();

	float buttonsUIY = 50;

	float baseY = 0;
	float inGameY = container->getHeight();
	float mult = 1.0f;
	if (m_introTimer > 0.0f) {
		//inGameY = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0, container->getHeight(), m_introDuration);
		baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, container->getHeight(), 0, m_introDuration);
		mult = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}
	else if (m_outroTimer > 0.0f) {
		baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 0, container->getHeight() * -1.0f, m_outroDuration);
		mult = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 1.0f, 0.0f, m_outroDuration);
	}

	//r->pushMatrix();
	//r->translate(0.0f, inGameY);
		//dg->stateInGame->render(container, game, r);
	//r->popMatrix();

	//r->setDrawColor("#a60e49", mult);
	//r->fillRect(0, inGameY - container->getHeight() , container->getWidth(), container->getHeight());

	float spinClosedAmount = 1.0f;
	dg->stateMenu->drawSpinBG(spinClosedAmount);
	dg->stateMenu->drawDitherGradient(spinClosedAmount);

	r->setDrawColor(Color::white);
	float playerX = container->getWidth()*0.5f;
	float playerY = baseY + (container->getHeight()*0.5f);
	//r->fillCircleSunrays(playerX, container->getHeight()*0.5f, 1.0f*mult, 300, 6, m_timer * 30.0f);

    Image* img = m_playerAnimation->getCurrentFrame();
	r->setDrawColor(Color::black);
	Player::s_shadow->drawAligned(playerX, playerY + img->getHeight(), Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 2.0f, 2.0f);

	r->setDrawColor(Color::white);

	img->drawAligned(playerX, playerY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 2.0f, 2.0f);



	ThreeSliceButton::draw(m_buttonNext->transform.position.x, baseY+m_buttonNext->transform.position.y, m_buttonNext->getWidth());
	iconNext->drawCentered(m_buttonNext->transform.position.x, baseY+m_buttonNext->transform.position.y);


	//r->pushMatrix();
	//r->translate(m_cameraShake->getXOffset(), m_cameraShake->getYOffset());

	//r->setDrawColor(Color::white);

	r->setDrawColor(Color::white);
	dg->font->drawString("CONGRATULATIONS!", (container->getWidth()*0.5f)- 2, baseY + (container->getHeight()*0.25f) - 10, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 2.0f);
	dg->font->drawString("YOU ESCAPED THE DEEP HEAT!", (container->getWidth()*0.5f)- 2, baseY + (container->getHeight()*0.25f) + 10, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.75f);
	dg->font->drawString("THANKS FOR PLAYING!", (container->getWidth()*0.5f)- 2, baseY + (container->getHeight()*0.75f)+5, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);

	r->setDrawColor(Color::black, 1.0f - mult);


}

bool SummaryState::keyPressed(unsigned int key) {
	m_buttonNext->keyPressed(key);
	return false;
}
bool SummaryState::keyReleased(unsigned int key) {
	m_buttonNext->keyReleased(key);
	return false;
}
bool SummaryState::mouseMoved(int x, int y, int oldx, int oldy) {
	m_buttonNext->mouseMoved(x, y, oldx, oldy);
	return false;
}

SummaryState::~SummaryState() {

}

