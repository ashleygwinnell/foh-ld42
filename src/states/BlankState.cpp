#include "BlankState.h"

#include "../DefaultGame.h"

BlankState::BlankState():
	GameState() {

}

unsigned int BlankState::id() {
	return StatesList::STATE_BLANK;
}

void BlankState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {

}

void BlankState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {

}

void BlankState::init(GameContainer* container, StateBasedGame* game) {

}

void BlankState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {

}

void BlankState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {
	r->setDrawColor(Color::white);
	//r->drawString("Blank State", 20, 20);
	r->drawString("SITELOCKED!", container->getWidth()*0.5f, container->getHeight()*0.4f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 2.0f);
	r->drawString("(c) FORCE OF HABIT", container->getWidth()*0.5f, container->getHeight()*0.6f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.75f);
	r->drawString("twitter.com/forcehabit", container->getWidth()*0.5f, container->getHeight()*0.65f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.75f);
}

BlankState::~BlankState() {

}
