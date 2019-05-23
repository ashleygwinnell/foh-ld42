
#ifndef ARK2D_DEFAULTGAME_H_
#define ARK2D_DEFAULTGAME_H_

#include <ARK2D/Audio/Sound.h>
#include <ARK2D/Core/Graphics/Image.h>
#include <ARK2D/Core/Graphics/SpriteSheetDescription.h>
#include <ARK2D/Core/State/StateBasedGame.h>
#include <ARK2D/Core/+.h>

#include "states/BlankState.h"
#include "states/InGameState.h"
#include "states/InstructionsState.h"
#include "states/MenuState.h"
#include "states/SplashState.h"
#include "states/SummaryState.h"
#include "states/UpgradesState.h"
#include "states/CreditsState.h"
#include "states/ArmorGamesState.h"
#include "states/AutoplayFixState.h"

#include "util/FMOD.h"

#include <ARK2D/Core/Graphics/ScreenRecorder.h>
#include <ARK2D/Core/Graphics/Shaders/HSVShader.h>
#include <ARK2D/Core/Graphics/Shaders/GrainShader.h>

class StatesList {
	public:
		static const unsigned int STATE_BLANK = 0;
		static const unsigned int STATE_LOADING = 1;
		static const unsigned int STATE_SPLASH = 2;
		static const unsigned int STATE_MENU = 3;
		static const unsigned int STATE_INSTRUCTIONS = 4;
		static const unsigned int STATE_INGAME = 5;
		static const unsigned int STATE_SUMMARY = 6;
		static const unsigned int STATE_UPGRADES = 7;
		static const unsigned int STATE_CREDITS = 8;
		static const unsigned int STATE_ARMORGAMES = 9;
		static const unsigned int STATE_AUTOPLAYFIX = 10;
};

class ThreeSliceButton {
	public:
		static Image* s_buttonBackgroundLeft;
		static Image* s_buttonBackgroundMiddle;
		static Image* s_buttonBackgroundRight;
		static Image* s_buttonBorderLeft;
		static Image* s_buttonBorderMiddle;
		static Image* s_buttonBorderRight;
		static void init();
		static void draw(int x, int y, int width);
		static void draw(int x, int y, int width, uint32_t bgcolor);
};

class ThreeSliceSmallerButton {
	public:
		static Image* s_buttonBackgroundLeft;
		static Image* s_buttonBackgroundMiddle;
		static Image* s_buttonBackgroundRight;
		static Image* s_buttonBorderLeft;
		static Image* s_buttonBorderMiddle;
		static Image* s_buttonBorderRight;
		static void init();
		static void draw(int x, int y, int width);
		static void draw(int x, int y, int width, uint32_t bgcolor);
};

class MyTextBubble {
	public:
		static Image* s_buttonBackgroundLeft;
		static Image* s_buttonBackgroundMiddle;
		static Image* s_buttonBackgroundRight;
		static Image* s_buttonBackgroundArrow;
		static void init();
		static void draw(string text, int x, int y, int width);
};

class KongStatsSubmit {

};

class StatsSubmit {
public:
	string loggedInAs;
	string gameId;
	string apiKey;
	string guestAccessKey;
	string guestAccessUrl;
	void init();
	void submit(string name, int val);
	void updateStats();

};

class DefaultGame : public StateBasedGame {
	public:



		BlankState* stateBlank = NULL;
		SplashState* stateSplash = NULL;
		MenuState* stateMenu = NULL;
		InstructionsState* stateInstructions = NULL;
		InGameState* stateInGame = NULL;
		SummaryState* stateSummary = NULL;
		UpgradesState* stateUpgrades = NULL;
		CreditsState* stateCredits = NULL;
		ArmorGamesState* stateArmorGames = NULL;
		AutoplayFixState* stateAutoplayFix = NULL;

		Image* spritesheet = NULL;
		SpriteSheetDescription* desc = NULL;

		ARK::Core::Font::Font* font = NULL;

		ARK::Core::Graphics::HSVShader* hsvShader;
		ARK::Core::Graphics::GrainShader* grainShader;
		ARK::Core::Graphics::FBO* grainFBO;

		int m_coins;

		ArkFMOD* m_fmod;
		ArkFMODSound* m_musicMenu;
		ArkFMODSound* m_musicGame;
		ArkFMODSound* m_musicSummary;
		ArkFMODSound* m_musicShop;
		ArkFMODSound* m_musicCurrent;

		ArkFMODSound* m_sfxFOH;
		ArkFMODSound* m_sfxMenuSelect;
		ArkFMODSound* m_sfxMenuBuy;
		ArkFMODSound* m_sfxPlayerDie;
		ArkFMODSound* m_sfxPlayerHurt;
		ArkFMODSound* m_sfxLadder;

		ArkFMODSound* m_sfxFireFlames;
		ArkFMODSound* m_sfxFireGas;
		ArkFMODSound* m_sfxEnemyDeathMini;
		ArkFMODSound* m_sfxEnemyDeathSmall;
		ArkFMODSound* m_sfxEnemyDeathMedium;

		ArkFMODSound* m_sfxSantaHurt;
		ArkFMODSound* m_sfxWin;
		vector<ArkFMODSound*> m_explosions;

		float m_musicVolume = 1.0f;
		float m_sfxVolume = 1.0f;

		Image* m_reticule = NULL;
		float m_reticuleRotation = 0.0f;

		Image* m_cornerBox = NULL;

		Image* m_buttonbg;
		Image* m_gradient;
		Image* m_gradientWhite;

		float m_floor;

		KeyPairFile* m_save = NULL;
		StatsSubmit* m_stats = NULL;

		GIFRecorder* m_recorder;
		static bool s_debug;
		static bool s_armorGames;

		DefaultGame(string title);
		void updateAudioVolumes(bool save=true);
		virtual void initStates(GameContainer* container);
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void render();
		virtual void renderBackground();
		virtual void resize(GameContainer* container, int width, int height);
		virtual ~DefaultGame();

		static DefaultGame* getInstance();
};

#endif /* ARK2D_DEFAULTGAME_H_ */
