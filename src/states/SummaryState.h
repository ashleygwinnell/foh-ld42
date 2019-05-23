
#ifndef ARK2D_DEFAULTGAME_SUMMARYSTATE_H_
#define ARK2D_DEFAULTGAME_SUMMARYSTATE_H_

#include <ARK2D/Core/State/GameState.h>

class DefaultGame;

class MapLocation {
    public:
        Image* cityImage;
        float x;
        float y;

        float timer;
        float duration;
        bool ruined;
        static const unsigned int FACE_INTRO = 0;
        static const unsigned int FACE_IDLE = 1;
};

class SummaryState : public GameState {
	public:

		Image* iconNext;
		ARK::UI::Button* m_buttonNext;

		float m_introTimer;
		float m_introDuration;

		float m_outroTimer;
		float m_outroDuration;
		GameState* m_outroTo;

		float m_timer;

		Animation* m_playerAnimation;
		bool m_finalWin;

		CameraShake* m_cameraShake;

		SummaryState();
		void enter(GameContainer* container, StateBasedGame* game, GameState* from);
		void leave(GameContainer* container, StateBasedGame* game, GameState* to);

		unsigned int id();
		float getSantaX();
		void init(GameContainer* container, StateBasedGame* game);
		void update(GameContainer* container, StateBasedGame* game, GameTimer* timer);
		void render(GameContainer* container, StateBasedGame* game, Renderer* r);

		virtual bool keyPressed(unsigned int key);
		virtual bool keyReleased(unsigned int key);
		virtual bool mouseMoved(int x, int y, int oldx, int oldy);

		virtual ~SummaryState();
};

#endif /* ARK2D_DEFAULTGAME_SUMMARYSTATE_H_ */
