#include "AutoplayFixState.h"

#include "../DefaultGame.h"

AutoplayFixState::AutoplayFixState():
	GameState() {

}

unsigned int AutoplayFixState::id() {
	return StatesList::STATE_AUTOPLAYFIX;
}

void AutoplayFixState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {
	ARK2D::getLog()->e("AutoplayFixState::enter");
	#ifdef ARK2D_EMSCRIPTEN_JS
		EM_ASM({
			window.focus();
		 	document.getElementById("canvas").focus();

			var playButton = window.foh_playButton;
			playButton.style.display = "block";

		});
	#else
		DefaultGame* dg = DefaultGame::getInstance();
		dg->enterState(dg->stateSplash);
	#endif
}

void AutoplayFixState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {

}

void AutoplayFixState::init(GameContainer* container, StateBasedGame* game) {
	#ifdef ARK2D_EMSCRIPTEN_JS

		int nextState = StatesList::STATE_SPLASH;
		EM_ASM_({

			var playButton = document.createElement("button");
			playButton.innerHTML = "Play";
			playButton.onclick = function(){
				window.foh_playButton.style.display = "none";

				Module.ccall('emscripten_StateBasedGame_enterState', // name of C function
                    null, // void return type
                    [ 'number' ], // argument types,
                    [ $0 ] //, // arguments
                    //{ async:true } // optional options
                );
			};
			playButton.style.display = "none";
			playButton.style.width = "50%";
			playButton.style.height = "40%";
			playButton.style.left = "25%";
			playButton.style.top = "30%";
			playButton.style.position = "absolute";
			playButton.style.zIndex = "199999";
			playButton.style.fontSize = "40px";
			window.foh_playButton = playButton;

			document.getElementById("canvas").parentElement.appendChild(playButton);

		}, nextState);

	#endif
}

void AutoplayFixState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {
	#ifndef ARK2D_EMSCRIPTEN_JS
		DefaultGame* dg = DefaultGame::getInstance();
		dg->enterState(dg->stateSplash);
	#endif
}

void AutoplayFixState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {

}

AutoplayFixState::~AutoplayFixState() {

}
