// Forrest Lowe 2021

#pragma once
#ifndef GAME_SCENE
#define GAME_SCENE

#include "playerData.h"

#include <unordered_map>

#include "sceneDefinition.h"
#include "ConstantData.h"
#include "debugger.h"

#include "timeConstruct.h"

#include "worldData.h"
#include "miniMap.h"

#include "objectDefinitions.h"
#include "renderer.h"

class GameScene : public Scene
{
private:
	GameServer* server = nullptr;
	Client* client = nullptr;

	WorldData* worldData = nullptr;
	MiniMap* minimap = nullptr;

	PlayerObject* localPlayer = nullptr;

	const static int MAX_HOTBAR_SLOTS = 9;
	const static int MIN_HOTBAR_SLOTS = 2;
	uint8_t currentHotbarSlot = -1;
	uint8_t hotbarSlotCount = 3;

	wchar_t hotbarInventory[MAX_HOTBAR_SLOTS];

	bool inventoryOpen = false;

	const static int MAX_INVENTORY_SLOTS = 36;
	const static int MIN_INVENTORY_SLOTS = 0;
	uint8_t inventorySlotCount = 36;

	std::wstring playerInventory;

	olc::vf2d toolTipPos{};
	std::string toolTipText = "";

public:
	GameScene() = default;
	~GameScene()
	{
		delete server;
		delete client;
		delete worldData;
		delete minimap;
		delete localPlayer;
		mapObjects.clear();
	}

private:
	void MovePlayer();
	bool ValidateWorld();

private: // Networking stuff
	std::unordered_map<uint32_t, PlayerDescription> mapObjects;
	uint32_t playerID = 0;
	PlayerDescription descPlayer;

	PlayerDescription& GetLocalPlayer()
	{
		return mapObjects.at(playerID);
	}

	bool waitingForConnection = true;

	bool TryConnect(const char* ip = "127.0.0.1", uint16_t port = SERVER_PORT)
	{
		if (client != nullptr) return false;
		client = new Client();

		return client->Connect(ip, port);
	}

	void HandleNetworkingMessages();
	void HandleClient();

public: // Inventory stuff

	// NOTE: later we will need this to be a generic ITEM
	Tools GetToolItemFromInventory(const uint32_t i);
	Tools GetToolItemFromHotbar(const uint32_t i);

	void RemoveToolItemFromHotbar(const uint32_t i);

	// NOTE: later we will need this to be a generic ITEM
	bool AddToolItemToInventory(const Tools item);
	bool AddToolItemToHotbarSlot(const Tools item, int slot, bool overrideItem = false);
	bool AddToolItemToHotbar(const Tools item);

public: // HUD
	std::vector<std::string> notifications;
	float totalNotificationTime = 0.0f; // Amount of time left on current notication
	float currentNotificationTime = 0.0f; // Amount of time left on current notication
	float NOTIFICATION_SHOW_TIME = 3.0f; // Amount of time each notification will show for

	void DisplayNotification(std::string notification);
	void HandleNotifications();

	void HandleToolTip();

	void DrawInventory();
	void DrawToolbarArea();
	void ModifyHotbarSlots(int mod);
	void ModifyInventorySlots(int mod);
	void ChangeCurrentHotbarSlot(int index);

public:
	bool OnLoad() override;
	bool Update() override;
};

#endif // !GAME_SCENE