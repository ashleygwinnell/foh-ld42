#include "CreditsState.h"

#include "../DefaultGame.h"

CreditsState::CreditsState():
	GameState() {

}

unsigned int CreditsState::id() {
	return StatesList::STATE_CREDITS;
}

void CreditsState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {
	m_outroTimer = 0.0f;
	m_introTimer = 0.01f;
	m_timer = 0.0f;

	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_save->set("viewed_credits", true);
    dg->m_stats->submit("View Credits", 1);
}

void CreditsState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {

}

void creditsEvent_back() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();

	if (dg->stateCredits->m_introTimer > 0.0f || dg->stateCredits->m_outroTimer > 0.0f) { return; }

	dg->stateCredits->m_outroTo = dg->stateMenu;
	dg->stateCredits->m_outroTimer = 0.01f;
}
void creditsEvent_website() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();
	SystemUtil::openBrowserToURL("http://forceofhab.it/");
}
void creditsEvent_mail() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();
	SystemUtil::openBrowserToURL("http://forceofhab.it/mailing-list/");
}
void creditsEvent_twitter() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();
	SystemUtil::openBrowserToURL("http://twitter.com/forcehabit");
}
void creditsEvent_facebook() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();
	SystemUtil::openBrowserToURL("http://facebook.com/forcehabit");
}
void creditsEvent_discord() {
	DefaultGame* dg = DefaultGame::getInstance();
	dg->m_sfxMenuSelect->play();
	SystemUtil::openBrowserToURL("http://forceofhab.it/discord/");
}

void CreditsState::init(GameContainer* container, StateBasedGame* game) {
	logo = SpriteSheetStore::getImage("sprites/ui-foh-purple.png");
	logoGreen = SpriteSheetStore::getImage("sprites/ui-foh-green.png");

	iconClose = SpriteSheetStore::getImage("sprites/ui-icon-close-sml.png");
	iconFacebook = SpriteSheetStore::getImage("sprites/ui-icon-facebook.png");
	iconDiscord = SpriteSheetStore::getImage("sprites/ui-icon-discord.png");
	iconTwitter = SpriteSheetStore::getImage("sprites/ui-icon-twitter.png");
	iconMail = SpriteSheetStore::getImage("sprites/ui-icon-mail.png");

	m_timer = 0.0f;

	m_buttonBack = new ARK::UI::Button();
	m_buttonBack->setLocation(0, 0);
	m_buttonBack->setSize(20, 20);
	m_buttonBack->setEvent((void*) &creditsEvent_back);
	m_buttonBack->setLocationByCenter(14, 14);

	float buttonsY = 134;
	m_buttonWeb = new ARK::UI::Button();
	m_buttonWeb->setLocation(0, 0);
	m_buttonWeb->setSize(60, 60);
	m_buttonWeb->setEvent((void*) &creditsEvent_website);
	m_buttonWeb->setLocationByCenter(container->getWidth()*0.5f, 42.0f);

	m_buttonMail = new ARK::UI::Button();
	m_buttonMail->setLocation(0, 0);
	m_buttonMail->setSize(24, 24);
	m_buttonMail->setEvent((void*) &creditsEvent_mail);
	m_buttonMail->setLocationByCenter(container->getWidth()*0.32f, buttonsY);

	m_buttonTwitter = new ARK::UI::Button();
	m_buttonTwitter->setLocation(0, 0);
	m_buttonTwitter->setSize(24, 24);
	m_buttonTwitter->setEvent((void*) &creditsEvent_twitter);
	m_buttonTwitter->setLocationByCenter(container->getWidth()*0.44f, buttonsY);

	m_buttonFacebook = new ARK::UI::Button();
	m_buttonFacebook->setLocation(0, 0);
	m_buttonFacebook->setSize(24, 24);
	m_buttonFacebook->setEvent((void*) &creditsEvent_facebook);
	m_buttonFacebook->setLocationByCenter(container->getWidth()*0.56f, buttonsY);

	m_buttonDiscord = new ARK::UI::Button();
	m_buttonDiscord->setLocation(0, 0);
	m_buttonDiscord->setSize(24, 24);
	m_buttonDiscord->setEvent((void*) &creditsEvent_discord);
	m_buttonDiscord->setLocationByCenter(container->getWidth()*0.68f, buttonsY);

	m_introTimer = 0.01f;
	m_introDuration = 0.5f;

	m_outroTimer = 0.0f;
	m_outroDuration = 0.5f;;
	m_outroTo = NULL;
}

void CreditsState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {

    DefaultGame* dg = DefaultGame::getInstance();
	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer > m_introDuration) {
			m_introTimer = 0.0f;
		}
	}
	if (m_outroTimer > 0.0f) {
		m_outroTimer += timer->getDelta();
		if (m_outroTimer > m_outroDuration) {
			dg->enterState(m_outroTo);
			return;
		}
	}

	m_timer += timer->getDelta();
}

void CreditsState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {

	DefaultGame* dg = DefaultGame::getInstance();

	float spinClosedAmount = 1.0f;
	if (m_outroTimer > 0.0f && m_outroTo == dg->stateInGame) {
		spinClosedAmount = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 1.0f, 0.0f, m_outroDuration);

		dg->stateInGame->render(container, game, r);
	}
	dg->stateMenu->drawSpinBG(spinClosedAmount);


	float baseY = 0;
	float alpha = 1.0f;

	if (m_introTimer > 0.0f) {
		baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, container->getHeight(), 0, m_introDuration);
		alpha = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0, 1, m_introDuration);
	}
	else if (m_outroTimer > 0.0f) {
		baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 0, container->getHeight() * -1.0f, m_outroDuration);
		alpha = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 1, 0, m_outroDuration);
	}

	r->setDrawColor("#333333",alpha);
	r->fillRect(0,0,container->getWidth(),container->getHeight());

	dg->stateMenu->drawDitherGradient(1.0f);

	r->setDrawColor(Color::white);
	logo->drawCentered(container->getWidth()*0.5f, baseY + 42);

	float fohAlphaTimer = Easing::easebetweenback(Easing::QUADRATIC_IN_OUT, Easing::QUADRATIC_IN_OUT, fmod(m_timer, 8.0f), 0.0f, 1.0f, 8.0f);

	r->setDrawColor(Color::white, fohAlphaTimer);//abs(sin(m_timer*0.25f)));
	logoGreen->drawCentered(container->getWidth()*0.5f, baseY + 42);

	r->setDrawColor(Color::white);
	dg->font->drawString("FORCE OF HABIT", container->getWidth()*0.5f, baseY+82, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);
	dg->font->drawString("A TINY INDIE GAME STUDIO", container->getWidth()*0.5f, baseY+92, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
	dg->font->drawString("PLEASE FOLLOW OUR GAME DEVELOPMENT!", container->getWidth()*0.5f, baseY+109, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);


	//ThreeSliceButton::draw(m_buttonBack->transform.position.x, m_buttonBack->transform.position.y, m_buttonBack->getWidth());
	dg->m_cornerBox->drawCentered(m_buttonBack->transform.position.x, baseY+m_buttonBack->transform.position.y);
	iconClose->drawCentered(m_buttonBack->transform.position.x, baseY+m_buttonBack->transform.position.y);

	ThreeSliceButton::draw(m_buttonFacebook->transform.position.x, baseY+m_buttonFacebook->transform.position.y, m_buttonFacebook->getWidth());
	iconFacebook->drawCentered(m_buttonFacebook->transform.position.x, baseY+m_buttonFacebook->transform.position.y);

	ThreeSliceButton::draw(m_buttonTwitter->transform.position.x, baseY+m_buttonTwitter->transform.position.y, m_buttonTwitter->getWidth());
	iconTwitter->drawCentered(m_buttonTwitter->transform.position.x, baseY+m_buttonTwitter->transform.position.y+0.5f);

	ThreeSliceButton::draw(m_buttonMail->transform.position.x, baseY+m_buttonMail->transform.position.y, m_buttonMail->getWidth());
	iconMail->drawCentered(m_buttonMail->transform.position.x-0.5f, baseY+m_buttonMail->transform.position.y);

	ThreeSliceButton::draw(m_buttonDiscord->transform.position.x, baseY+m_buttonDiscord->transform.position.y, m_buttonDiscord->getWidth());
	iconDiscord->drawCentered(m_buttonDiscord->transform.position.x-0.5f, baseY+m_buttonDiscord->transform.position.y);
}


bool CreditsState::keyPressed(unsigned int key) {
	m_buttonBack->keyPressed(key);
	m_buttonWeb->keyPressed(key);
	m_buttonMail->keyPressed(key);
	m_buttonTwitter->keyPressed(key);
	m_buttonFacebook->keyPressed(key);
	m_buttonDiscord->keyPressed(key);
	return false;
}
bool CreditsState::keyReleased(unsigned int key) {
	m_buttonBack->keyReleased(key);
	m_buttonWeb->keyReleased(key);
	m_buttonMail->keyReleased(key);
	m_buttonTwitter->keyReleased(key);
	m_buttonFacebook->keyReleased(key);
	m_buttonDiscord->keyReleased(key);
	return false;
}
bool CreditsState::mouseMoved(int x, int y, int oldx, int oldy) {
	m_buttonBack->mouseMoved(x, y, oldx, oldy);
	m_buttonWeb->mouseMoved(x, y, oldx, oldy);
	m_buttonMail->mouseMoved(x, y, oldx, oldy);
	m_buttonTwitter->mouseMoved(x, y, oldx, oldy);
	m_buttonFacebook->mouseMoved(x, y, oldx, oldy);
	m_buttonDiscord->mouseMoved(x, y, oldx, oldy);
	return false;
}

CreditsState::~CreditsState() {

}
