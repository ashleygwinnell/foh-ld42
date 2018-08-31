
#include "MenuState.h"
#include "../DefaultGame.h"

MenuState::MenuState():
	GameState() {

}

unsigned int MenuState::id() {
	return StatesList::STATE_MENU;
}

void MenuState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {
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
}

void MenuState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {
	DefaultGame* dg = DefaultGame::getInstance();
	// if (dg->m_musicCurrent != NULL) {
	// 	if (dg->m_musicCurrent->isPlaying()){// && m_outroTo == dg->stateInGame) {
	// 		dg->m_musicCurrent->setLowpass(true);
	// 		dg->m_musicCurrent->setLowpassFrequency(22100.0f);
	// 	}
	// }

}
void menuEvent_help() {
	//SystemUtil::openBrowserToURL("http://twitter.com/forcehabit");

	DefaultGame* dg = DefaultGame::getInstance();

    if (dg->stateMenu->m_introTimer > 0.0f || dg->stateMenu->m_outroTimer > 0.0f) { return; }

	dg->stateMenu->m_outroTo = dg->stateCredits;
	dg->stateMenu->m_outroTimer = 0.01f;

    dg->m_sfxMenuSelect->play();

}
void menuEvent_settings() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();
	if (dg->stateMenu->m_settingsClosingTimer == 0.0f && dg->stateMenu->m_settingsOpeningTimer == 0.0f) {
		if (dg->stateMenu->m_settingsOpen) {
			dg->stateMenu->m_settingsClosingTimer = 0.01f;
		}
		else{
			dg->stateMenu->m_settingsOpeningTimer = 0.01f;
		}
	}
}
void menuEvent_fullscreen() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();

	ARK2D::getContainer()->setFullscreen(!ARK2D::getContainer()->isFullscreen());
}
void menuEvent_musicToggle() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();
	dg->m_musicVolume = (dg->m_musicVolume == 1.0f)?0.0f:1.0f;
	dg->updateAudioVolumes();
}
void menuEvent_sfxToggle() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();
	dg->m_sfxVolume = (dg->m_sfxVolume == 1.0f)?0.0f:1.0f;
	dg->updateAudioVolumes();
}
void menuEvent_play () {
    DefaultGame* dg = DefaultGame::getInstance();

    if (dg->stateMenu->m_introTimer > 0.0f || dg->stateMenu->m_outroTimer > 0.0f) { return; }

	dg->stateMenu->m_outroTo = dg->stateInGame;
	dg->stateMenu->m_outroTimer = 0.01f;

    dg->m_sfxMenuSelect->play();

    dg->stateInGame->reset();
    dg->stateInGame->start();
}
void menuEvent_upgrades() {
	DefaultGame* dg = DefaultGame::getInstance();
	if (dg->stateMenu->m_introTimer > 0.0f || dg->stateMenu->m_outroTimer > 0.0f) { return; }

	dg->stateMenu->m_outroTo = dg->stateUpgrades;
	dg->stateMenu->m_outroTimer = 0.01f;

	dg->m_sfxMenuSelect->play();
}

void MenuState::init(GameContainer* container, StateBasedGame* game) {
	DefaultGame* dg = DefaultGame::getInstance();
	m_selectedIndex = 0;

	m_buttonPlay = new ARK::UI::Button();
	m_buttonPlay->setLocation(0, 0);
	m_buttonPlay->setSize(50, 24);
	m_buttonPlay->setEvent((void*) &menuEvent_play);
	m_buttonPlay->setLocationByCenter(container->getWidth()*0.5, container->getHeight()*0.625f);

	m_buttonUpgrades = new ARK::UI::Button();
	m_buttonUpgrades->setLocation(0, 0);
	m_buttonUpgrades->setSize(70, 24);
	m_buttonUpgrades->setEvent((void*) &menuEvent_upgrades);
	m_buttonUpgrades->setLocationByCenter(container->getWidth()*0.5, container->getHeight()*0.8f);

	m_buttonFOH = new ARK::UI::Button();
	m_buttonFOH->setLocation(0, 0);
	m_buttonFOH->setSize(24, 24);
	m_buttonFOH->setEvent((void*) &menuEvent_help);
	m_buttonFOH->setLocationByCenter(container->getWidth() - 16, container->getHeight() - 16);


	m_buttonSettings = new ARK::UI::Button();
	m_buttonSettings->setLocation(0, 0);
	m_buttonSettings->setSize(20, 20);
	m_buttonSettings->setEvent((void*) &menuEvent_settings);
	m_buttonSettings->setLocationByCenter(14, container->getHeight() - 14);

	m_buttonFullscreen = new ARK::UI::Button();
	m_buttonFullscreen->setLocation(0, 0);
	m_buttonFullscreen->setSize(20, 20);
	m_buttonFullscreen->setEvent((void*) &menuEvent_fullscreen);
	m_buttonFullscreen->setLocationByCenter(14, container->getHeight() - 14 - 22 - 22 - 22);

	m_buttonMusic = new ARK::UI::Button();
	m_buttonMusic->setLocation(0, 0);
	m_buttonMusic->setSize(20, 20);
	m_buttonMusic->setEvent((void*) &menuEvent_musicToggle);
	m_buttonMusic->setLocationByCenter(14, container->getHeight() - 14 - 22 - 22);

	m_buttonSFX = new ARK::UI::Button();
	m_buttonSFX->setLocation(0, 0);
	m_buttonSFX->setSize(20, 20);
	m_buttonSFX->setEvent((void*) &menuEvent_sfxToggle);
	m_buttonSFX->setLocationByCenter(14, container->getHeight() - 14 - 22);

	m_settingsOpen = false;
	m_settingsOpeningTimer = 0.0f;
	m_settingsClosingTimer = 0.0f;
	m_settingsDuration = 0.25f;

	imageGradient = SpriteSheetStore::getImage("sprites/gradient-white.png");
	imageGradient->flip(false, true);
	imageGradientOffsetX = 0;
	imageRedPurpleRotation = -45;
	iconTwitter = SpriteSheetStore::getImage("sprites/ui-icon-twitter.png");
	iconFullscreen = SpriteSheetStore::getImage("sprites/ui-icon-fullscreen.png");
	iconHelp = SpriteSheetStore::getImage("sprites/ui-icon-help.png");
	iconMusicOn = SpriteSheetStore::getImage("sprites/ui-icon-music.png");
	iconMusicOff = SpriteSheetStore::getImage("sprites/ui-icon-music-off.png");
	iconSFXOn = SpriteSheetStore::getImage("sprites/ui-icon-sound.png");
	iconSFXOff = SpriteSheetStore::getImage("sprites/ui-icon-sound-off.png");
	iconPlay = SpriteSheetStore::getImage("sprites/ui-icon-play.png");
	iconUpgrades = SpriteSheetStore::getImage("sprites/ui-icon-upgrades.png");
	iconSettings = SpriteSheetStore::getImage("sprites/ui-icon-settings.png");




	m_introTimer = 0.01f;
	m_introDuration = 0.5f;

	m_outroTimer = 0.0f;
	m_outroDuration = 0.5f;;
	m_outroTo = NULL;

	juiceTimer = 0.0f;
	juiceDuration = 2.5f;
}

float MenuState::getSettingsButtonsX() {
	float defaultX = -14.0f;
	if (m_settingsOpeningTimer > 0.0f) {
		defaultX = Easing::easebetween(Easing::QUADRATIC_OUT, m_settingsOpeningTimer, -14, 14, m_settingsDuration);
	}
	else if (m_settingsClosingTimer > 0.0f) {
		defaultX = Easing::easebetween(Easing::QUADRATIC_OUT, m_settingsClosingTimer, 14, -14, m_settingsDuration);
	}
	else if (m_settingsOpen) {
		defaultX = 14;
	}
	return defaultX;
}

void MenuState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {
	DefaultGame* dg = DefaultGame::getInstance();
	Input* i = ARK2D::getInput();

	if (dg->m_musicCurrent != NULL) {
		if (!dg->m_musicCurrent->isPlaying()) {
			dg->m_musicCurrent->play();
			dg->m_musicCurrent->setLowpass(true);
		}
		dg->m_musicCurrent->setLowpassFrequency(500.0f);
	}


	if (m_settingsOpeningTimer > 0.0f) {
		m_settingsOpeningTimer += timer->getDelta();
		if (m_settingsOpeningTimer >= m_settingsDuration) {
			m_settingsOpeningTimer = 0.0f;
			m_settingsOpen = true;
		}
	}
	if (m_settingsClosingTimer > 0.0f) {
		m_settingsClosingTimer += timer->getDelta();
		if (m_settingsClosingTimer >= m_settingsDuration) {
			m_settingsClosingTimer = 0.0f;
			m_settingsOpen = false;
		}
	}

	float settingsX = getSettingsButtonsX();
	m_buttonSFX->setLocationByCenter(settingsX, m_buttonSFX->getY());
	m_buttonMusic->setLocationByCenter(settingsX, m_buttonMusic->getY());
	m_buttonFullscreen->setLocationByCenter(settingsX, m_buttonFullscreen->getY());

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer > m_introDuration) {
			m_introTimer = 0.0f;
		}
	}
	if (m_outroTimer > 0.0f) {
		m_outroTimer += timer->getDelta();
		if (m_outroTo == dg->stateInGame) {
			dg->stateInGame->update(container,game,timer);
		}
		if (m_outroTimer > m_outroDuration) {
			if (m_outroTo == dg->stateInGame) {
				//dg->stateInGame->reset();
				//dg->stateInGame->start();
			}
			dg->enterState(m_outroTo);
			return;
		}
	}

	if (i->isKeyPressed(Input::KEY_R)) {
		m_introTimer = 0.01f;
	}

	if (i->isKeyPressed(Input::KEY_S)) {
		game->enterState(dg->stateSplash);
	}
	if (i->isKeyPressed(Input::KEY_C)) {
		game->enterState(dg->stateSummary);
	}
	if (i->isKeyPressed(Input::KEY_U)) {
		game->enterState(dg->stateUpgrades);
	}

	m_buttonPlay->update();
	m_buttonUpgrades->update();
	m_buttonFOH->update();
	m_buttonFullscreen->update();
	m_buttonMusic->update();
	m_buttonSFX->update();

	if (i->isKeyPressed(Input::KEY_ENTER) ||
		i->isGamepadButtonPressed(Gamepad::BUTTON_START) ||
		i->isGamepadButtonPressed(Gamepad::BUTTON_A)) {
		m_buttonPlay->doEvent();
		//if (m_selectedIndex == MENU_PLAY) {



		//}
		// else if (m_selectedIndex == MENU_INSTRUCTIONS) {
		// 	game->enterState(dg->stateInstructions);
		// }
		// else if (m_selectedIndex == MENU_EXIT) {
		// 	container->close();
		// }
	}

	juiceTimer += timer->getDelta();
	if (juiceTimer >= juiceDuration) {
		juiceTimer -= juiceDuration;
	}
}

void MenuState::drawSpinBG(float percentageClosed) {
	//float percentageClosed = 1.0f;
	Renderer* r = ARK2D::getRenderer();
    GameContainer* container = ARK2D::getContainer();
	float halfWidth = container->getWidth()*0.5f;
	float halfHeight = container->getHeight()*0.5f;
	float rot = imageRedPurpleRotation;
	r->pushMatrix();
	r->translate(halfWidth, halfHeight);
	r->rotate(rot);
	halfWidth *= 1.5f;
	halfHeight *= 1.5f;
	r->setDrawColor("#4e1353", 1.0f);
	r->fillRect(-halfWidth, -2*halfHeight, 2*halfWidth, 2*halfHeight*percentageClosed);
	r->setDrawColor("#a60e49", 1.0f);
	r->fillRect(-halfWidth, 0 + (2*halfHeight*(1.0-percentageClosed)), 2*halfWidth, 2*halfHeight);
	r->popMatrix();
}

void MenuState::drawDitherGradient(float on) {
    Renderer* r = ARK2D::getRenderer();
    GameContainer* container = ARK2D::getContainer();
	float h = imageGradient->getHeight();
	float y = (1.0f - on) * h;
	r->setDrawColor(Color::black);
	imageGradient->draw(imageGradientOffsetX*-1, -y);
	imageGradient->draw((imageGradientOffsetX*-1) + imageGradient->getWidth(), -y);
	imageGradient->drawFlipped(imageGradientOffsetX, container->getHeight() - imageGradient->getHeight() + y, false, true);
	imageGradient->drawFlipped(imageGradientOffsetX - imageGradient->getWidth(), container->getHeight() - imageGradient->getHeight() + y, false, true);
}

void MenuState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {
	DefaultGame* dg = DefaultGame::getInstance();

	float widthMultiplier = Easing::easebetweenback(Easing::QUADRATIC_IN_OUT, Easing::QUADRATIC_IN_OUT, juiceTimer, 0.85f, 1.00f, juiceDuration);


	r->setDrawColor("#4e1353", 1.0f);
	r->fillRect(0,0,container->getWidth(), container->getHeight());

	//dg->fillPattern(1.0f, angle);

	float spinClosedAmount = 1.0f;
	if (m_outroTimer > 0.0f && m_outroTo == dg->stateInGame) {
		spinClosedAmount = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 1.0f, 0.0f, m_outroDuration);

		dg->stateInGame->render(container, game, r);
	}
	drawSpinBG(spinClosedAmount);
	drawDitherGradient(spinClosedAmount);

	float baseY = 0;
	float baseYBottom = 0;
	float baseYTop = 0;
	if (m_introTimer > 0.0f) {
		baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, container->getHeight(), 0, m_introDuration);
		baseYTop = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, -30, 0, m_introDuration);
		baseYBottom = baseY;
	}
	else if (m_outroTimer > 0.0f) {
		baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 0, container->getHeight() * -1.0f, m_outroDuration);
		baseYTop = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 0, -30, m_outroDuration);
		baseYBottom = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 0, 50, m_outroDuration);
	}

	r->setDrawColor(Color::white);
	dg->font->drawString("DEEP", (container->getWidth()*0.5f)-2, baseY + (container->getHeight()*0.25f), Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 3.0f);
	dg->font->drawString("HEAT", (container->getWidth()*0.5f)-2, baseY + (container->getHeight()*0.45f), Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 3.0f);

	if (dg->m_coins > 0 || dg->stateUpgrades->hasAnyUpgrade()) {
		m_buttonPlay->setLocationByCenter(container->getWidth()*0.5, container->getHeight()*0.625f);
		m_buttonUpgrades->setLocationByCenter(container->getWidth()*0.5, container->getHeight()*0.8f);
	} else {
		m_buttonPlay->setLocationByCenter(container->getWidth()*0.5, container->getHeight()*0.7125f);
	}

	ThreeSliceButton::draw(m_buttonPlay->transform.position.x, baseY+m_buttonPlay->transform.position.y, m_buttonPlay->getWidth());
	//dg->font->drawString("PLAY", m_buttonPlay->transform.position.x - 1.5f, baseY+m_buttonPlay->transform.position.y + 2.0f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);
	//ThreeSliceButton::draw(m_buttonPlay->transform.position.x, baseY+m_buttonPlay->transform.position.y, 24);
	iconPlay->drawCentered(m_buttonPlay->transform.position.x, baseY+m_buttonPlay->transform.position.y);

	if (dg->m_coins > 0) {
		ThreeSliceButton::draw(m_buttonUpgrades->transform.position.x, baseY+m_buttonUpgrades->transform.position.y, m_buttonUpgrades->getWidth());
		//dg->font->drawString("UPGRADES", m_buttonUpgrades->transform.position.x - 1.5f, baseY+m_buttonUpgrades->transform.position.y + 2.0f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);
		//ThreeSliceButton::draw(m_buttonUpgrades->transform.position.x, baseY+m_buttonUpgrades->transform.position.y, 24);
		iconUpgrades->drawCentered(m_buttonUpgrades->transform.position.x, baseY+m_buttonUpgrades->transform.position.y);
	}

	ThreeSliceButton::draw(m_buttonFOH->transform.position.x, baseYBottom + m_buttonFOH->transform.position.y, m_buttonFOH->getWidth());
	iconHelp->drawCentered(m_buttonFOH->transform.position.x, baseYBottom + m_buttonFOH->transform.position.y);

	dg->m_cornerBox->drawCentered(m_buttonFullscreen->transform.position.x, baseYBottom + m_buttonFullscreen->transform.position.y);
	iconFullscreen->drawCentered(m_buttonFullscreen->transform.position.x, baseYBottom + m_buttonFullscreen->transform.position.y);

	Image* musicIcon = (dg->m_musicVolume == 1.0f)?iconMusicOn:iconMusicOff;
	dg->m_cornerBox->drawCentered(m_buttonMusic->transform.position.x, baseYBottom + m_buttonMusic->transform.position.y);
	musicIcon->drawCentered(m_buttonMusic->transform.position.x, baseYBottom + m_buttonMusic->transform.position.y);

	Image* sfxIcon = (dg->m_sfxVolume == 1.0f)?iconSFXOn:iconSFXOff;
	dg->m_cornerBox->drawCentered(m_buttonSFX->transform.position.x, baseYBottom + m_buttonSFX->transform.position.y);
	sfxIcon->drawCentered(m_buttonSFX->transform.position.x, baseYBottom + m_buttonSFX->transform.position.y);

	dg->m_cornerBox->drawCentered(m_buttonSettings->transform.position.x, baseYBottom + m_buttonSettings->transform.position.y);
	iconSettings->drawCentered(m_buttonSettings->transform.position.x, baseYBottom + m_buttonSettings->transform.position.y);

	dg->font->drawString("(C) FORCE OF HABIT", container->getWidth()*0.5f, baseYBottom + container->getHeight()-6, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
}


bool MenuState::keyPressed(unsigned int key) {
	m_buttonPlay->keyPressed(key);
	m_buttonUpgrades->keyPressed(key);
	m_buttonFOH->keyPressed(key);
	m_buttonFullscreen->keyPressed(key);
	m_buttonMusic->keyPressed(key);
	m_buttonSFX->keyPressed(key);
	m_buttonSettings->keyPressed(key);
	return false;
}
bool MenuState::keyReleased(unsigned int key) {
	m_buttonPlay->keyReleased(key);
	m_buttonUpgrades->keyReleased(key);
	m_buttonFOH->keyReleased(key);
	m_buttonFullscreen->keyReleased(key);
	m_buttonMusic->keyReleased(key);
	m_buttonSFX->keyReleased(key);
	m_buttonSettings->keyReleased(key);
	return false;
}
bool MenuState::mouseMoved(int x, int y, int oldx, int oldy) {
	m_buttonPlay->mouseMoved(x, y, oldx, oldy);
	m_buttonUpgrades->mouseMoved(x, y, oldx, oldy);
	m_buttonFOH->mouseMoved(x, y, oldx, oldy);
	m_buttonFullscreen->mouseMoved(x, y, oldx, oldy);
	m_buttonMusic->mouseMoved(x, y, oldx, oldy);
	m_buttonSFX->mouseMoved(x, y, oldx, oldy);
	m_buttonSettings->mouseMoved(x, y, oldx, oldy);
	return false;
}

MenuState::~MenuState() {

}
