
#ifndef ARK2D_DEFAULTGAME_MENUSTATE_H_
#define ARK2D_DEFAULTGAME_MENUSTATE_H_

#include <ARK2D/Core/State/GameState.h>

class DefaultGame;

class MenuState : public GameState {
	public:
		signed int m_selectedIndex;

		static const int MENU_BEGIN = 0;
		static const int MENU_PLAY = 0;
		static const int MENU_INSTRUCTIONS = 1;
		static const int MENU_EXIT = 2;
		static const int MENU_END = MENU_EXIT;

		Image* logo;
		Image* imageGradient;
		float imageGradientOffsetX;
		float imageRedPurpleRotation;;

		Image* iconTwitter;
		Image* iconFullscreen;
		Image* iconHelp;
		Image* iconMusicOn;
		Image* iconMusicOff;
		Image* iconSFXOn;
		Image* iconSFXOff;
		Image* iconPlay;
		Image* iconUpgrades;
		Image* iconSettings;

		bool m_settingsOpen;
		float m_settingsOpeningTimer;
		float m_settingsClosingTimer;
		float m_settingsDuration;

		ARK::UI::Button* m_buttonPlay;
		ARK::UI::Button* m_buttonUpgrades;
		ARK::UI::Button* m_buttonFOH;
		ARK::UI::Button* m_buttonSettings;
		ARK::UI::Button* m_buttonFullscreen;
		ARK::UI::Button* m_buttonMusic;
		ARK::UI::Button* m_buttonSFX;

		float m_introTimer;
		float m_introDuration;

		float m_outroTimer;
		float m_outroDuration;
		GameState* m_outroTo;

		float juiceTimer;
		float juiceDuration;

		float getSettingsButtonsX();

		MenuState();
		void enter(GameContainer* container, StateBasedGame* game, GameState* from);
		void leave(GameContainer* container, StateBasedGame* game, GameState* to);

		unsigned int id();
		void init(GameContainer* container, StateBasedGame* game);
		void update(GameContainer* container, StateBasedGame* game, GameTimer* timer);
		void render(GameContainer* container, StateBasedGame* game, Renderer* r);

		void drawSpinBG(float percentageClosed = 1.0f);
		void drawDitherGradient(float on = 1.0f);

		virtual bool keyPressed(unsigned int key);
		virtual bool keyReleased(unsigned int key);
		virtual bool mouseMoved(int x, int y, int oldx, int oldy);

		virtual ~MenuState();
};

#endif /* ARK2D_DEFAULTGAME_MENUSTATE_H_ */
