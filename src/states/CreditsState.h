
#ifndef ARK2D_DEFAULTGAME_CREDITSSTATE_H_
#define ARK2D_DEFAULTGAME_CREDITSSTATE_H_

#include <ARK2D/Core/State/GameState.h>

class DefaultGame;

class CreditsState : public GameState {

	public:

		Image* logo;
		Image* logoGreen;

		Image* iconClose;
		Image* iconFacebook;
		Image* iconTwitter;
		Image* iconMail;
		Image* iconDiscord;

		ARK::UI::Button* m_buttonBack;
		ARK::UI::Button* m_buttonWeb;
		ARK::UI::Button* m_buttonMail;
		ARK::UI::Button* m_buttonTwitter;
		ARK::UI::Button* m_buttonFacebook;
		ARK::UI::Button* m_buttonDiscord;

		float m_introTimer;
		float m_introDuration;

		float m_outroTimer;
		float m_outroDuration;
		GameState* m_outroTo;

		float m_timer;

		CreditsState();
		void enter(GameContainer* container, StateBasedGame* game, GameState* from);
		void leave(GameContainer* container, StateBasedGame* game, GameState* to);

		unsigned int id();
		void init(GameContainer* container, StateBasedGame* game);
		void update(GameContainer* container, StateBasedGame* game, GameTimer* timer);
		void render(GameContainer* container, StateBasedGame* game, Renderer* r);

		virtual bool keyPressed(unsigned int key);
		virtual bool keyReleased(unsigned int key);
		virtual bool mouseMoved(int x, int y, int oldx, int oldy);

		virtual ~CreditsState();
};

#endif /* ARK2D_DEFAULTGAME_CREDITSSTATE_H_ */
