#include "ArmorGamesState.h"

#include "../DefaultGame.h"

void agPlayMoreGames() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();
	SystemUtil::openBrowserToURL("http://armor.ag/MoreGames");
}
void agLikeUs() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();
	SystemUtil::openBrowserToURL("http://www.facebook.com/ArmorGames");
}

ArmorGamesOverlay::ArmorGamesOverlay() {
	GameContainer* container = ARK2D::getContainer();

	m_buttonPlayMoreGames = new ARK::UI::Button();
	m_buttonPlayMoreGames->setLocation(0, 0);
	m_buttonPlayMoreGames->setSize(70, 14);//24);
	m_buttonPlayMoreGames->setEvent((void*) &agPlayMoreGames);
	m_buttonPlayMoreGames->setLocationByCenter(66, container->getHeight() - 12);

	m_buttonLikeUs = new ARK::UI::Button();
	m_buttonLikeUs->setLocation(0, 0);
	m_buttonLikeUs->setSize(70, 14);
	m_buttonLikeUs->setEvent((void*) &agLikeUs);
	m_buttonLikeUs->setLocationByCenter(container->getWidth() - 66, container->getHeight() - 12);
}
void ArmorGamesOverlay::update() {
	m_buttonPlayMoreGames->update();
	m_buttonLikeUs->update();
}
void ArmorGamesOverlay::render() {
    DefaultGame* dg = DefaultGame::getInstance();
    float baseYBottom = 0;
    if (dg->getCurrentState() == dg->stateMenu) {
    	baseYBottom = dg->stateMenu->getOutYMultiplier() * 100;
    } else if (dg->getCurrentState() == dg->stateUpgrades) {
    	baseYBottom = dg->stateUpgrades->getOutYMultiplier() * 100;
    }

	ThreeSliceSmallerButton::draw(m_buttonLikeUs->transform.position.x, baseYBottom + m_buttonLikeUs->transform.position.y, m_buttonLikeUs->getWidth());
	dg->font->drawString("LIKE US!", m_buttonLikeUs->transform.position.x-1, baseYBottom + m_buttonLikeUs->transform.position.y+1, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);

	ThreeSliceSmallerButton::draw(m_buttonPlayMoreGames->transform.position.x, baseYBottom + m_buttonPlayMoreGames->transform.position.y, m_buttonPlayMoreGames->getWidth());
	dg->font->drawString("PLAY MORE GAMES", m_buttonPlayMoreGames->transform.position.x-1, baseYBottom + m_buttonPlayMoreGames->transform.position.y+1, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
}
bool ArmorGamesOverlay::keyPressed(unsigned int key) {
	m_buttonPlayMoreGames->keyPressed(key);
	m_buttonLikeUs->keyPressed(key);
	return false;
}
bool ArmorGamesOverlay::keyReleased(unsigned int key) {
	m_buttonPlayMoreGames->keyReleased(key);
	m_buttonLikeUs->keyReleased(key);
	return false;
}
bool ArmorGamesOverlay::mouseMoved(int x, int y, int oldx, int oldy) {
	m_buttonPlayMoreGames->mouseMoved(x,y,oldx,oldy);
	m_buttonLikeUs->mouseMoved(x,y,oldx,oldy);
	return false;
}


ArmorGamesState::ArmorGamesState():
	GameState() {

}

unsigned int ArmorGamesState::id() {
	return StatesList::STATE_ARMORGAMES;
}

void ArmorGamesState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {
	ARK2D::getLog()->e("ArmorGamesState::enter");
	#ifdef ARK2D_EMSCRIPTEN_JS
		EM_ASM({
			window.focus();
		 	document.getElementById("canvas").focus();

			var videoElement = window.armorGamesSponsorVideo;
			videoElement.play().then(function(){
				// playing
			}).catch( function(){
				// not playing
				var playButton = window.armorGamesSponsorVideo_playButton;
				playButton.style.display = "block";
			});
		});
	#else
		DefaultGame* dg = DefaultGame::getInstance();
		dg->enterState(dg->stateSplash);
	#endif
}

void ArmorGamesState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {

}

void ArmorGamesState::init(GameContainer* container, StateBasedGame* game) {
	#ifdef ARK2D_EMSCRIPTEN_JS

		if (DefaultGame::s_armorGames) {
			int nextState = StatesList::STATE_SPLASH;
			EM_ASM_({

				var anchor = document.createElement("a");
				anchor.href = "http://armor.ag/MoreGames";
				anchor.target = "new";

				var videoElement = document.createElement("video");
				videoElement.style.zIndex = "99999";
				videoElement.style.position = "absolute";
				videoElement.style.visibility = "hidden";
				videoElement.style.backgroundColor = "black";
				videoElement.style.cursor = "pointer";

				var sourceM4V = document.createElement("source");
				sourceM4V.src = "http://files.armorgames.com/intro/648x432_v1.m4v";
				sourceM4V.type = "video/mp4";
				videoElement.appendChild(sourceM4V);

				var sourceWebM = document.createElement("source");
				sourceWebM.src = "http://files.armorgames.com/intro/648x432_v1.webm";
				sourceWebM.type = "video/webm";
				videoElement.appendChild(sourceWebM);

				var sourceOgg = document.createElement("source");
				sourceOgg.src = "http://files.armorgames.com/intro/648x432_v1.ogv";
				sourceOgg.type = "video/ogg";
				videoElement.appendChild(sourceOgg);

				videoElement.addEventListener("play", () => {

				});
				videoElement.addEventListener('ended', () => {
					console.log('ended');

					window.armorGamesSponsorVideo.parentElement.removeChild(window.armorGamesSponsorVideo);

					Module.ccall('emscripten_StateBasedGame_enterState', // name of C function
			                        null, // void return type
			                        [ 'number' ], // argument types,
			                        [ $2 ] //, // arguments
			                        //{ async:true } // optional options
			                    );
				});

				videoElement.style.width = $0;
				videoElement.style.height = $1;
				videoElement.width = $0;
				videoElement.height = $1;
				videoElement.style.top = "0px";
				videoElement.style.left = ((window.innerWidth - $0) * 0.5) + "px";

				videoElement.style.visibility = "visible";
				videoElement.currentTime = 0;
				videoElement.muted = false;

				videoElement.setAttribute("playsinline", "");
				videoElement.setAttribute("preload", "auto");
				videoElement.onerror = function() {
					console.log('error');
				};
				videoElement.load();

				anchor.appendChild(videoElement);
				document.getElementById("canvas").parentElement.appendChild(anchor);

				window.armorGamesSponsorVideo = videoElement;


				var playButton = document.createElement("button");
				playButton.innerHTML = "Play";
				playButton.onclick = function(){
					window.armorGamesSponsorVideo.play();
					window.armorGamesSponsorVideo_playButton.style.display = "none";
				};
				playButton.style.display = "none";
				playButton.style.width = "50%";
				playButton.style.height = "40%";
				playButton.style.left = "25%";
				playButton.style.top = "30%";
				playButton.style.position = "absolute";
				playButton.style.zIndex = "199999";
				playButton.style.fontSize = "40px";
				window.armorGamesSponsorVideo_playButton = playButton;

				document.getElementById("canvas").parentElement.appendChild(playButton);

			}, container->getWidth()*3, container->getHeight()*3, nextState);
		}
	#endif
}

void ArmorGamesState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {
	#ifndef ARK2D_EMSCRIPTEN_JS
		DefaultGame* dg = DefaultGame::getInstance();
		dg->enterState(dg->stateSplash);
	#endif
}

void ArmorGamesState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {

}

ArmorGamesState::~ArmorGamesState() {

}
