// Forrest Lowe 2021

#pragma once
#ifndef GAME_SCENE
#define GAME_SCENE

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

	Renderer* renderer = nullptr;
	MiniMap* minimap = nullptr;

	const olc::vf2d CAM_OFFSET{ 0.0f, 25.0f };

	olc::Renderable* toolsRenderable = nullptr;
	olc::Renderable* itemSlotRenderable = nullptr;

	PlayerObject* localPlayer = nullptr;

	std::wstring playerInventory;

	const int MAX_HOTBAR_SLOTS = 9;
	const int MIN_HOTBAR_SLOTS = 2;
	uint8_t currentSlot = -1;
	uint8_t slotCount = 3;

public:
	~GameScene()
	{
		delete server;
		delete client;
		delete worldData;
		delete renderer;
		delete minimap;
		delete toolsRenderable;
		delete localPlayer;
	}

private:
	olc::vi2d vWorldSize;

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
	Tools GetToolItemFromInventory(const int i)
	{
		if (i >= playerInventory.length())
			return Tools::None;

		auto c = playerInventory.at(i);

		return static_cast<Tools>(c);
	}

	// NOTE: later we will need this to be a generic ITEM
	void AddToolItemFromInventory(const Tools item)
	{
		wchar_t itemChar = static_cast<wchar_t>(item);

		switch (item)
		{
		case Tools::Shovel:
			playerInventory.push_back(itemChar);
			std::cout << itemChar << " Shovel" << std::endl;
			break;
		case Tools::Pickaxe:
			playerInventory.push_back(itemChar);
			std::cout << itemChar << " Pickaxe" << std::endl;
			break;
		case Tools::Axe:
			playerInventory.push_back(itemChar);
			std::cout << itemChar << " Axe" << std::endl;
			break;
		case Tools::Hoe:
			playerInventory.push_back(itemChar);
			std::cout << itemChar << " Hoe" << std::endl;
			break;
		default:
			// Do nothing
			print("WARNING! Atempted to add item to inventory. No item added.");
			break;
		}

		auto val = GetToolItemFromInventory(playerInventory.length() - 1);
		std::cout << "Item = " << static_cast<wchar_t>(val) << std::endl;
	}

public: // HUD
	void DrawToolbarArea();
	void ModifyHotbarSlots(int mod);
	void ChangeCurrentHotbarSlot(int index)
	{
		if (currentSlot != index)
		{
			currentSlot = index;
		}
		else
		{
			currentSlot = -1;
			localPlayer->currentTool = Tools::None;
		}
	}

public:
	bool OnLoad() override;
	bool Update() override;
};

#endif // !GAME_SCENE