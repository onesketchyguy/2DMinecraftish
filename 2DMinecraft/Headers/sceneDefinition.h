#pragma once

/* example of use
class MainMenu : public Scene
{
public:
	bool OnLoad() override
	{
		return true;
	}

	bool Update() override
	{
		return true;
	}
};*/

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPGEX_TransformedView.h"

enum class SCENE : uint8_t
{
	SCENE_INTRO,
	SCENE_MAIN_MENU,
	SCENE_MP_LOBBY,
	SCENE_SINGLE_PLAYER,
	SCENE_MULTI_PLAYER,
	TEMP_HOST
};

SCENE currentScene = SCENE::SCENE_INTRO;

struct TimeConstruct
{
public:
	// Amount of time since the last frame
	float elapsedTime = 0.0f;

	// Number of frames passed since the application has started. Note that this loops.
	int32_t frameCount = 0;

	void Update(float elapsedTime)
	{
		this->elapsedTime = elapsedTime;

		frameCount++;
		frameCount %= 4294967294; // Keep the frame count within the range of its values.
	}
};

class Scene
{
public:
	void Initialize(TimeConstruct* time, olc::PixelGameEngine* engine)
	{
		this->time = time;
		this->engine = engine;
	}

protected:
	olc::PixelGameEngine* engine;

	TimeConstruct* time;

	olc::HWButton GetKey(olc::Key key)
	{
		return engine->GetKey(key);
	}

	int32_t ScreenWidth()
	{
		return engine->ScreenWidth();
	}

	int32_t ScreenHeight()
	{
		return engine->ScreenHeight();
	}

private:
	bool sceneLoaded = false;

public:
	bool UpdateScene()
	{
		if (!sceneLoaded)
		{
			sceneLoaded = OnLoad();
		}
		else
		{
			sceneLoaded = Update();
		}

		return sceneLoaded;
	}

	virtual bool OnLoad() { return true; }
	virtual bool Update() { return true; }
};

class IntroScene : public Scene
{
	olc::Renderable* olc_logo;
	olc::Renderable* flowe_logo;
	olc::vf2d logoPos;
	olc::vf2d olc_logoSize = { 0.25f, 0.25f };
	olc::vf2d flowe_logoSize = { 1.25f, 1.25f };

	float logoTime = 3;

public:
	bool OnLoad() override
	{
		logoPos.x = ScreenWidth() / 2;
		logoPos.y = ScreenHeight() / 2;

		olc_logo = new olc::Renderable();
		olc_logo->Load("Data/olc_logo.png");

		flowe_logo = new olc::Renderable();
		flowe_logo->Load("Data/flowe_logo.png");

		logoPos.x -= olc_logo->Sprite()->width * (olc_logoSize.x / 2.0f);
		logoPos.y -= olc_logo->Sprite()->height * (olc_logoSize.y / 2.0f);

		return true;
	}

	bool Update() override
	{
		engine->Clear(olc::DARK_BLUE);

		engine->DrawDecal({ logoPos.x, logoPos.y - 50 }, olc_logo->Decal(), olc_logoSize);
		engine->DrawDecal({ logoPos.x, logoPos.y + 50 }, flowe_logo->Decal(), flowe_logoSize);

		logoTime -= time->elapsedTime;
		if (logoTime <= 0)
		{
			currentScene = SCENE::SCENE_MAIN_MENU;
		}

		return true;
	}
};

class MainMenu : public Scene
{
private:
	TextBox* title;

	TextBox* singlePlayerButton;
	TextBox* multiPlayerButton;
	TextBox* quitButton;

	olc::Pixel defaultColor = olc::WHITE;
	olc::Pixel highlightColor = olc::GREY;

public:
	bool OnLoad() override
	{
		int xPos = (ScreenWidth() / 2) - 75;
		int yPos = (ScreenHeight() / 2) - 50;

		title = new TextBox(engine, "Mini-craft",
			xPos, yPos,
			100, 25,
			2, 2,
			0, 0,
			olc::WHITE);

		xPos += 25;
		yPos += 40;

		singlePlayerButton = new TextBox(engine, "single player",
			xPos, yPos,
			125, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		yPos += 40;

		multiPlayerButton = new TextBox(engine, "multi player",
			xPos, yPos,
			125, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		xPos += 25;
		yPos += 40;

		quitButton = new TextBox(engine, "quit",
			xPos, yPos,
			60, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		return true;
	}

	bool Update() override
	{
		// Draw the menu

		engine->Clear(olc::Pixel(15, 180, 180, 255));
		title->Draw();
		singlePlayerButton->Draw();
		multiPlayerButton->Draw();
		quitButton->Draw();

		// Singleplayer button
		if (singlePlayerButton->MouseOver())
		{
			singlePlayerButton->fillColor = highlightColor;

			if (engine->GetMouse(0).bReleased)
			{
				currentScene = SCENE::SCENE_SINGLE_PLAYER;
			}
		}
		else singlePlayerButton->fillColor = defaultColor;

		// Multiplayer button
		if (multiPlayerButton->MouseOver())
		{
			multiPlayerButton->fillColor = highlightColor;

			if (engine->GetMouse(0).bReleased)
			{
				// FIXME: We really should be switching to a scene where the user can input an IP
				currentScene = SCENE::SCENE_MP_LOBBY;
			}
		}
		else multiPlayerButton->fillColor = defaultColor;

		// Quit button
		if (quitButton->MouseOver())
		{
			quitButton->fillColor = highlightColor;

			if (engine->GetMouse(0).bReleased)
			{
				APPLICATION_RUNNING = false;
			}
		}
		else quitButton->fillColor = defaultColor;

		return true;
	}
};

class MultiplayerLobby : public Scene
{
private:
	TextBox* hostButton;
	TextBox* joinButton;
	TextBox* backButton;

	olc::Pixel defaultColor = olc::WHITE;
	olc::Pixel highlightColor = olc::GREY;

public:
	bool OnLoad() override
	{
		int xPos = (ScreenWidth() / 2) - 75;
		int yPos = (ScreenHeight() / 2) - 50;

		hostButton = new TextBox(engine, "host game",
			xPos, yPos,
			125, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		yPos += 40;

		joinButton = new TextBox(engine, "join game",
			xPos, yPos,
			125, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		xPos += 25;
		yPos += 40;

		backButton = new TextBox(engine, "back",
			xPos, yPos,
			60, 25,
			1, 1,
			10, 10,
			olc::BLACK, defaultColor);

		return true;
	}

	bool Update() override
	{
		// Draw the menu

		engine->Clear(olc::Pixel(15, 180, 180, 255));
		hostButton->Draw();
		joinButton->Draw();
		backButton->Draw();

		// Host button
		if (hostButton->MouseOver())
		{
			hostButton->fillColor = highlightColor;

			if (engine->GetMouse(0).bReleased)
			{
				IS_SERVER = true;
				currentScene = SCENE::SCENE_MULTI_PLAYER;
			}
		}
		else hostButton->fillColor = defaultColor;

		// Join button
		if (joinButton->MouseOver())
		{
			joinButton->fillColor = highlightColor;

			if (engine->GetMouse(0).bReleased)
			{
				IS_SERVER = false;
				currentScene = SCENE::SCENE_MULTI_PLAYER;
			}
		}
		else joinButton->fillColor = defaultColor;

		// Quit button
		if (backButton->MouseOver())
		{
			backButton->fillColor = highlightColor;

			if (engine->GetMouse(0).bReleased)
			{
				currentScene = SCENE::SCENE_MAIN_MENU;
			}
		}
		else backButton->fillColor = defaultColor;

		return true;
	}
};

class SinglePlayer : public Scene
{
public:
	Renderer* renderer;
	WorldData* worldData;

	Object* localPlayer;
	int tileId;

	olc::Sprite* miniMapSprite;
	olc::Decal* miniMapDecal;

	std::vector<Item> items;

	bool drawMiniMap = false;
	byte miniMapDrawLocation = 1;
	float miniMapDrawScale = 0.25f;
	olc::Pixel* mapColors;

private:
	std::unordered_map<uint32_t, PlayerDescription> mapObjects;
	uint32_t playerID = 0;
	PlayerDescription descPlayer;

	bool waitingForConnection = true;

public:
	void MovePlayer(float fElapsedTime)
	{
		float speed = 50.0f;

		float x = 0;
		float y = 0;

		if (GetKey(olc::A).bHeld || GetKey(olc::LEFT).bHeld) {
			x -= speed * fElapsedTime;
		}
		if (GetKey(olc::D).bHeld || GetKey(olc::RIGHT).bHeld) {
			x += speed * fElapsedTime;
		}

		if (GetKey(olc::W).bHeld || GetKey(olc::UP).bHeld) {
			y -= speed * fElapsedTime;
		}
		if (GetKey(olc::S).bHeld || GetKey(olc::DOWN).bHeld) {
			y += speed * fElapsedTime;
		}

		// Basic world collision here
		olc::vf2d playerPos = localPlayer->GetPosition();
		playerPos += {x, y};

		tileId = worldData->GetTileID(playerPos.x, playerPos.y);

		// in water
		localPlayer->inWater = (tileId == 0 || tileId == 1);

		if (tileId != 0)
		{
			// FIXME: do things here
			// There is a tile where the player is trying to go!
		}
		else
		{
			// FIXME: Check for a boat
			x = 0;
			y = 0;
		}

		localPlayer->velocity = { x,y };
		localPlayer->Update(fElapsedTime);
	}

	/// <summary>
	/// Loops the player around the world.
	/// </summary>
	void LoopPlayer()
	{
		float xLoop = localPlayer->GetPosition().x;
		float yLoop = localPlayer->GetPosition().y;
		if (yLoop > MAP_HEIGHT * SPRITE_SCALE)
		{
			yLoop = 0;
		}

		if (xLoop > MAP_WIDTH * SPRITE_SCALE)
		{
			xLoop = 0;
		}

		if (yLoop < 0)
		{
			yLoop = MAP_HEIGHT * SPRITE_SCALE;
		}

		if (xLoop < 0)
		{
			xLoop = MAP_WIDTH * SPRITE_SCALE;
		}

		if (yLoop != localPlayer->GetPosition().y || xLoop != localPlayer->GetPosition().x)
		{
			olc::vf2d targetCameraPosition = localPlayer->GetPosition() - ANIMATION::spriteScale * 5;
			olc::vf2d cameraOffset = targetCameraPosition - renderer->cameraPosition;

			localPlayer->SetPosition({ xLoop, yLoop });

			// This is gonna look like shit
			renderer->SnapCamera(localPlayer->GetPosition() - cameraOffset);
		}
	}

	void UpdateMiniMap()
	{
		// Called once per frame, draws random coloured pixels
		for (int x = 0; x < MAP_WIDTH; x++)
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				int dat = renderer->worldData->GetTileID(x, y);

				if (y * SPRITE_SCALE > localPlayer->GetPosition().y - 25 &&
					y * SPRITE_SCALE < localPlayer->GetPosition().y + 25 &&
					x * SPRITE_SCALE > localPlayer->GetPosition().x - 25 &&
					x * SPRITE_SCALE < localPlayer->GetPosition().x + 25)
				{
					miniMapSprite->SetPixel(x, y, olc::Pixel(255, 0, 0, 255));
				}
				else
				{
					miniMapSprite->SetPixel(x, y, mapColors[dat]);
				}
			}

		miniMapDecal->Update();
	}

	void DestroyTree()
	{
	}

public:
	bool OnLoad() override
	{
		worldData = new WorldData();

		// Initialize the renderer
		renderer = new Renderer(engine, worldData);

		// Player shit
		localPlayer = new Object(renderer->playerSpriteData);
		localPlayer->SetPosition(renderer->worldData->GetRandomGroundTile());

		renderer->SnapCamera(localPlayer->GetPosition());

		miniMapSprite = new olc::Sprite(MAP_WIDTH, MAP_HEIGHT);
		miniMapDecal = new olc::Decal(miniMapSprite);

		mapColors = new olc::Pixel[WORLD_TILES_WIDTH * WORLD_TILES_HEIGHT];

		for (uint8_t i = 0; i < WORLD_TILES_WIDTH * WORLD_TILES_HEIGHT; i++)
		{
			int x = i % WORLD_TILES_WIDTH;
			int y = i / WORLD_TILES_WIDTH;

			x *= SPRITE_SCALE;
			y *= SPRITE_SCALE;

			mapColors[i] = renderer->tileSpriteData->Sprite()->GetPixel(x, y);
		}

		//if (Connect("127.0.0.1", 60000))
		//{
		//	return true;
		//}

		return !DEBUG;
	}

	bool Update() override
	{
		// Movement code
		MovePlayer(time->elapsedTime);
		LoopPlayer();

		renderer->UpdateCameraPosition(localPlayer->GetPosition(), time->elapsedTime);

		// Draw routine
		renderer->DrawWorld();
		renderer->DrawObject(localPlayer);

		for (auto& item : items)
		{
			renderer->DrawItem(item);
		}

		renderer->UpdateSun(time->elapsedTime);
		renderer->UpdateLights();

		if (GetKey(olc::SPACE).bPressed)
		{
			// Set the action direction
			olc::vf2d hitPosition = localPlayer->GetPosition();

			switch (localPlayer->GetLookDir())
			{
			case ANIMATION::right:
				hitPosition.x += SPRITE_SCALE;
				break;
			case ANIMATION::left:
				hitPosition.x -= SPRITE_SCALE;
				break;
			case ANIMATION::down:
				hitPosition.y += SPRITE_SCALE;
				break;
			case ANIMATION::up:
				hitPosition.y -= SPRITE_SCALE;
				break;
			default:
				break;
			}

			int foliageHit[5] =
			{
				worldData->GetFoliageIndex(hitPosition.x, hitPosition.y),
				worldData->GetFoliageIndex(hitPosition.x - SPRITE_SCALE, hitPosition.y),
				worldData->GetFoliageIndex(hitPosition.x + SPRITE_SCALE, hitPosition.y),
				worldData->GetFoliageIndex(hitPosition.x, hitPosition.y - SPRITE_SCALE),
				worldData->GetFoliageIndex(hitPosition.x, hitPosition.y + SPRITE_SCALE)
			};

			for (size_t i = 0; i < 5; i++)
			{
				int worldFoliageIndex = foliageHit[i];
				int foliageID = worldData->foliageData[worldFoliageIndex];

				// Do the action
				if (foliageID == 2) // Hit tree
				{
					worldData->foliageData[worldFoliageIndex] = 0;

					Item log = Item();
					log.ID = 0;
					log.position = hitPosition;
					items.push_back(log);

					break;
				}
			}

			//if (DEBUG)
			//{
			//	print("Player tried to hit: " + std::to_string(foliageID));
			//}
		}

		if (GetKey(olc::Key::M).bReleased)
		{
			drawMiniMap = !drawMiniMap;

			if (drawMiniMap == true)
			{
				UpdateMiniMap();
			}
		}

		if (GetKey(olc::Key::F3).bReleased)
		{
			DEBUG = !DEBUG;
		}

		// Draw ye bloody minimap
		if (drawMiniMap == true)
		{
			// L is for location
			if (GetKey(olc::Key::L).bReleased)
			{
				miniMapDrawLocation++;
				miniMapDrawLocation %= 4;
			}

			// I is for in
			if (GetKey(olc::Key::I).bHeld)
			{
				miniMapDrawScale += 0.01f;
				if (miniMapDrawScale > 1) miniMapDrawScale = 1;
			}

			// O is for out
			if (GetKey(olc::Key::O).bHeld)
			{
				miniMapDrawScale -= 0.01f;
				if (miniMapDrawScale < 0.05f) miniMapDrawScale = 0.05f;
			}

			// Only update the minimap every 'n'th frame
			if (time->frameCount % 20 == 0)
				UpdateMiniMap();

			// Draw the minimap

			olc::vf2d drawLocation;
			switch (miniMapDrawLocation)
			{
			case 0:
				drawLocation = { 0,0 };
				break;
			case 1:
				drawLocation = { ScreenWidth() - miniMapSprite->width * miniMapDrawScale, 0 };
				break;
			case 2:
				drawLocation = { ScreenWidth() - miniMapSprite->width * miniMapDrawScale, ScreenHeight() - miniMapSprite->height * miniMapDrawScale };
				break;
			case 3:
				drawLocation = { 0, ScreenHeight() - miniMapSprite->height * miniMapDrawScale };
				break;
			default:
				break;
			}

			engine->DrawDecal(drawLocation, miniMapDecal, { miniMapDrawScale, miniMapDrawScale });
		}

		if (DEBUG)
		{
			engine->DrawStringDecal({ 0,0 }, "pos:" + std::to_string(localPlayer->GetPosition().x) + ", " + std::to_string(localPlayer->GetPosition().y) + "\n" +
				"tileID:" + std::to_string(tileId) + "\n",
				olc::YELLOW, { 0.5f, 0.5f });
		}

		if (GetKey(olc::ESCAPE).bReleased) {
			//FIXME: Toggle pause
			APPLICATION_RUNNING = !APPLICATION_RUNNING; // remove this
		}

		return APPLICATION_RUNNING;
	}
};

class MultiPlayerClient : public Scene
{
	GameServer* server = nullptr;
	Client* client = nullptr;

private:
	olc::TileTransformedView tv;

	std::string sWorldMap =
		"################################"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..........####...####.........#"
		"#..........#.........#.........#"
		"#..........#.........#.........#"
		"#..........#.........#.........#"
		"#..........##############......#"
		"#..............................#"
		"#..................#.#.#.#.....#"
		"#..............................#"
		"#..................#.#.#.#.....#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"#..............................#"
		"################################";

	olc::vi2d vWorldSize = { 32, 32 };

private:
	std::unordered_map<uint32_t, PlayerDescription> mapObjects;
	uint32_t playerID = 0;
	PlayerDescription descPlayer;

	bool waitingForConnection = true;

	bool TryConnect(const char* ip = "127.0.0.1", uint16_t port = 60000)
	{
		if (client != nullptr) return false;

		client = new Client();

		return client->Connect(ip, port);
	}

public:
	bool OnLoad() override
	{
		if (IS_SERVER)
		{
			server = new GameServer(60000);
			server->Start();
		}

		tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 8, 8 });

		//mapObjects[0].uniqueID = 0;
		//mapObjects[0].vPos = { 3.0f, 3.0f };

		return TryConnect();
	}

	bool Update() override
	{
		// Check for incoming network messages
		if (client->IsConnected())
		{
			while (!client->Incoming().empty())
			{
				auto msg = client->Incoming().pop_front().msg;

				switch (msg.header.id)
				{
				case(GameMsg::Client_Accepted):
				{
					std::cout << "Server accepted client - you're in!\n";
					olc::net::message<GameMsg> msg;
					msg.header.id = GameMsg::Client_RegisterWithServer;
					descPlayer.position = { 3.0f, 3.0f };
					msg << descPlayer;
					client->Send(msg);
					break;
				}

				case(GameMsg::Client_AssignID):
				{
					// Server is assigning us OUR id
					msg >> playerID;
					std::cout << "Assigned Client ID = " << playerID << "\n";
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

		if (IS_SERVER == false && waitingForConnection)
		{
			engine->Clear(olc::DARK_BLUE);
			engine->DrawString({ 10,10 }, "Waiting to connect...", olc::WHITE);
			return true;
		}

		// Control of Player Object
		mapObjects[playerID].velocity = { 0.0f, 0.0f };
		if (GetKey(olc::Key::W).bHeld) mapObjects[playerID].velocity += { 0.0f, -1.0f };
		if (GetKey(olc::Key::S).bHeld) mapObjects[playerID].velocity += { 0.0f, +1.0f };
		if (GetKey(olc::Key::A).bHeld) mapObjects[playerID].velocity += { -1.0f, 0.0f };
		if (GetKey(olc::Key::D).bHeld) mapObjects[playerID].velocity += { +1.0f, 0.0f };

		if (mapObjects[playerID].velocity.mag2() > 0)
			mapObjects[playerID].velocity = mapObjects[playerID].velocity.norm() * 4.0f;

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
					if (sWorldMap[vCell.y * vWorldSize.x + vCell.x] == '#')
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

		// Handle Pan & Zoom
		if (engine->GetMouse(2).bPressed) tv.StartPan(engine->GetMousePos());
		if (engine->GetMouse(2).bHeld) tv.UpdatePan(engine->GetMousePos());
		if (engine->GetMouse(2).bReleased) tv.EndPan(engine->GetMousePos());
		if (engine->GetMouseWheel() > 0) tv.ZoomAtScreenPos(1.5f, engine->GetMousePos());
		if (engine->GetMouseWheel() < 0) tv.ZoomAtScreenPos(0.75f, engine->GetMousePos());

		// Clear World
		engine->Clear(olc::BLACK);

		// Draw World
		olc::vi2d vTL = tv.GetTopLeftTile().max({ 0,0 });
		olc::vi2d vBR = tv.GetBottomRightTile().min(vWorldSize);
		olc::vi2d vTile;
		for (vTile.y = vTL.y; vTile.y < vBR.y; vTile.y++)
			for (vTile.x = vTL.x; vTile.x < vBR.x; vTile.x++)
			{
				if (sWorldMap[vTile.y * vWorldSize.x + vTile.x] == '#')
				{
					tv.DrawRect(vTile, { 1.0f, 1.0f });
					tv.DrawRect(olc::vf2d(vTile) + olc::vf2d(0.1f, 0.1f), { 0.8f, 0.8f });
				}
			}

		// Draw World Objects
		for (auto& object : mapObjects)
		{
			// Draw Boundary
			tv.DrawCircle(object.second.position, object.second.radius);

			// Draw Velocity
			if (object.second.velocity.mag2() > 0)
				tv.DrawLine(object.second.position, object.second.position + object.second.velocity.norm() * object.second.radius, olc::MAGENTA);

			// Draw Name
			olc::vi2d vNameSize = engine->GetTextSizeProp("ID: " + std::to_string(object.first));
			tv.DrawStringPropDecal(object.second.position - olc::vf2d{ vNameSize.x * 0.5f * 0.25f * 0.125f, -object.second.radius * 1.25f }, "ID: " + std::to_string(object.first), olc::BLUE, { 0.25f, 0.25f });
		}

		// Send player description
		olc::net::message<GameMsg> msg;
		msg.header.id = GameMsg::Game_UpdatePlayer;
		msg << mapObjects[playerID];
		client->Send(msg);

		if (IS_SERVER)
		{
			server->Update(-1, true);

			if (DEBUG) ShowWindow(GetConsoleWindow(), SW_SHOW);
		}

		return true;
	}
};