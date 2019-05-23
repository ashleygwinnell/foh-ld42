
#ifndef ARK2D_DEFAULTGAME_ARMORGAMESSTATE_H_
#define ARK2D_DEFAULTGAME_ARMORGAMESSTATE_H_

#include <ARK2D/Core/State/GameState.h>

class DefaultGame;

class ArmorGamesOverlay {
		public:
			ARK::UI::Button* m_buttonPlayMoreGames;
			ARK::UI::Button* m_buttonLikeUs;
			ArmorGamesOverlay();
			void update();
			void render();
			virtual bool keyPressed(unsigned int key);
			virtual bool keyReleased(unsigned int key);
			virtual bool mouseMoved(int x, int y, int oldx, int oldy);
};

class ArmorGamesState : public GameState {

	public:
		ArmorGamesState();
		void enter(GameContainer* container, StateBasedGame* game, GameState* from);
		void leave(GameContainer* container, StateBasedGame* game, GameState* to);

		unsigned int id();
		void init(GameContainer* container, StateBasedGame* game);
		void update(GameContainer* container, StateBasedGame* game, GameTimer* timer);
		void render(GameContainer* container, StateBasedGame* game, Renderer* r);

		virtual ~ArmorGamesState();
};

#endif /* ARK2D_DEFAULTGAME_ARMORGAMESSTATE_H_ */
