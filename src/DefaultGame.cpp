
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

Image* ThreeSliceSmallerButton::s_buttonBackgroundLeft = NULL;
Image* ThreeSliceSmallerButton::s_buttonBackgroundMiddle = NULL;
Image* ThreeSliceSmallerButton::s_buttonBackgroundRight = NULL;

Image* ThreeSliceSmallerButton::s_buttonBorderLeft = NULL;
Image* ThreeSliceSmallerButton::s_buttonBorderMiddle = NULL;
Image* ThreeSliceSmallerButton::s_buttonBorderRight = NULL;

void ThreeSliceSmallerButton::init() {
	s_buttonBackgroundLeft = SpriteSheetStore::getImage("sprites/ui-button-smaller-3s-left.png");
	s_buttonBackgroundRight = SpriteSheetStore::getImage("sprites/ui-button-smaller-3s-right.png");
	s_buttonBackgroundMiddle = SpriteSheetStore::getImage("sprites/ui-button-smaller-3s-middle-bg.png");

	s_buttonBorderLeft = SpriteSheetStore::getImage("sprites/ui-button-smaller-3s-left.png");
	s_buttonBorderRight = SpriteSheetStore::getImage("sprites/ui-button-smaller-3s-right.png");
	s_buttonBorderMiddle = SpriteSheetStore::getImage("sprites/ui-button-smaller-3s-middle.png");
}
void ThreeSliceSmallerButton::draw(int x, int y, int width) {
	draw(x, y, width, Color::black.asInt());
}
void ThreeSliceSmallerButton::draw(int x, int y, int width, uint32_t bgcolor) {
    Renderer* r = ARK2D::getRenderer();
    r->setDrawColor(bgcolor);
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
	//DefaultGame::getInstance()->font->drawString(text, x - 1, y+1, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.75f);
	DefaultGame::getInstance()->font->asFont()->drawString(text, x - 1, y+1, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.75f);

}
extern "C" void EMSCRIPTEN_KEEPALIVE emscripten_html5helper_setKongUsername(char* href) {
	ARK2D::getLog()->e(StringUtil::append("setting kong username in c :", string(href)));
	DefaultGame::getInstance()->m_stats->loggedInAs = href;
	DefaultGame::getInstance()->m_stats->updateStats();


}

void StatsSubmit::init() {
	loggedInAs = "";
	#ifdef ARK2D_EMSCRIPTEN_JS
		//EM_ASM_({
		EM_ASM({

			if (window.location.href.indexOf("konggames.com") >= 0) {
				window.ark_loadJSAndThen("https://cdn1.kongregate.com/javascripts/kongregate_api.js", function(){
					kongregateAPI.loadAPI(function(){
						window.kongregate = kongregateAPI.getAPI();
						//kongregate.stats.submit('Score', 1000);
						//$('#username').text('Username: ' + kongregate.services.getUsername());
						var username = window.kongregate.services.getUsername();
						console.log(window.kongregate.services.getUsername());

						 Module.ccall('emscripten_html5helper_setKongUsername', // name of C function
		                        null, // void return type
		                        [ 'string' ], // argument types,
		                        [ username ] //, // arguments
		                        //{ async:true } // optional options
		                    );


					});
				});
			}
			else {
				console.error("STATS INIT");
			}



			//Pointer_stringify($0);
		});//, gameId, apiKey, guestAccessKey, guestAccessUrl);
	#endif
}
void StatsSubmit::submit(string name, int val) {
	EM_ASM_ARGS({
		var name = UTF8ToString($0);
		var val = $1;
		console.error("STATS SUBMIT: " + name + " = " + val);
		if (window.location.href.indexOf("konggames.com") >= 0) {
			if (!window.kongregate) {
				console.warn("cannot submit stat as kong not loaded yet.");
				return;
			}
			window.kongregate.stats.submit(name, val); // Replace / 136572
		}
	}, name.c_str(), val);
}
void StatsSubmit::updateStats() {
	// Update stats.
	DefaultGame* dg = DefaultGame::getInstance();

	bool completed = dg->m_save->getBoolean("game_complete", false);
	if (completed) {
		dg->m_stats->submit("Game Complete", 1);
	}
	bool viewedCredits = dg->m_save->getBoolean("viewed_credits", false);
	if (viewedCredits) {
		dg->m_stats->submit("View Credits", 1);
	}

	bool purchasedAnything = dg->m_save->getBoolean("purchasedAnything", false);
	if (purchasedAnything) {
		dg->m_stats->submit("Upgraded", 1);
	}

	if (dg->stateUpgrades->hasAllUpgrades()) {
		dg->m_stats->submit("All Upgrades", 1);
	}
}


bool DefaultGame::s_debug = false;
bool DefaultGame::s_armorGames = true;

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
    DefaultGame::s_armorGames = false;


}

void DefaultGame::initStates(GameContainer* container) {

	m_stats = new StatsSubmit();


	#ifdef ARK2D_EMSCRIPTEN_JS

		auto sitelock = [container](vector<string> sites, std::function<void()> onFail)->bool {
			string url = HTML5Helper::getWindowHref();
			ARK2D::getLog()->e(StringUtil::append("url: ", url));
			bool found = false;
			for(int i = 0; i < sites.size(); i++) {
				ARK2D::getLog()->e(string("comparing ") + sites[i] + string(" to ") + url);
				if (url.find(sites[i]) != string::npos) {
					found = true;
				}
			}
			if (!found) {
				onFail();
			}
			return !found;
		};

		vector<string> sitelockUrls = {
			// force of habit
			"localhost",
			"forceofhab.it",
			// newgrounds
			"newgrounds.com",
			"ungrounded.net",
			// kongregate
			"konggames.com",
			"*.konggames.com",
			// game jolt
			"gamejolt.com",
			"gamejolt.net",
			// itchio
			"itch.io",
			"hwcdn.net",
			// armor games
			"http://games.armorgames.com",
			"https://games.armorgames.com",
			"http://preview.armorgames.com",
			"https://preview.armorgames.com",
			"http://cache.armorgames.com",
			"https://cache.armorgames.com",
			"http://cdn.armorgames.com",
			"https://cdn.armorgames.com",
			"http://gamemedia.armorgames.com",
			"https://gamemedia.armorgames.com",
			"http://files.armorgames.com",
			"https://files.armorgames.com",
			"http://*.armorgames.com",
			"https://*.armorgames.com",
			// crazygames
			"crazygames.com",
			"1001juegos.com",
			"gioca.re",
			"speelspelletjes.nl",
			"onlinegame.co.id"
		};

		bool wrongUrl = sitelock(sitelockUrls, [container]()->void {
			ErrorDialog::createAndShow("Sitelocked!");
			//container->close();
		});
		if (wrongUrl) {
			stateBlank = new BlankState();
			addState(stateBlank);
			enterState(stateBlank);

			container->setSize(284*3, 160*3);

			return;
		}
	#endif

	m_recorder = new GIFRecorder("output.gif", 0, 0, container->getWidth(), container->getHeight());

	m_fmod = new ArkFMOD();
	m_fmod->init();

	//m_fmod->m_soundGroups




	hsvShader = new HSVShader();
	hsvShader->load();

	// grainShader = new GrainShader();
	// grainShader->load();
	// grainShader->resolution.set(1024,512);
	// grainShader->strength = 1;
	// grainShader->timer = 0;

	// grainFBO = new FBO(284*3, 160*3);

	stateBlank = new BlankState();
	stateSplash = new SplashState();
	stateMenu = new MenuState();
	stateInstructions = new InstructionsState();
	stateInGame = new InGameState();
	stateSummary = new SummaryState();
	stateUpgrades = new UpgradesState();
	stateCredits = new CreditsState();
	stateArmorGames = new ArmorGamesState();
	stateAutoplayFix = new AutoplayFixState();

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
	ThreeSliceSmallerButton::init();
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
	addState(stateArmorGames);
	addState(stateAutoplayFix);


	//enterState(stateMenu);
	if (s_armorGames) {
		enterState(stateArmorGames);
	}
	else {
		enterState(stateSplash);
	}
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
	if ((i->isGamepadButtonPressed(Gamepad::BUTTON_ACTIVATE) || i->isKeyPressed(Input::KEY_F)) && !s_armorGames) {
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

	if (m_recorder != NULL) {
		if (i->isKeyPressed(Input::KEY_9)) {
			m_recorder->screenshotEvery(0.5, 1000);
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
	}

	if (m_fmod != NULL) { m_fmod->update(); }

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
	else if (getCurrentState() == stateArmorGames || getCurrentState() == stateAutoplayFix) {
		if (m_musicMenu->isPlaying()) {	m_musicMenu->stop(); }
		if (m_musicGame->isPlaying()) { m_musicGame->stop(); }
		if (m_musicSummary->isPlaying()) { m_musicSummary->stop(); }
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
	//hsvShader->set(1.0f, 1.0f, 1.0f);
	Renderer* r = ARK2D::getRenderer();

	auto renderGame = [this, r]()->void {

		hsvShader->start();
		renderBackground();

		StateBasedGame::render();

		r->setDrawColor(Color::white);
		Input* in = ARK2D::getInput();
		if (m_reticule != NULL) {
			m_reticule->setRotation(m_reticuleRotation);
			m_reticule->drawCentered(in->getMouseX(), in->getMouseY());
		}

		hsvShader->stop();
	};

   	//grainFBO->bind();
	///grainFBO->bind_2d();
	//renderGame();
	//grainFBO->unbind_2d();
	//grainFBO->unbind();

	r->setDrawColor(Color::white);
	renderGame();

    // grainShader->timer += 0.016f;
    // if (grainShader->timer >= 4.0f) {
    // 	grainShader->timer = 0.0f;
    // }
    // grainShader->start();
    // r->setDrawColor(Color::white, 1.0f);
    // (*grainFBO->getImage())->draw(0, 0);
    // grainShader->stop();



	if (m_recorder != NULL) m_recorder->render();
}

void DefaultGame::resize(GameContainer* container, int width, int height) {
	StateBasedGame::resize(container, width, height);
	if (m_recorder != NULL) m_recorder->resize(0,0,width,height);
	//if (grainFBO != NULL) grainFBO->resize(width,height);
}

DefaultGame* DefaultGame::getInstance() {
    return dynamic_cast<DefaultGame*>(ARK2D::getGame());
}

DefaultGame::~DefaultGame() {

}
