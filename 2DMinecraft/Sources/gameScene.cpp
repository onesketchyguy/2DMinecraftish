#include "../Headers/gameScene.h"

void GameScene::MovePlayer()
{
	float speed = 500.0f * time->elapsedTime;

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

	if (velocity.mag2() > 0) velocity = velocity.norm() * 4.0f;
	velocity *= speed;

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
			if (worldData->GetWorldGenerating() == false)
			{
				if (worldSeed.empty() == false) worldData->ApplySeed(worldSeed);
				worldData->GenerateMap();
				minimap->Initialize(renderer->tileSpriteData->Sprite(), worldData, engine, time);
				vWorldSize = { worldData->GetMapWidth(), worldData->GetMapHeight() };

				if (playMode != PLAY_MODE::CLIENT)
				{ // Auto accept this client
					if (playMode == PLAY_MODE::SERVER) print("Server started.");
					if (playMode == PLAY_MODE::SINGLE_PLAYER) print("Single player session started.");

					descPlayer.position = worldData->GetRandomGroundTile();

					if (descPlayer.position.x <= 0.001f && descPlayer.position.y <= 0.001f)
					{
						DEBUG = true;
						print("Failed to generate map properly!");

						return false;
					}

					mapObjects.insert_or_assign(0, descPlayer);
					waitingForConnection = false;

					localPlayer = new PlayerObject(&GetLocalPlayer());
				}
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
			olc::net::message<GameMsg> msg = client->Incoming().pop_front().msg;

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

bool GameScene::OnLoad()
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

	itemSlotRenderable = new olc::Renderable();
	itemSlotRenderable->Load("Data/item_slot.png");

	minimap = new MiniMap();

	if (playMode == PLAY_MODE::CLIENT) TryConnect(serverIP.c_str());

	return true;
}

bool GameScene::Update()
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
	renderer->SetCamera(GetLocalPlayer().position);

	renderer->DrawWorld();

	// Control of Player Object
	MovePlayer();
	localPlayer->Update(time->elapsedTime);

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
		renderer->DrawPlayer(object.second);

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
	DrawToolbarArea();

	if (GetKey(olc::Key::E).bPressed || GetKey(olc::Key::I).bPressed)
	{
		inventoryOpen = !inventoryOpen;
	}

	if (inventoryOpen)
	{
		DrawInventory();
	}

	// Show notifications on top
	HandleNotifications();
	HandleToolTip();

	// DEBUG: just for testing here we have some upgrades for the slotbar
	int newHotbar = 0;

	if (GetKey(olc::Key::NP_ADD).bPressed)
	{
		newHotbar++;
	}

	if (GetKey(olc::Key::NP_SUB).bPressed)
	{
		newHotbar--;
	}

	ModifyHotbarSlots(newHotbar);

	// DEBUG: add items to inventory
	for (size_t i = 1; i < TOOLS_COUNT; i++)
	{
		if (GetKey(olc::Key(69 + i)).bPressed)
		{
			Tools item = (Tools)i;

			AddToolItemToInventory(item);
		}
	}

	return true;
}