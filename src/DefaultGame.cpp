
#include "DefaultGame.h"
#include "objects/Player.h"




Image* ThreeSliceButton::s_buttonBackgroundLeft = NULL;
Image* ThreeSliceButton::s_buttonBackgroundMiddle = NULL;
Image* ThreeSliceButton::s_buttonBackgroundRight = NULL;

Image* ThreeSliceButton::s_buttonBorderLeft = NULL;
Image* ThreeSliceButton::s_buttonBorderMiddle = NULL;
Image* ThreeSliceButton::s_buttonBorderRight = NULL;

void ThreeSliceButton::init() {
	s_buttonBackgroundLeft = SpriteSheetStore::getImage("sprites/ui-button-3s-left.png");
	s_buttonBackgroundRight = SpriteSheetStore::getImage("sprites/ui-button-3s-right.png");
	s_buttonBackgroundMiddle = SpriteSheetStore::getImage("sprites/ui-button-3s-middle-bg.png");

	s_buttonBorderLeft = SpriteSheetStore::getImage("sprites/ui-button-3s-left.png");
	s_buttonBorderRight = SpriteSheetStore::getImage("sprites/ui-button-3s-right.png");
	s_buttonBorderMiddle = SpriteSheetStore::getImage("sprites/ui-button-3s-middle.png");
}

void ThreeSliceButton::draw(int x, int y, int width) {
	draw(x, y, width, Color::black.asInt());
}
void ThreeSliceButton::draw(int x, int y, int width, uint32_t bgcolor) {

    Renderer* r = ARK2D::getRenderer();

    r->setDrawColor(bgcolor);
	//s_buttonBackgroundLeft->drawCentered(x - (width/2), y);
	//s_buttonBackgroundRight->drawCentered(x + (width/2), y);
	s_buttonBackgroundMiddle->drawCenteredScaled(x, y, width - 4, 1.0f);

	r->setDrawColor(Color::white);
	s_buttonBorderLeft->drawAligned(x - (width/2), y, -1, 0);
	s_buttonBorderRight->drawAligned(x + (width/2), y, 1, 0);
	s_buttonBorderMiddle->drawCenteredScaled(x, y, width - 4, 1.0f);
}


Image* MyTextBubble::s_buttonBackgroundLeft = NULL;
Image* MyTextBubble::s_buttonBackgroundMiddle = NULL;
Image* MyTextBubble::s_buttonBackgroundRight = NULL;
Image* MyTextBubble::s_buttonBackgroundArrow = NULL;
void MyTextBubble::init() {
	s_buttonBackgroundLeft = SpriteSheetStore::getImage("sprites/speechbub-left.png");
	s_buttonBackgroundMiddle = SpriteSheetStore::getImage("sprites/speechbub-middle.png");
	s_buttonBackgroundRight = SpriteSheetStore::getImage("sprites/speechbub-right.png");
	s_buttonBackgroundArrow = SpriteSheetStore::getImage("sprites/speechbub-arrow.png");
}
void MyTextBubble::draw(string text, int x, int y, int width) {
	Renderer* r = ARK2D::getRenderer();
	s_buttonBackgroundLeft->drawAligned(x - (width/2), y, Renderer::ALIGN_RIGHT, 0);
	s_buttonBackgroundRight->drawAligned(x + (width/2), y, Renderer::ALIGN_LEFT, 0);
	s_buttonBackgroundMiddle->drawCenteredScaled(x, y, width, 1.0f);
	s_buttonBackgroundArrow->drawAligned(x, y + (s_buttonBackgroundRight->getHeight()*0.5), 0, -1);

	// draw text
	DefaultGame::getInstance()->font->drawString(text, x - 1, y+1, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.75f);
}


bool DefaultGame::s_debug = false;

DefaultGame::DefaultGame(string title):
	StateBasedGame(title),
	m_coins(0),
	m_musicMenu(NULL),
    m_musicGame(NULL),
    m_musicShop(NULL),
    m_musicCurrent(NULL),
    m_explosions(),
    m_recorder(NULL)
    {

}

void DefaultGame::initStates(GameContainer* container) {

	m_recorder = new GIFRecorder("output.gif", 0, 0, container->getWidth(), container->getHeight());

	m_fmod = new ArkFMOD();
	m_fmod->init();

	//m_fmod->m_soundGroups



	// hsvShader = new HSVShader();
	// hsvShader->load();

	stateBlank = new BlankState();
	stateSplash = new SplashState();
	stateMenu = new MenuState();
	stateInstructions = new InstructionsState();
	stateInGame = new InGameState();
	stateSummary = new SummaryState();
	stateUpgrades = new UpgradesState();
	stateCredits = new CreditsState();

	Renderer::setInterpolation(Renderer::INTERPOLATION_NEAREST);
	spritesheet = Resource::get("all.png")->asImage();
	desc = Resource::get("all.spritesheetdescription")->asSpriteSheetDescription();
	SpriteSheetStore::addSheet(desc, spritesheet);

	ARK2D::getRenderer()->setInterpolation(Renderer::INTERPOLATION_NEAREST);
	font = Resource::get("fonts/SG12_small.fnt")->asFont()->asBMFont();
	font->setKerning(0);
	//font->scale(0.8f);
	ARK2D::getRenderer()->setDefaultFont(font);

	m_musicMenu = new ArkFMODSound(m_fmod, "music/menu.ogg", 0);
	m_musicGame = new ArkFMODSound(m_fmod, "music/action.ogg", 0);
	m_musicSummary = new ArkFMODSound(m_fmod, "music/gameover.ogg", 0);
	m_musicShop = m_musicSummary;
	m_musicCurrent = m_musicMenu;

	m_sfxFOH = new ArkFMODSound(m_fmod, "sfx/foh.wav", 1);
	m_sfxMenuSelect = new ArkFMODSound(m_fmod, "sfx/menu-select.wav", 1);
	m_sfxMenuBuy = new ArkFMODSound(m_fmod, "sfx/coin.wav", 1);
	m_sfxPlayerDie = new ArkFMODSound(m_fmod, "sfx/player-die.wav", 1);
	m_sfxPlayerHurt = new ArkFMODSound(m_fmod, "sfx/player-hurt.wav", 1);
	m_sfxLadder = new ArkFMODSound(m_fmod, "sfx/ladder.wav", 1);
	m_sfxWin = new ArkFMODSound(m_fmod, "sfx/game-win.wav", 1);

	m_sfxFireFlames = new ArkFMODSound(m_fmod, "sfx/fire-flames.wav", 1);
	m_sfxFireGas = new ArkFMODSound(m_fmod, "sfx/fire-gas.wav", 1);
	m_sfxEnemyDeathMini = new ArkFMODSound(m_fmod, "sfx/enemy-death-mini.wav", 1);
	m_sfxEnemyDeathSmall = new ArkFMODSound(m_fmod, "sfx/enemy-death-small.wav", 1);
	m_sfxEnemyDeathMedium = new ArkFMODSound(m_fmod, "sfx/enemy-death-medium.wav", 1);

	m_save = Resource::get("save.kpf")->asKeyPairFile();
	m_coins = m_save->getInteger("coins", 0);
	//m_save->getBoolean("playedCutscene_intro", false);

	m_musicVolume = m_save->getFloat("music_volume", 1.0f);
	m_sfxVolume = m_save->getFloat("sfx_volume", 1.0f);

	m_reticule = SpriteSheetStore::getImage("sprites/reticule.png");

	m_cornerBox = SpriteSheetStore::getImage("sprites/ui-corner-box.png");



	m_floor = container->getHeight() - 24;
	ThreeSliceButton::init();
	MyTextBubble::init();
	m_buttonbg = SpriteSheetStore::getImage("sprites/ui-button.png");
	m_gradient = SpriteSheetStore::getImage("sprites/gradient.png");
	m_gradientWhite = SpriteSheetStore::getImage("sprites/gradient-white.png");

	addState(stateBlank);
	addState(stateSplash);
	addState(stateMenu);
	addState(stateInstructions);
	addState(stateInGame);
	addState(stateSummary);
	addState(stateUpgrades);
	addState(stateCredits);


	//enterState(stateMenu);
	enterState(stateSplash);
	//container->setSize(284*4, 160*4);
	container->setSize(284*3, 160*3);

	container->getTimer()->flush();
	//enterState(stateInGame);
}

void DefaultGame::updateAudioVolumes(bool save) {
	m_musicMenu->setVolume(m_musicVolume);
	m_musicGame->setVolume(m_musicVolume);
	m_musicSummary->setVolume(m_musicVolume);
	m_musicCurrent->setVolume(m_musicVolume);
	m_musicShop->setVolume(m_musicVolume);

	for(int i = 0; i < ArkFMODSound::s_allSounds->size(); i++) {
		if (ArkFMODSound::s_allSounds->at(i)->m_group == 0) {
			ArkFMODSound::s_allSounds->at(i)->setVolume(m_musicVolume);
		}
		if (ArkFMODSound::s_allSounds->at(i)->m_group == 1) {
			ArkFMODSound::s_allSounds->at(i)->setVolume(m_sfxVolume);
		}
	}

	if (save) {
		m_save->set("music_volume", m_musicVolume);
		m_save->set("sfx_volume", m_sfxVolume);
		m_save->save();
	}
}

void DefaultGame::update(GameContainer* container, GameTimer* timer) {
	StateBasedGame::update(container, timer);

	Input* i = ARK2D::getInput();
	if (i->isGamepadButtonPressed(Gamepad::BUTTON_ACTIVATE) || i->isKeyPressed(Input::KEY_F)) {
		container->setFullscreen(!container->isFullscreen());
	}

	if (i->isKeyPressed(Input::KEY_3)) {
		container->setSize(284*3, 160*3);
	}

	if (i->isKeyPressed(Input::KEY_K)) {
		s_debug = !s_debug;
	}

	if (m_fmod != NULL) {
		if (i->isKeyPressed(Input::KEY_M)) {
			m_musicVolume = (m_musicVolume == 1.0f)?0.0f:1.0f;
			updateAudioVolumes();
		}
		updateAudioVolumes(false);
	}

	if (stateMenu != NULL) {
		stateMenu->imageGradientOffsetX += 10 * timer->getDelta();
		if (stateMenu->imageGradientOffsetX >= 4.0f) {
			stateMenu->imageGradientOffsetX -= 4.0f;
		}

		stateMenu->imageRedPurpleRotation += 45.0f * timer->getDelta();
		if (stateMenu->imageRedPurpleRotation >= 360.0f) {
			stateMenu->imageRedPurpleRotation -= 360.0f;
		}
	}

	if (i->isKeyPressed(Input::KEY_SPACE) && !m_recorder->isRecording()) {
        m_recorder->recordForLength(2.0f);
    }
    if (i->isKeyPressed(Input::KEY_0)) {
    	if (!m_recorder->isRecording()) {
    		m_recorder->record();
    	} else {
    		m_recorder->cut();
    		m_recorder->save();
    	}
    }
    m_recorder->update(timer->getDelta());

	m_fmod->update();
	if (getCurrentState() == stateInGame) {
		if (m_musicMenu->isPlaying()) { m_musicMenu->stop(); }
		if (m_musicSummary->isPlaying()) { m_musicSummary->stop(); }

        if (!m_musicGame->isPlaying() && !stateInGame->m_win) {
			m_musicCurrent = m_musicGame;
			m_musicGame->play();
		}

		if (stateInGame->m_win) {
			if (m_musicGame->isPlaying()) { m_musicGame->stop(); }
		}
	}
	else if (getCurrentState() == stateUpgrades) {
		if (m_musicMenu->isPlaying()) { m_musicMenu->stop(); }
		if (m_musicGame->isPlaying()) { m_musicGame->stop(); }

		if (!m_musicSummary->isPlaying()) {
			m_musicCurrent = m_musicSummary;

			if (!stateSummary->m_finalWin) {
				m_musicSummary->play();
				if (getCurrentState() == stateMenu) {
					m_musicSummary->setLowpass(true);
					m_musicSummary->setLowpassFrequency(500.0f);
				}
				else {
					m_musicSummary->setLowpass(true);
					m_musicSummary->setLowpassFrequency(22100.0f);
				}
			} else {
				if (m_musicSummary->isPlaying()) { m_musicSummary->stop(); }
				if (m_musicMenu->isPlaying()) {	m_musicMenu->stop(); }
				if (m_musicGame->isPlaying()) { m_musicGame->stop(); }
			}
		}
	}
	else if (getCurrentState() == stateMenu || getCurrentState() == stateCredits) {
		if (m_musicGame->isPlaying()) { m_musicGame->stop(); }
		if (m_musicSummary->isPlaying()) { m_musicSummary->stop(); }

		if (!m_musicMenu->isPlaying()) {
			m_musicCurrent = m_musicMenu;
			m_musicMenu->play();
			m_musicMenu->setLowpass(true);
			m_musicMenu->setLowpassFrequency(500.0f); //dg->m_musicGame->setLowpassFrequency(22100.0f);
		}
	}

	m_reticuleRotation += timer->getDelta() * 135.0f;


}

void DefaultGame::render(GameContainer* container, Renderer* r) {
	StateBasedGame::render(container, r);
}

void DefaultGame::renderBackground() {

}

void DefaultGame::render() {

	//hsvShader->set(1.0f, MathUtil::randBetweenf(0.8f,1.0f), 1.0f);
	// hsvShader->set(1.0f, 1.0f, 1.0f);
	// hsvShader->start();
	renderBackground();

	StateBasedGame::render();


	Renderer* r = ARK2D::getRenderer();

	r->setDrawColor(Color::white);
	Input* in = ARK2D::getInput();
	m_reticule->setRotation(m_reticuleRotation);
	m_reticule->drawCentered(in->getMouseX(), in->getMouseY());

	// hsvShader->stop();

	if (m_recorder != NULL) m_recorder->render();
}

void DefaultGame::resize(GameContainer* container, int width, int height) {
	StateBasedGame::resize(container, width, height);
	if (m_recorder != NULL) m_recorder->resize(0,0,width,height);
}

DefaultGame* DefaultGame::getInstance() {
    return dynamic_cast<DefaultGame*>(ARK2D::getGame());
}

DefaultGame::~DefaultGame() {

}
