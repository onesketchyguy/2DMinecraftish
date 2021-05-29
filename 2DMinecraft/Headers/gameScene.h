// Forrest Lowe 2021
#ifndef GAME_SCENE
#define GAME_SCENE

class GameScene : public Scene
{
private:
	GameServer* server = nullptr;
	Client* client = nullptr;

	WorldData* worldData;

	Renderer* renderer = nullptr;
	MiniMap* minimap = nullptr;

	olc::Renderable* toolsRenderable = nullptr;

private:
	olc::vi2d vWorldSize = { MAP_WIDTH, MAP_HEIGHT };

private:
	void MovePlayer();
	bool ValidateWorld();

private: // Networking stuff
	std::unordered_map<uint32_t, PlayerDescription> mapObjects;
	uint32_t playerID = 0;
	PlayerDescription descPlayer;

	PlayerObject* localPlayer;

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
	bool OnLoad() override
	{
		if (playMode == PLAY_MODE::SERVER)
		{
			server = new GameServer(SERVER_PORT);
			server->Start();
		}

		worldData = new WorldData();
		renderer = new Renderer(engine, worldData);

		toolsRenderable = new olc::Renderable();
		toolsRenderable->Load("Data/tools.png");

		minimap = new MiniMap();
		minimap->Initialize(renderer->tileSpriteData->Sprite(), worldData, engine, time);

		if (playMode == PLAY_MODE::CLIENT) TryConnect(serverIP.c_str());

		return true;
	}

	bool Update() override
	{
		engine->Clear(olc::BLACK);

		if (GetKey(olc::ESCAPE).bReleased)
		{
			//FIXME: Toggle pause

			currentScene = SCENE::SCENE_MAIN_MENU;

			if (playMode == PLAY_MODE::SERVER)
			{
				// FIXME: Move the host or kick all players
			}

			return false;
		}

		// Dont continue if the world is invalid
		if (ValidateWorld() == false)
		{
			return true;
		}

		// Check for incoming network messages
		if (playMode == PLAY_MODE::CLIENT)
		{
			HandleNetworkingMessages();

			if (waitingForConnection)
			{
				engine->Clear(olc::DARK_BLUE);
				engine->DrawStringDecal({ 10,10 }, "Waiting to connect...", olc::WHITE);
				return true;
			}
		}

		// Handle Pan & Zoom
		renderer->UpdateZoom();
		renderer->SetCamera(GetLocalPlayer().position / SPRITE_SCALE);

		renderer->DrawWorld();

		// Control of Player Object
		MovePlayer();

		// Update objects locally
		for (auto& object : mapObjects)
		{
			// Where will object be worst case?
			olc::vf2d vPotentialPosition = object.second.position + object.second.velocity * time->elapsedTime;

			// Extract region of world cells that could have collision this frame
			olc::vi2d vCurrentCell = object.second.position.floor();
			olc::vi2d vTargetCell = vPotentialPosition;
			olc::vi2d vAreaTL = (vCurrentCell.min(vTargetCell) - olc::vi2d(1, 1)).max({ 0,0 });
			olc::vi2d vAreaBR = (vCurrentCell.max(vTargetCell) + olc::vi2d(1, 1)).min(vWorldSize);

			// Iterate through each cell in test area
			olc::vi2d vCell;
			for (vCell.y = vAreaTL.y; vCell.y <= vAreaBR.y; vCell.y++)
			{
				for (vCell.x = vAreaTL.x; vCell.x <= vAreaBR.x; vCell.x++)
				{
					// Check if the cell is actually solid...
					//	olc::vf2d vCellMiddle = vCell.floor();
					int mapIndex = vCell.y * vWorldSize.x + vCell.x;

					if (worldData->tileData[mapIndex] == 0)
					{
						// ...it is! So work out nearest point to future player position, around perimeter
						// of cell rectangle. We can test the distance to this point to see if we have
						// collided.

						olc::vf2d vNearestPoint;
						// Inspired by this (very clever btw)
						// https://stackoverflow.com/questions/45370692/circle-rectangle-collision-response
						vNearestPoint.x = std::max(float(vCell.x), std::min(vPotentialPosition.x, float(vCell.x + 1)));
						vNearestPoint.y = std::max(float(vCell.y), std::min(vPotentialPosition.y, float(vCell.y + 1)));

						// But modified to work :P
						olc::vf2d vRayToNearest = vNearestPoint - vPotentialPosition;
						float fOverlap = object.second.radius - vRayToNearest.mag();
						if (std::isnan(fOverlap)) fOverlap = 0;// Thanks Dandistine!

						// If overlap is positive, then a collision has occurred, so we displace backwards by the
						// overlap amount. The potential position is then tested against other tiles in the area
						// therefore "statically" resolving the collision
						if (fOverlap > 0)
						{
							// Statically resolve the collision
							vPotentialPosition = vPotentialPosition - vRayToNearest.norm() * fOverlap;
						}
					}
				}
			}

			// Set the objects new position to the allowed potential position
			object.second.position = vPotentialPosition;
		}

		// Draw World Objects
		for (auto& object : mapObjects)
		{
			// Draw object
			renderer->DrawPlayer(object.second.position / SPRITE_SCALE);

			// Draw Boundary
			renderer->viewPort.DrawCircle(object.second.position / SPRITE_SCALE, object.second.radius);

			// Draw Velocity
			if (object.second.velocity.mag2() > 0)
				renderer->viewPort.DrawLine(object.second.position / SPRITE_SCALE, object.second.position + object.second.velocity.norm() * object.second.radius, olc::MAGENTA);

			// Draw Name
			olc::vi2d vNameSize = engine->GetTextSizeProp("ID: " + std::to_string(object.first));
			renderer->viewPort.DrawStringPropDecal(object.second.position - olc::vf2d{ vNameSize.x * 0.5f * 0.25f * 0.125f, -object.second.radius * 1.25f }, "ID: " + std::to_string(object.first), olc::BLUE, { 0.25f, 0.25f });
		}

		minimap->UpdateMiniMap(GetLocalPlayer().position);

		if (playMode != PLAY_MODE::SINGLE_PLAYER)
		{
			// Send player description
			HandleClient();

			if (playMode == PLAY_MODE::SERVER)
			{
				//server->Update(-1, true);
				server->Update();
			}
		}

		// Draw UI

		Tools& currentTool = localPlayer->currentTool;

		if (GetKey(olc::Key::K1).bReleased)
		{
			switch (currentTool)
			{
			case Tools::None:
				currentTool = Tools::Hoe;
				break;
			case Tools::Shovel:
				currentTool = Tools::None;
				break;
			case Tools::Axe:
				currentTool = Tools::Shovel;
				break;
			case Tools::Pickaxe:
				currentTool = Tools::Axe;
				break;
			case Tools::Hoe:
				currentTool = Tools::Pickaxe;
				break;
			default:
				break;
			}
		}

		if (GetKey(olc::Key::K2).bReleased)
		{
			switch (currentTool)
			{
			case Tools::None:
				currentTool = Tools::Shovel;
				break;
			case Tools::Shovel:
				currentTool = Tools::Axe;
				break;
			case Tools::Axe:
				currentTool = Tools::Pickaxe;
				break;
			case Tools::Pickaxe:
				currentTool = Tools::Hoe;
				break;
			case Tools::Hoe:
				currentTool = Tools::None;
				break;
			default:
				break;
			}
		}

		// Draw current tool in the bottom right corner
		if (currentTool != Tools::None)
		{
			olc::vi2d toolSpritePos = { int(currentTool) - 1 , 0 };
			engine->DrawPartialDecal(
				{ engine->ScreenWidth() - SPRITE_SCALE * 2.0f, engine->ScreenHeight() - SPRITE_SCALE * 2.0f },
				{ SPRITE_SCALE * 2, SPRITE_SCALE * 2 }, toolsRenderable->Decal(),
				toolSpritePos * (SPRITE_SCALE * 2), { SPRITE_SCALE * 2, SPRITE_SCALE * 2 });
		}

		return true;
	}
};

void GameScene::MovePlayer()
{
	float speed = 50.0f * time->elapsedTime;

	olc::vf2d velocity = { 0.0f, 0.0f };

	if (GetKey(olc::A).bHeld || GetKey(olc::LEFT).bHeld) {
		velocity += { -1.0f, 0.0f };
	}
	if (GetKey(olc::D).bHeld || GetKey(olc::RIGHT).bHeld) {
		velocity += { +1.0f, 0.0f };
	}

	if (GetKey(olc::W).bHeld || GetKey(olc::UP).bHeld) {
		velocity += { 0.0f, -1.0f };
	}
	if (GetKey(olc::S).bHeld || GetKey(olc::DOWN).bHeld) {
		velocity += { 0.0f, +1.0f };
	}

	velocity *= speed;

	if (velocity.mag2() > 0) velocity = velocity.norm() * 4.0f;

	// Basic world collision here
	olc::vf2d playerPos = GetLocalPlayer().position;
	playerPos += velocity;

	int tileId = worldData->GetTileID(playerPos.x, playerPos.y);

	// in water
	//localPlayer->inWater = (tileId == 0 || tileId == 1);

	if (tileId != 0)
	{
		// FIXME: do things here
		// There is a tile where the player is trying to go!
	}
	else
	{
		// FIXME: Check for a boat
		velocity = { 0, 0 };
	}

	GetLocalPlayer().velocity = velocity;
	//localPlayer->Update(fElapsedTime);
}

bool GameScene::ValidateWorld()
{
	if (worldData->GetWorldGenerated() == false)
	{
		if (worldData->GetWorldProgress() < 0.99f)
		{
			engine->Clear(olc::DARK_BLUE);
			engine->DrawStringDecal({ 10,10 }, "Loading...", olc::WHITE);
			return false;
		}
		else
		{
			worldData->GenerateMap();

			if (playMode != PLAY_MODE::CLIENT)
			{ // Auto accept this client
				print("You're the host!");
				descPlayer.position = worldData->GetRandomGroundTile();

				if (descPlayer.position.x <= -0.9f && descPlayer.position.y <= -0.9f)
				{
					worldData->GenerateMap();
					return false;
				}

				mapObjects.insert_or_assign(0, descPlayer);
				waitingForConnection = false;

				localPlayer = new PlayerObject(&GetLocalPlayer());
			}

			return false;
		}
	}

	// Done
	return true;
}

void GameScene::HandleNetworkingMessages()
{
	if (client->IsConnected())
	{
		while (client->Incoming().empty() == false)
		{
			auto msg = client->Incoming().pop_front().msg;

			switch (msg.header.id)
			{
			case(GameMsg::Client_Accepted):
			{
				print("Server accepted client - you're in!");
				olc::net::message<GameMsg> msg;
				msg.header.id = GameMsg::Client_RegisterWithServer;
				descPlayer.position = worldData->GetRandomGroundTile();
				msg << descPlayer;
				client->Send(msg);
				break;
			}

			case(GameMsg::Client_AssignID):
			{
				// Server is assigning us OUR id
				msg >> playerID;
				print("Assigned Client ID = " + playerID);
				break;
			}

			case(GameMsg::Game_AddPlayer):
			{
				PlayerDescription desc;
				msg >> desc;
				mapObjects.insert_or_assign(desc.uniqueID, desc);

				if (desc.uniqueID == playerID)
				{
					// Now we exist in game world
					waitingForConnection = false;
				}
				break;
			}

			case(GameMsg::Game_RemovePlayer):
			{
				uint32_t nRemovalID = 0;
				msg >> nRemovalID;
				mapObjects.erase(nRemovalID);
				break;
			}

			case(GameMsg::Game_UpdatePlayer):
			{
				PlayerDescription desc;
				msg >> desc;
				mapObjects.insert_or_assign(desc.uniqueID, desc);
				break;
			}
			}
		}
	}
}

void GameScene::HandleClient()
{
	olc::net::message<GameMsg> msg;
	msg.header.id = GameMsg::Game_UpdatePlayer;
	msg << GetLocalPlayer();
	client->Send(msg);
}

#endif // !GAME_SCENE