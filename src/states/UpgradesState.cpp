
#include "UpgradesState.h"
#include "../DefaultGame.h"
#include "../objects/Pickup.h"

UpgradesState::UpgradesState():
	GameState(),
	m_upgradeData() {

}

unsigned int UpgradesState::id() {
	return StatesList::STATE_UPGRADES;
}

void UpgradesState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {

	m_fromState = from;

	DefaultGame* dg = DefaultGame::getInstance();
	if (from == dg->stateInGame) {
		// m_messageIndex++;
		// m_messageIndex = m_messageIndex % m_blobMessages.size();
		// m_messageTimer = 0.01f;
		m_introTimer = 0.01f;
	} else {
		m_introTimer = 0.01f;
	}

	m_outroTimer = 0.0f;
	//m_introTimer = 0.01f;

	//DefaultGame* dg = DefaultGame::getInstance();
	if (dg->m_musicShop->isPlaying()) {
		dg->m_musicShop->setLowpass(true);
		dg->m_musicShop->setLowpassFrequency(22100.0f); //dg->m_musicGame->setLowpassFrequency(22100.0f);
	}
}

void UpgradesState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {

}

void upgradesEvent_twitter() {
	SystemUtil::openBrowserToURL("http://twitter.com/forcehabit");
}
void upgradesEvent_back () {
    DefaultGame* dg = DefaultGame::getInstance();

    if (dg->stateUpgrades->m_introTimer > 0.0f ||
    	dg->stateUpgrades->m_messageTimer > 0.0f ||
    	dg->stateUpgrades->m_outroTimer > 0.0f) { return; }

    dg->m_sfxMenuSelect->play();
    dg->stateUpgrades->m_outroTo = dg->stateMenu;
	dg->stateUpgrades->m_outroTimer = 0.01f;
}
void upgradesEvent_play() {
	DefaultGame* dg = DefaultGame::getInstance();

	if (dg->stateUpgrades->m_introTimer > 0.0f ||
		dg->stateUpgrades->m_messageTimer > 0.0 ||
		dg->stateUpgrades->m_outroTimer > 0.0f) { return; }

    dg->stateUpgrades->m_outroTo = dg->stateInGame;
	dg->stateUpgrades->m_outroTimer = 0.01f;

    dg->m_sfxMenuSelect->play();
    dg->stateInGame->reset();
    dg->stateInGame->start();
}
void upgrades_setExclusiveEquippedForType(Upgrade* u) {
	DefaultGame* dg = DefaultGame::getInstance();
	UpgradesState* s = dg->stateUpgrades;
	for(unsigned int i = 0; i < s->m_upgradeData.size(); i++) {
		if (s->m_upgradeData.at(i)->type == u->type) {
			s->m_upgradeData.at(i)->equipped = false;
			dg->m_save->set(StringUtil::append("upgradeEquipped_", s->m_upgradeData.at(i)->id), false);
		}
	}
	u->equipped = true;
	dg->m_save->set("coins", (unsigned int) dg->m_coins);
	dg->m_save->set(StringUtil::append("upgradeEquipped_", u->id), true);
	dg->m_save->save();
}
bool upgrades_hasPurchasedAllPreviousInType(int type, int order) {
	DefaultGame* dg = DefaultGame::getInstance();
	UpgradesState* s = dg->stateUpgrades;
	for(unsigned int i = 0; i < s->m_upgradeData.size(); i++) {
		Upgrade* u = s->m_upgradeData.at(i);
		if (u->type == type) {
			if (u->typeOrder < order && !u->purchased) {
				return false;
			}
		}
	}
	return true;
}
void upgradesEvent_buy();
void upgradesEvent_click() {
	GameContainer* container = ARK2D::getContainer();
//    if (DefaultGame::isTouchMode()) {
//        UpgradesState* s = DefaultGame::getInstance()->stateUpgrades;
//        s->m_overButtonIndex = s->getPointerOverButtonIndex();
//    } else {
		upgradesEvent_buy();
	//}
}
bool upgradeCanBeBought(Upgrade* u, bool forcetrue) {
	if (forcetrue) { return true; }

	if (u->purchased) { return false; }

    DefaultGame* dg = DefaultGame::getInstance();
    bool hasMoney = dg->m_coins >= u->cost;
	if ( !hasMoney ) { return false; }

	bool canGoAhead = false;
	if (u->type >= Upgrade::UPGRADE_TYPE_MISC1) {
		canGoAhead = true;
	}
	else {
		canGoAhead = upgrades_hasPurchasedAllPreviousInType(u->type, u->typeOrder);
	}
	return canGoAhead;
}
bool upgradeCanBeEquipped(Upgrade* u) {
	if (!u->purchased) { return false; }
    return false; //(u->type == Upgrade::UPGRADE_TYPE_WEAPON);
}
void upgradesEvent_buy() {
    DefaultGame* dg = DefaultGame::getInstance();

    if (dg->stateUpgrades->m_introTimer > 0.0f ||
		dg->stateUpgrades->m_messageTimer > 0.0 ||
		dg->stateUpgrades->m_outroTimer > 0.0f) { return; }
    //dg->enterState(dg->stateMenu);
    bool forceBuy = ARK2D::getInput()->isKeyDown(Input::KEY_LSHIFT);

    UpgradesState* s = dg->stateUpgrades;
    if (s->m_overButtonIndex >= 0) {
    	Upgrade* u = s->m_upgradeData.at(s->m_overButtonIndex);

    	if (!u->purchased) {
			if (upgradeCanBeBought(u, forceBuy)) {
				dg->m_sfxMenuSelect->play();
				dg->m_sfxMenuBuy->play();
				u->purchased = true;

				if (!forceBuy) dg->m_coins -= u->cost;

				dg->m_save->set(StringUtil::append("upgradePurchased_", u->id), true);
				dg->m_save->set("purchasedAnything", true);
				dg->m_save->set("coins", (unsigned int)dg->m_coins);
				dg->m_save->save();
				dg->m_stats->submit("Upgraded", 1);

				if (dg->stateUpgrades->hasAllUpgrades()) {
					dg->m_stats->submit("All Upgrades", 1);
				}

				// if (u->type == Upgrade::UPGRADE_TYPE_GAS ||
    //                 u->type == Upgrade::UPGRADE_TYPE_FLAMES ||
    //                 u->type == Upgrade::UPGRADE_TYPE_MONEY ||
    //                 u->type == Upgrade::UPGRADE_TYPE_FEET ) {
					upgrades_setExclusiveEquippedForType(u);
				//}
				s->m_overButtonIndex = -1;
				return;
			}
    	}
    	else {
           if (u->typeIsEquippable) {// == Upgrade::UPGRADE_TYPE_WEAPON) {
               dg->m_sfxMenuSelect->play();
               upgrades_setExclusiveEquippedForType(u);
               s->m_overButtonIndex = -1;
           }
    	}

    }
}

Upgrade::Upgrade() {
	typeIsEquippable = false;
	hoverTimer = 0.0f;
	hoverDuration = 2.0f;
}
uint32_t Upgrade::getButtonBackgroundColor() {
	DefaultGame* dg = DefaultGame::getInstance();
    //BackdropPlanet* bp = dg->backdrop->planets[dg->backdrop->currentPlanet];

	uint32_t bgColor = Color::black.asInt();;
	if (dg->m_coins >= cost) { // can afford it.
	   bgColor = Color::pack("#972cb2");
	}
	if (equipped) { // has it
	   bgColor = Color::pack("#cb2347"); //dg->stateInGame->colorRed->asInt();; //bp->types.at(0 % bp->types.size())->color;
	}
	else if (purchased) { // has it
	   bgColor = Color::pack("#740a33");//dg->stateInGame->colorRed->asInt();; //bp->types.at(6 % bp->types.size())->color;
	}

    //uint32_t bgColor = Color::black.asInt();
	return bgColor;
}

void UpgradesState::init(GameContainer* container, StateBasedGame* game) {
	DefaultGame* dg = DefaultGame::getInstance();

	m_purchasedAnything = dg->m_save->getBoolean("purchasedAnything", false);

	Upgrade* u;

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_HP_1;
	u->type = Upgrade::UPGRADE_TYPE_HP;
	u->typeOrder = 0;
	u->gx = Upgrade::UPGRADE_TYPE_HP;
	u->gy = 0;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), true);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), true);
	u->cost = 0;
	u->name = "HEART";
	u->description = "A beating heart inside your chest. 1 hp!";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/hp-1.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_HP_2;
	u->type = Upgrade::UPGRADE_TYPE_HP;
	u->typeOrder = 1;
	u->gx = Upgrade::UPGRADE_TYPE_HP;
	u->gy = 1;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 15;
	u->name = "GAS SHIELD";
	u->description = "Made of CO2. 2 hp!";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/hp-2.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_HP_3;
	u->type = Upgrade::UPGRADE_TYPE_HP;
	u->typeOrder = 2;
	u->gx = Upgrade::UPGRADE_TYPE_HP;
	u->gy = 2;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 35;
	u->name = "SPRINKER";
	u->description = "A gentle spray, just when you need it. 3 hp!";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/hp-3.png");
	m_upgradeData.push_back(u);


	u = new Upgrade();
	u->id = Upgrade::UPGRADE_FLAMES_CAPACITY_SML;;
	u->type = Upgrade::UPGRADE_TYPE_FLAMES;
	u->typeOrder = 0;
	u->gx = Upgrade::UPGRADE_TYPE_FLAMES;
	u->gy = 0;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), true);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), true);
	u->cost = 0;
	u->name = "FLAMETHROWER";
	u->description = "You're a firestarter.";
	u->icon = SpriteSheetStore::getImage("sprites/fire/embers.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_FLAMES_CAPACITY_MED;;
	u->type = Upgrade::UPGRADE_TYPE_FLAMES;
	u->typeOrder = 1;
	u->gx = Upgrade::UPGRADE_TYPE_FLAMES;
	u->gy = 1;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 5;
	u->name = "TWISTED!";
	u->description = "Increased fire capacity.";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/fire-sml.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_FLAMES_CAPACITY_LRG;
	u->type = Upgrade::UPGRADE_TYPE_FLAMES;
	u->typeOrder = 2;
	u->gx = Upgrade::UPGRADE_TYPE_FLAMES;
	u->gy = 2;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 25;
	u->name = "OVERCOOKED";
	u->description = "Unlimited flames, maximum fun!";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/fire-med.png");
	m_upgradeData.push_back(u);


	// u = new Upgrade();
	// u->id = Upgrade::UPGRADE_FLAMES_EXTRA_HOT;
	// u->type = Upgrade::UPGRADE_TYPE_FLAMES;
	// u->typeOrder = 3;
	// u->gx = 0;
	// u->gy = 3;
	// u->purchased = false;
	// u->equipped = false;
	// u->cost = 25;
	// u->name = "SUPER HOT";
	// u->description = "Don't try this at home.";
	// u->icon = SpriteSheetStore::getImage("sprites/upgrades/fire-blue.png");
	// m_upgradeData.push_back(u);



	u = new Upgrade();
	u->id = Upgrade::UPGRADE_GAS_CAPACITY_SML;
	u->type = Upgrade::UPGRADE_TYPE_GAS;
	u->typeOrder = 0;
	u->gx = Upgrade::UPGRADE_TYPE_GAS;
	u->gy = 0;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), true);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), true);
	u->cost = 0;
	u->name = "CO2";
    u->description = "Put out flames at your leisure. (Right click.)";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/co2-sml.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_GAS_CAPACITY_MED;
	u->type = Upgrade::UPGRADE_TYPE_GAS;
	u->typeOrder = 1;
	u->gx = Upgrade::UPGRADE_TYPE_GAS;
	u->gy = 1;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 5;
	u->name = "CO3?";
    u->description = "Increased co2 capacity.";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/co2-med.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_GAS_CAPACITY_LRG;
	u->type = Upgrade::UPGRADE_TYPE_GAS;
	u->typeOrder = 2;
	u->gx = Upgrade::UPGRADE_TYPE_GAS;
	u->gy = 2;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 25;
	u->name = "Max Pressure CO2";
    u->description = string("Any more and it will explode!");
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/co2-lrg.png");
	m_upgradeData.push_back(u);


	u = new Upgrade();
	u->id = Upgrade::UPGRADE_WALLET_SIZE_SML;
	u->type = Upgrade::UPGRADE_TYPE_MONEY;
	u->typeOrder = 0;
	u->gx = Upgrade::UPGRADE_TYPE_MONEY;
	u->gy = 0;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), true);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), true);
	u->cost = 0;
	u->name = "POCKET";
	u->description = "Holds just 15 coins.";
	u->icon = SpriteSheetStore::getImage("sprites/coin-1.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_WALLET_SIZE_MED;
	u->type = Upgrade::UPGRADE_TYPE_MONEY;
	u->typeOrder = 1;
	u->gx = Upgrade::UPGRADE_TYPE_MONEY;
	u->gy = 1;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 15;
	u->name = "BIG SACK";
	u->description = "Holds 30 coins. That's better.";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/money-sml.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_WALLET_SIZE_LRG;
	u->type = Upgrade::UPGRADE_TYPE_MONEY;
	u->typeOrder = 2;
	u->gx = Upgrade::UPGRADE_TYPE_MONEY;
	u->gy = 2;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 30;
	u->name = "LUG";
	u->description = "Holds 50 coins. Stylish!";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/money-med.png");
	m_upgradeData.push_back(u);



	u = new Upgrade();
	u->id = Upgrade::UPGRADE_BARE_FEET;
	u->type = Upgrade::UPGRADE_TYPE_FEET;
	u->typeOrder = 0;
	u->typeIsEquippable = true;
	u->gx = Upgrade::UPGRADE_TYPE_FEET;
	u->gy = 0;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), true);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), true);
	u->cost = 0;
	u->name = "BARE FOOT";
	u->description = "Better watch where you walk.";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/walk-normal.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_HOT_FEET;
	u->type = Upgrade::UPGRADE_TYPE_FEET;
	u->typeOrder = 0;
	u->typeIsEquippable = true;
	u->gx = Upgrade::UPGRADE_TYPE_FEET;
	u->gy = 1;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 30;
	u->name = "HOT FEET";
	u->description = "Danger! Beware the fire walk!";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/walk-fire.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_FAST_FEET;
	u->type = Upgrade::UPGRADE_TYPE_FEET;
	u->typeOrder = 0;
	u->typeIsEquippable = true;
	u->gx = Upgrade::UPGRADE_TYPE_FEET;
	u->gy = 2;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 30;
	u->name = "FAST FEET";
	u->description = "Dance dance dance!";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/walk-speed.png");
	m_upgradeData.push_back(u);

	// u = new Upgrade();
	// u->id = Upgrade::UPGRADE_COLD_FEET;
	// u->type = Upgrade::UPGRADE_TYPE_FEET;
	// u->typeOrder = 2;
	// u->typeIsEquippable = true;
	// u->gx = Upgrade::UPGRADE_TYPE_FEET;
	// u->gy = 2;
	// u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	// u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	// u->cost = 50;
	// u->name = "COLD FEET";
	// u->description = "Warning! These boots make you... gassy.";
	// u->icon = SpriteSheetStore::getImage("sprites/upgrades/walk-co2.png");
	// m_upgradeData.push_back(u);



	u = new Upgrade();
	u->id = Upgrade::UPGRADE_HEALTH_VISION;
	u->type = Upgrade::UPGRADE_TYPE_VISION;
	u->typeOrder = 0;
	u->gx = Upgrade::UPGRADE_TYPE_VISION;
	u->gy = 0;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 10;
	u->name = "HEALTH VISION";
	u->description = "Know your enemy.";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/sight-health.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_NIGHT_VISION;
	u->type = Upgrade::UPGRADE_TYPE_VISION;
	u->typeOrder = 1;
	u->gx = Upgrade::UPGRADE_TYPE_VISION;
	u->gy = 1;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 20;
	u->name = "NIGHT VISION";
	u->description = "The power of a thousand carrots!";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/sight-nightvision.png");
	m_upgradeData.push_back(u);


	u = new Upgrade();
	u->id = Upgrade::UPGRADE_MAGNET;
	u->type = Upgrade::UPGRADE_TYPE_MISC1;
	u->typeOrder = 0;
	u->gx = Upgrade::UPGRADE_TYPE_MISC1;
	u->gy = 0;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradeEquipped_", u->id), false);
	u->cost = 5;
	u->name = "MAGNET";
	u->description = "Grab those coins!";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/magnet.png");
	m_upgradeData.push_back(u);

	u = new Upgrade();
	u->id = Upgrade::UPGRADE_BASEMENT_KEYS;
	u->type = Upgrade::UPGRADE_TYPE_MISC2;
	u->typeOrder = 0;
	u->gx = Upgrade::UPGRADE_TYPE_MISC1;
	u->gy = 1;
	u->purchased = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);;
	u->equipped = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);;
	u->cost = 50;
	u->name = "BASEMENT KEY";
	u->description = "Start from down in the deep: Floor -5.";
	u->icon = SpriteSheetStore::getImage("sprites/upgrades/key.png");
	m_upgradeData.push_back(u);






	btnBorder = SpriteSheetStore::getImage("sprites/ui-upgrade-btn.png");
	title = SpriteSheetStore::getImage("ui/upgrades.png");

	m_buttonBack = new ARK::UI::Button();
	m_buttonBack->setLocation(0, 0);
	m_buttonBack->setSize(24, 24);
	m_buttonBack->setEvent((void*) &upgradesEvent_back);
	m_buttonBack->setLocationByCenter(20, container->getHeight()-20);

	m_buttonPlay = new ARK::UI::Button();
	m_buttonPlay->setLocation(0, 0);
	m_buttonPlay->setSize(24, 24);
	m_buttonPlay->setEvent((void*) &upgradesEvent_play);
	m_buttonPlay->setLocationByCenter(container->getWidth()-20, container->getHeight()-20);

	m_overButtonIndex = -1;

	iconBack = SpriteSheetStore::getImage("sprites/ui-icon-back.png");
	iconPlay = SpriteSheetStore::getImage("sprites/ui-icon-play.png");

	int columns = Upgrade::UPGRADE_TYPE_MISC1+1;
	float allButtonsWidth = (26*columns) + (columns*3);
	float allButtonsX = ((284 - allButtonsWidth) * 0.5f) + (26*0.5f);
	m_upgradeButtons = new vector<ARK::UI::Button*>();
	for(int i = 0; i < m_upgradeData.size(); i++) {
		Upgrade* u = m_upgradeData.at(i);
		StringUtil::toUpper(u->name);
		StringUtil::toUpper(u->description);

		ARK:UI::Button* b = new ARK::UI::Button();
		b->setLocation(0, 0);
		b->setSize(26, 26);
		b->setEvent((void*) &upgradesEvent_click);
		b->setLocationByCenter(allButtonsX + (m_upgradeData.at(i)->gx * 29), 61 + (m_upgradeData.at(i)->gy * 28));

		m_upgradeButtons->push_back(b);
	}

	m_buttonUpgradeBuy = new ARK::UI::Button();
	m_buttonUpgradeBuy->setLocation(0, 0);
	m_buttonUpgradeBuy->setSize(50, 54);
	m_buttonUpgradeBuy->setEvent((void*) &upgradesEvent_buy);
	m_buttonUpgradeBuy->setLocationByCenter(210, 285);
	m_buttonUpgradeBuy->visible = false;

	m_buttonUpgradeEquip = new ARK::UI::Button();
	m_buttonUpgradeEquip->setLocation(0, 0);
	m_buttonUpgradeEquip->setSize(50, 54);
	m_buttonUpgradeEquip->setEvent((void*) &upgradesEvent_buy);
	m_buttonUpgradeEquip->setLocationByCenter(210, 285);
	m_buttonUpgradeEquip->visible = false;

	juiceTimer = 0.0f;
	juiceDuration = 2.5f;

	m_messageIndex = -1;
	m_messageTimer = 0.0f;
	m_messageDuration = 4.0f;

	m_introTimer = 0.0f;
	m_introDuration = 0.4f;

	m_outroTimer = 0.0f;
	m_outroDuration = 0.4f;
	m_outroTo = NULL;


	// m_blobFrames = AnimationFrameList::create("ui/store-blob-*.png", 1, 4, dg->spritesheet, dg->desc);
	// m_blob = new Animation();
	// m_blob->setFrameTime(0.1f);
	// m_blob->setFrames(m_blobFrames);

	// m_blobMessages.push_back("TRYING TO LEAVE THE PLANET?\nTRADE ME YOUR ENERGY CUBES!");
	// m_blobMessages.push_back("CHECK MY WARES. I'LL RECHARGE YOU FOR FREE!");
	// m_blobMessages.push_back("TERRY, HUH? THE NAME SOUNDS FAMILIAR...");
	// m_blobMessages.push_back("I'M STILL HERE! WHAT DO YOU NEED?");
	// m_blobMessages.push_back("...");
	//m_blobMessages.push_back("NOTHIN' LEFT HERE FOR YOU, BUD!");
}
bool UpgradesState::hasAnyUpgrade() {
	return m_purchasedAnything;
}
bool UpgradesState::hasAllUpgrades() {
	for(int i = 0; i < m_upgradeData.size(); i++) {
		Upgrade* u = m_upgradeData.at(i);
		//bool b = dg->m_save->getBoolean(StringUtil::append("upgradePurchased_", u->id), false);
		bool b = u->purchased;
		//if (!->purchased) {
		if (!b) {
			return false;
		}
	}
	return true;
}
Upgrade* UpgradesState::getUpgradeById(unsigned int id) {
	for(int i = 0; i < m_upgradeData.size(); i++) {
		if (m_upgradeData.at(i)->id == id) {
			return m_upgradeData.at(i);
		}
	}
	return NULL;
}

int UpgradesState::getCurrentHP() {
	unsigned int hpUpgrade = getCurrentForUpgradeType(Upgrade::UPGRADE_TYPE_HP);
    int hp = 1;
	if (hpUpgrade == Upgrade::UPGRADE_HP_1) {
		hp = 1;
	} else if (hpUpgrade == Upgrade::UPGRADE_HP_2) {
		hp = 2;
	} else if (hpUpgrade == Upgrade::UPGRADE_HP_3) {
		hp = 3;
	}
	return hp;
}
int UpgradesState::getCurrentFireCapacity() {
	unsigned int fireCap = getCurrentForUpgradeType(Upgrade::UPGRADE_TYPE_FLAMES);
    //return -1;

    int cap = 6;
	if (fireCap == Upgrade::UPGRADE_FLAMES_CAPACITY_SML) {
		cap = 6;
	} else if (fireCap == Upgrade::UPGRADE_FLAMES_CAPACITY_MED) {
		cap = 12;
	} else if (fireCap == Upgrade::UPGRADE_FLAMES_CAPACITY_LRG) {
		cap = -1;
	}
	// static const unsigned int UPGRADE_FLAMES_EXTRA_HOT = 3;
	return cap;
}
int UpgradesState::getCurrentCO2Capacity() {
	//return getCurrentFireCapacity();
	unsigned int co2Cap = getCurrentForUpgradeType(Upgrade::UPGRADE_TYPE_GAS);
	int cap = 0;
	if (co2Cap == Upgrade::UPGRADE_GAS_CAPACITY_SML) {
		cap = 2;
	} else if (co2Cap == Upgrade::UPGRADE_GAS_CAPACITY_MED) {
		cap = 6;
	} else if (co2Cap == Upgrade::UPGRADE_GAS_CAPACITY_LRG) {
		cap = -1;
	}
	return cap;
}
int UpgradesState::getCurrentMaxWallet() {
	unsigned int co2Cap = getCurrentForUpgradeType(Upgrade::UPGRADE_TYPE_MONEY);
	int cap = 15;
	if (co2Cap == Upgrade::UPGRADE_WALLET_SIZE_MED) {
		cap = 30;
	} else if (co2Cap == Upgrade::UPGRADE_WALLET_SIZE_LRG) {
		cap = 50;
	}
	return cap;
}
bool UpgradesState::hasMagnet() {
	// unsigned int co2Cap = getCurrentForUpgradeType(Upgrade::UPGRADE_TYPE_MONEY);
	// if (co2Cap == Upgrade::UPGRADE_MAGNET || co2Cap == Upgrade::UPGRADE_WALLET_SIZE_MED || co2Cap == Upgrade::UPGRADE_WALLET_SIZE_LRG) {
	// 	return true;
	// }
	// return false;
	return getUpgradeById(Upgrade::UPGRADE_MAGNET)->purchased;
}
bool UpgradesState::hasBasementKey() {
	return getUpgradeById(Upgrade::UPGRADE_BASEMENT_KEYS) != NULL && getUpgradeById(Upgrade::UPGRADE_BASEMENT_KEYS)->purchased;
}
bool UpgradesState::hasNightVision() {
	return getUpgradeById(Upgrade::UPGRADE_NIGHT_VISION) != NULL && getUpgradeById(Upgrade::UPGRADE_NIGHT_VISION)->purchased;
}
bool UpgradesState::hasHealthVision() {
	return (getUpgradeById(Upgrade::UPGRADE_HEALTH_VISION) != NULL && getUpgradeById(Upgrade::UPGRADE_HEALTH_VISION)->purchased) || hasNightVision();
}

//unsigned int UpgradesState::getCurrentWeapon() {
//    return getCurrentForUpgradeType(Upgrade::UPGRADE_TYPE_WEAPON);
//}
//unsigned int UpgradesState::getCurrentBattery() {
//    return getCurrentForUpgradeType(Upgrade::UPGRADE_TYPE_BATTERY);
//}
//int UpgradesState::getCurrentMaxAltitude() {
//    unsigned int u = getCurrentForUpgradeType(Upgrade::UPGRADE_TYPE_ALTITUDE);
//    return getMaxAltitude(u);
//}
//int UpgradesState::getMaxAltitude(int u) {
//    if (u == Upgrade::UPGRADE_ALTITUDE_TOWER) {
//        return 150;
//    }
//    else if (u == Upgrade::UPGRADE_ALTITUDE_CLOUDS) {
//        return 250;
//    }
//    else if (u == Upgrade::UPGRADE_ALTITUDE_SPACE) {
//        return -1;    // 600 max!
//    }
//    return 500;
//}

unsigned int UpgradesState::getCurrentForUpgradeType(unsigned int type) {
	for(int i = 0; i < m_upgradeData.size(); i++) {
		if (m_upgradeData.at(i)->type == type &&
			m_upgradeData.at(i)->equipped) {
			return m_upgradeData.at(i)->id;
		}
	}
	return 0;
}

void UpgradesState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {
	DefaultGame* dg = DefaultGame::getInstance();
	Input* i = ARK2D::getInput();

	if (DefaultGame::s_debug) {
		if (i->isKeyPressed(Input::KEY_I)) {
			dg->m_coins += 50;
		}
		if (i->isKeyPressed(Input::KEY_U)) {
			for(unsigned int i = 0; i < m_upgradeData.size(); i++) {
				m_upgradeData[i]->purchased = true;
				dg->m_save->set(StringUtil::append("upgradePurchased_", m_upgradeData[i]->id), true);
				dg->m_save->set("purchasedAnything", true);
				dg->m_save->save();
			}
			upgrades_setExclusiveEquippedForType(getUpgradeById(Upgrade::UPGRADE_HP_3));
			upgrades_setExclusiveEquippedForType(getUpgradeById(Upgrade::UPGRADE_FLAMES_CAPACITY_LRG));
			upgrades_setExclusiveEquippedForType(getUpgradeById(Upgrade::UPGRADE_GAS_CAPACITY_LRG));
			upgrades_setExclusiveEquippedForType(getUpgradeById(Upgrade::UPGRADE_WALLET_SIZE_LRG));
			upgrades_setExclusiveEquippedForType(getUpgradeById(Upgrade::UPGRADE_FAST_FEET));
			upgrades_setExclusiveEquippedForType(getUpgradeById(Upgrade::UPGRADE_NIGHT_VISION));
			//upgrades_setExclusiveEquippedForType(getUpgradeById(Upgrade::UPGRADE_BASEMENT_KEYS));

			if (hasAllUpgrades()) {
				dg->m_stats->submit("All Upgrades", 1);
			}
		}
	}

	if (DefaultGame::s_armorGames) {
		//dg->stateMenu->armorGamesOverlay->update();
	}

	if (m_messageTimer > 0.0f) {
		m_messageTimer += timer->getDelta();
		if (m_messageTimer >= m_messageDuration) {
			m_messageTimer = 0.0f;
			m_introTimer = 0.01f;
		}
	}
	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer > m_introDuration) {
			m_introTimer = 0.0f;
		}
	}

	if (m_outroTimer > 0.0f) {
		m_outroTimer += timer->getDelta();
		if (m_outroTo == dg->stateInGame) {
			dg->stateInGame->update(container,game,timer);
		}

		if (m_outroTimer > m_outroDuration) {
			if (m_outroTo == dg->stateInGame) {
				//dg->stateInGame->reset();
				//dg->stateInGame->start();
			}
			dg->enterState(m_outroTo);
			return;
		}
	}

	m_buttonBack->update();
	m_buttonPlay->update();
	//m_blob->update(timer);

	juiceTimer += timer->getDelta();
	if (juiceTimer >= juiceDuration) {
		juiceTimer -= juiceDuration;
	}
}

// void UpgradesState::renderBlobGuy() {
// 	float blobX = 270;
// 	float blobY = 280;

// 	if (m_outroTimer > 0.0f) {
// 		blobX = Easing::easebetween(Easing::QUADRATIC_IN_OUT, m_outroTimer, 270, 350, m_outroDuration);
// 		blobY = Easing::easebetweenback(Easing::QUADRATIC_OUT, Easing::QUADRATIC_IN, m_outroTimer, 280, 240, m_outroDuration);
// 	}

// 	Image* blobGuy = m_blob->getCurrentFrame();
// 	blobGuy->setFlipped(true, false);
// 	blobGuy->drawAligned(blobX, blobY, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, 1.0f);
// }

signed int UpgradesState::getPointerOverButtonIndex() {
	GameContainer* container = ARK2D::getContainer();

	signed int result = -1;
	for (int i = 0; i < m_upgradeButtons->size(); i++) {
		ARK::UI::Button* b = m_upgradeButtons->at(i);
		if (b->isPointerOver()) {
			result = i;
			break;
		}
	}
	return result;
}

float UpgradesState::getOutYMultiplier() {
	float mult = 0;
	if (m_introTimer > 0.0f) {
		mult = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 1.0f, 0.0f, m_introDuration);
	}
	else if (m_outroTimer > 0.0f) {
		mult = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 0.0f, 1.0f, m_outroDuration);
	}
	return mult;
}

void UpgradesState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	r->setDrawColor("#4e1353", 1.0f);
	r->fillRect(0,0,container->getWidth(), container->getHeight());

	float bgSpinClosedAm = 1.0f;
	if (m_introTimer > 0.0f && m_fromState == dg->stateInGame) {
		bgSpinClosedAm = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
		dg->stateInGame->render(container, game, r);
	} else if (m_outroTimer > 0.0f && m_outroTo == dg->stateInGame) {
		bgSpinClosedAm = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 1.0f, 0.0f, m_outroDuration);
		dg->stateInGame->render(container, game, r);
	}

	dg->stateMenu->drawSpinBG(bgSpinClosedAm);
	dg->stateMenu->drawDitherGradient(bgSpinClosedAm);


	r->setDrawColor(Color::white);
	//dg->backdrop->render(container, r);

	//renderBlobGuy();

	float baseUIY = 0.0f;

	if (m_messageTimer > 0.0f) {
		baseUIY = container->getHeight();

		float textY = 0.0f;
		if (m_messageTimer < 0.5f) {
			textY = Easing::easebetween(Easing::QUADRATIC_OUT, m_messageTimer, container->getHeight(), 0.0f, 0.5f);
		} else if (m_messageTimer > m_messageDuration - 0.5) {
			textY = Easing::easebetween(Easing::QUADRATIC_OUT, m_messageTimer - (m_messageDuration - 0.5), 0.0f, container->getHeight() * -1.0f, 0.5f);
		}

		//r->setDrawColorf(1.0f, 1.0f, 1.0f, textAlpha);
		// string message = m_blobMessages.at(m_messageIndex);
		// vector<string> lines = StringUtil::split(message, "\n");
		// if (lines.size() > 1) {
		// 	for(int i = 0; i < lines.size(); i++) {
		// 		dg->font->drawString(lines.at(i), 160 - 2.0f, textY+220 + (i*16) + 2.0f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);
		// 	}
		// } else {
		// 	dg->font->drawString(message, 160 - 2.0f, textY+220 + 2.0f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);
		// }
	}

	if (m_introTimer > 0.0f) {
		baseUIY = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, container->getHeight(), 0, m_introDuration);
	}
	else if (m_outroTimer > 0.0f) {
		baseUIY = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 0, container->getHeight() * -1.0f, m_outroDuration);
	}


    float widthMultiplier = Easing::easebetweenback(Easing::QUADRATIC_IN_OUT, Easing::QUADRATIC_IN_OUT, juiceTimer, 0.85f, 1.00f, juiceDuration);

	r->setDrawColor(Color::white);
	//title->drawCentered(120.0f, baseUIY+30);
	dg->font->drawString("UPGRADES", container->getWidth()*0.5f, baseUIY + 10, Renderer::ALIGN_CENTER, Renderer::ALIGN_TOP, 0.0f, 1.5f);


    r->setDrawColor(Color::white);
	Pickup::s_animCoin->frames[0]->drawAligned((container->getWidth()*0.5f) - 1, baseUIY+32, Renderer::ALIGN_RIGHT, Renderer::ALIGN_CENTER, 1.0f, 1.0f);
	r->drawString(StringUtil::append("", dg->m_coins), (container->getWidth()*0.5f) + 1, baseUIY+33, Renderer::ALIGN_LEFT, Renderer::ALIGN_CENTER, 0.0f, 1.0f);

	ThreeSliceButton::draw(m_buttonBack->transform.position.x, baseUIY+m_buttonBack->transform.position.y, m_buttonBack->getWidth());
	iconBack->drawCentered(m_buttonBack->transform.position.x, baseUIY+m_buttonBack->transform.position.y); //dg->font->drawString("BACK", m_buttonBack->transform.position.x - 2.0f, baseUIY+m_buttonBack->transform.position.y + 2.0f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);

	ThreeSliceButton::draw(m_buttonPlay->transform.position.x, baseUIY+m_buttonPlay->transform.position.y, m_buttonPlay->getWidth());
	iconPlay->drawCentered(m_buttonPlay->transform.position.x, baseUIY+m_buttonPlay->transform.position.y); //dg->font->drawString("BACK", m_buttonBack->transform.position.x - 2.0f, baseUIY+m_buttonBack->transform.position.y + 2.0f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);
	//dg->font->drawString("PLAY", m_buttonPlay->transform.position.x - 2.0f, baseUIY+m_buttonPlay->transform.position.y + 2.0f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);

	if (DefaultGame::s_armorGames) {
		//dg->stateMenu->armorGamesOverlay->render();
	}


	for(int i = 0; i < m_upgradeButtons->size(); i++) {
		ARK::UI::Button* b = m_upgradeButtons->at(i);

		uint32_t bgColor = m_upgradeData.at(i)->getButtonBackgroundColor();

		//b->render();
		ThreeSliceButton::draw(b->transform.position.x, baseUIY+b->transform.position.y, b->getWidth()-2, bgColor);
		//btnBorder->drawCentered(b->transform.position.x, baseUIY+b->transform.position.y);
        Image* icon = m_upgradeData.at(i)->icon;
		icon->drawCentered(b->transform.position.x, baseUIY+b->transform.position.y);
	}

	//if (!DefaultGame::isTouchMode()) {
		m_overButtonIndex = getPointerOverButtonIndex();
	//}
	m_buttonUpgradeBuy->visible = false;
	m_buttonUpgradeEquip->visible = false;


	if (m_overButtonIndex >= 0) {
		Upgrade* d = m_upgradeData.at(m_overButtonIndex);
		ARK::UI::Button* b = m_upgradeButtons->at(m_overButtonIndex);

		float hoverX = b->transform.position.x;
		float hoverY = baseUIY+b->transform.position.y + 10;

		float boxWidth = 170.0f;
		if (hoverX - (boxWidth*0.5f) < 0.0f) {
			hoverX = 110;
		} else if (hoverX + (boxWidth*0.5f) > container->getWidth()) {
			hoverX = container->getWidth() - 110;
		}

		uint32_t bgColor = d->getButtonBackgroundColor();

		string nameString;
		nameString += d->name;
		if (!d->purchased) {
			nameString += StringUtil::append(" (COST: ", d->cost);
			nameString += string(")");
		}
		else if (d->equipped) {
			nameString += string(" (CURRENT)");
		}

		ThreeSliceButton::draw(hoverX, hoverY, boxWidth, bgColor);
		dg->font->drawString(nameString, hoverX - 2.0f, hoverY + 2.0f - 4, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.75f);
		dg->font->drawString(d->description, hoverX - 2.0f, hoverY + 2.0f + 4, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);

		//if (DefaultGame::isTouchMode()) {
			if (upgradeCanBeBought(d, false)) {

				m_buttonUpgradeBuy->visible = true;
				m_buttonUpgradeBuy->setLocationByCenter(hoverX - 2, hoverY + 25);

				ThreeSliceButton::draw(m_buttonUpgradeBuy->transform.position.x, baseUIY+m_buttonUpgradeBuy->transform.position.y, m_buttonUpgradeBuy->getWidth()*widthMultiplier, bgColor);
				dg->font->drawString("BUY", m_buttonUpgradeBuy->transform.position.x - 2.0f, baseUIY+m_buttonUpgradeBuy->transform.position.y + 2.0f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);
			}
			else if (upgradeCanBeEquipped(d))  {
				m_buttonUpgradeEquip->visible = true;
				m_buttonUpgradeEquip->setLocationByCenter(hoverX - 2, hoverY + 25);

				ThreeSliceButton::draw(m_buttonUpgradeEquip->transform.position.x, baseUIY+m_buttonUpgradeEquip->transform.position.y, m_buttonUpgradeEquip->getWidth()*widthMultiplier, bgColor);
				dg->font->drawString("USE", m_buttonUpgradeEquip->transform.position.x - 2.0f, baseUIY+m_buttonUpgradeEquip->transform.position.y + 2.0f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);

			}


		//}
	}


	//float width = m_buttonBack->getWidth(); //Easing::easebetweenback(Easing::QUADRATIC_IN, Easing::QUADRATIC_OUT, juiceTimer, m_buttonBack->getWidth()*0.9, m_buttonBack->getWidth()*1.1f, juiceDuration);


	//dg->font->drawString("CLICK TO BUY!", 160 - 2.0f, baseUIY + 58, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);


	float startX = m_upgradeButtons->at(0)->transform.position.x - 1.0f;
	float eachX = 29;
	float titlesY = baseUIY + 45;
    dg->font->drawString("HP", startX, titlesY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
    dg->font->drawString("FIRE", startX + eachX, titlesY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
    dg->font->drawString("CO2", startX + (eachX*2), titlesY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
    dg->font->drawString("MONEY", startX + (eachX*3), titlesY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
    dg->font->drawString("FEET", startX + (eachX*4), titlesY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
    dg->font->drawString("VISION", startX + (eachX*5), titlesY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
    dg->font->drawString("MISC", startX + (eachX*6), titlesY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
//    dg->font->drawString("BATTERY", m_upgradeButtons->at(7)->transform.position.x - 1.0f, baseUIY + 60, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
//    dg->font->drawString("ALTITUDE", m_upgradeButtons->at(3)->transform.position.x - 1.0f, baseUIY + 60, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);
//    dg->font->drawString("MISC", m_upgradeButtons->at(10)->transform.position.x - 1.0f, baseUIY + 60, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 0.5f);




	//dg->font->drawString(StringUtil::appendf("m_introTimer: ", m_introTimer), 20, 20);
	//dg->font->drawString(StringUtil::appendf("m_outroTimer: ", m_outroTimer), 20, 40);
	//dg->font->drawString(StringUtil::appendf("m_messageTimer: ", m_messageTimer), 20, 60);
}

bool UpgradesState::keyPressed(unsigned int key) {
	if (m_buttonUpgradeBuy->visible && m_buttonUpgradeBuy->keyPressed(key)) { return true; }
	if (m_buttonUpgradeEquip->visible && m_buttonUpgradeEquip->keyPressed(key)) { return true; }
	m_buttonBack->keyPressed(key);
	m_buttonPlay->keyPressed(key);
	for(int i = 0; i < m_upgradeButtons->size(); i++) {
		m_upgradeButtons->at(i)->keyPressed(key);
	}
	if (DefaultGame::s_armorGames) {
        DefaultGame* dg = DefaultGame::getInstance();
		//dg->stateMenu->armorGamesOverlay->keyPressed(key);
	}
	return false;
}
bool UpgradesState::keyReleased(unsigned int key) {
	if (m_messageTimer > 0.0f && m_messageTimer < m_messageDuration - 0.5f) {
		m_messageTimer = m_messageDuration - 0.5f;//0.0f;
		//m_introTimer = 0.01f;
	}
	if (m_buttonUpgradeBuy->visible && m_buttonUpgradeBuy->keyReleased(key)) { return true; }
	if (m_buttonUpgradeEquip->visible && m_buttonUpgradeEquip->keyReleased(key)) { return true; }
	m_buttonBack->keyReleased(key);
	m_buttonPlay->keyReleased(key);

	bool released = false;
	for(int i = 0; i < m_upgradeButtons->size(); i++) {
		if (m_upgradeButtons->at(i)->keyReleased(key)) {
			released = true;
		}
	}

	if (!released) m_overButtonIndex = -1;

	if (DefaultGame::s_armorGames) {
        DefaultGame* dg = DefaultGame::getInstance();
		//dg->stateMenu->armorGamesOverlay->keyReleased(key);
	}

	return false;
}
bool UpgradesState::mouseMoved(int x, int y, int oldx, int oldy) {
    DefaultGame* dg = DefaultGame::getInstance();
    m_buttonUpgradeBuy->mouseMoved(x, y, oldx, oldy);
	m_buttonUpgradeEquip->mouseMoved(x, y, oldx, oldy);
	m_buttonBack->mouseMoved(x, y, oldx, oldy);
	m_buttonPlay->mouseMoved(x, y, oldx, oldy);
	for(int i = 0; i < m_upgradeButtons->size(); i++) {
		m_upgradeButtons->at(i)->mouseMoved(x, y, oldx, oldy);
	}
	if (DefaultGame::s_armorGames) {
		//dg->stateMenu->armorGamesOverlay->mouseMoved(x, y, oldx, oldy);
	}
	return false;
}

UpgradesState::~UpgradesState() {

}
