
#ifndef ARK2D_DEFAULTGAME_UPGRADESSTATE_H_
#define ARK2D_DEFAULTGAME_UPGRADESSTATE_H_

#include <ARK2D/Core/State/GameState.h>

class DefaultGame;

class Upgrade {
	public:
		Upgrade();
		int id;
		int type;
		int typeOrder;
		int typeIsEquippable;
		int gx;
		int gy;
		float hoverTimer;
		float hoverDuration;
		bool purchased;
		bool equipped;
		int cost;
		string name;
		string description;
		Image* icon;
		uint32_t getButtonBackgroundColor();

		static const unsigned int UPGRADE_HP_1 = 0;
		static const unsigned int UPGRADE_HP_2 = 1;
		static const unsigned int UPGRADE_HP_3 = 2;

		static const unsigned int UPGRADE_FLAMES_CAPACITY_SML = 3;
		static const unsigned int UPGRADE_FLAMES_CAPACITY_MED = 4;
		static const unsigned int UPGRADE_FLAMES_CAPACITY_LRG = 5;
		//static const unsigned int UPGRADE_FLAMES_EXTRA_HOT = 6;

		static const unsigned int UPGRADE_GAS_CAPACITY_SML = 7;
		static const unsigned int UPGRADE_GAS_CAPACITY_MED = 8;
		static const unsigned int UPGRADE_GAS_CAPACITY_LRG = 9;

		static const unsigned int UPGRADE_WALLET_SIZE_SML = 10;
		static const unsigned int UPGRADE_WALLET_SIZE_MED = 11;
		static const unsigned int UPGRADE_WALLET_SIZE_LRG = 12;

		static const unsigned int UPGRADE_BARE_FEET = 13;
		static const unsigned int UPGRADE_HOT_FEET = 14;
		static const unsigned int UPGRADE_COLD_FEET = 15;
		static const unsigned int UPGRADE_FAST_FEET = 16;

		static const unsigned int UPGRADE_HEALTH_VISION = 17;
		static const unsigned int UPGRADE_NIGHT_VISION = 18;

		static const unsigned int UPGRADE_MAGNET = 19;
		static const unsigned int UPGRADE_BASEMENT_KEYS = 20;





		static const unsigned UPGRADE_TYPE_HP = 0;
		static const unsigned UPGRADE_TYPE_FLAMES = 1;
		static const unsigned UPGRADE_TYPE_GAS = 2;
		static const unsigned UPGRADE_TYPE_MONEY = 3;
		static const unsigned UPGRADE_TYPE_FEET = 4;
		static const unsigned UPGRADE_TYPE_VISION = 5;

		static const unsigned UPGRADE_TYPE_MISC1 = 6;
		static const unsigned UPGRADE_TYPE_MISC2 = 7;
		static const unsigned UPGRADE_TYPE_MISC3 = 8;
		static const unsigned UPGRADE_TYPE_MISC4 = 9;
		static const unsigned UPGRADE_TYPE_MISC5 = 10;
};

class UpgradesState : public GameState {
	public:
		GameState* m_fromState;
		vector<Upgrade*> m_upgradeData;

		Image* btnBorder;
		Image* title;

		ARK::UI::Button* m_buttonBack;
		ARK::UI::Button* m_buttonPlay;
		vector<ARK::UI::Button* >* m_upgradeButtons;
		ARK::UI::Button* m_buttonUpgradeBuy;
		ARK::UI::Button* m_buttonUpgradeEquip;
		signed int m_overButtonIndex;

		Image* iconBack;
		Image* iconPlay;


		float juiceTimer;
		float juiceDuration;

		//AnimationFrameList* m_blobFrames;
		//Animation* m_blob;
		vector<string> m_blobMessages;

		int m_state;
		static const unsigned int STATE_MESSAGE = 0;
		static const unsigned int STATE_SHOP = 1;

		float m_messageTimer;
		float m_messageDuration;
		int m_messageIndex;

		float m_introTimer;
		float m_introDuration;

		float m_outroTimer;
		float m_outroDuration;
		GameState* m_outroTo;

		bool m_purchasedAnything;

		Upgrade* getUpgradeById(unsigned int id);
		unsigned int getCurrentForUpgradeType(unsigned int upgradeType);
		// unsigned int getCurrentWeapon();
		// unsigned int getCurrentBattery();
		int getCurrentHP();
		int getCurrentFireCapacity();
		int getCurrentCO2Capacity();
		int getCurrentMaxWallet();
		bool hasMagnet();
		bool hasBasementKey();
		bool hasNightVision();
		bool hasHealthVision();
		bool hasAnyUpgrade();


		UpgradesState();
		void enter(GameContainer* container, StateBasedGame* game, GameState* from);
		void leave(GameContainer* container, StateBasedGame* game, GameState* to);

		unsigned int id();
		void init(GameContainer* container, StateBasedGame* game);
		void update(GameContainer* container, StateBasedGame* game, GameTimer* timer);
		void render(GameContainer* container, StateBasedGame* game, Renderer* r);

		signed int getPointerOverButtonIndex();

		//void renderBlobGuy();


		virtual bool keyPressed(unsigned int key);
		virtual bool keyReleased(unsigned int key);
		virtual bool mouseMoved(int x, int y, int oldx, int oldy);

		virtual ~UpgradesState();
};

#endif /* ARK2D_DEFAULTGAME_MENUSTATE_H_ */
