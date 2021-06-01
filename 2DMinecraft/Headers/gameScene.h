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

	olc::Renderable* toolsRenderable = nullptr;

	PlayerObject* localPlayer = nullptr;

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

public:
	bool OnLoad() override;
	bool Update() override;
};

#endif // !GAME_SCENE