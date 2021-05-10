// Forrest Lowe 2021
#pragma once

#define OLC_PGEX_NETWORK
#include "olcPGEX_Network.h"
#include "networkCommon.h"

const int8_t SPRITE_SCALE = 12;
olc::vi2d spriteScale = { (int)SPRITE_SCALE, (int)SPRITE_SCALE };

const uint8_t WORLD_TILES_WIDTH = 4;
const uint8_t WORLD_TILES_HEIGHT = 3;

const uint8_t WORLD_ITEMS_WIDTH = 3;
const uint8_t WORLD_ITEMS_HEIGHT = 1;

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

#include "worldData.h"
#include "miniMap.h"

#include "objectDefinitions.h"
#include "renderer.h"

#include <unordered_map>

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

enum class PLAY_MODE : uint8_t
{
	SINGLE_PLAYER,
	CLIENT,
	SERVER
};

PLAY_MODE playMode = PLAY_MODE::SINGLE_PLAYER;

enum class SCENE : uint8_t
{
	SCENE_INTRO,
	SCENE_MAIN_MENU,
	SCENE_MP_LOBBY,
	SCENE_SINGLE_PLAYER,
	SCENE_MULTI_PLAYER
};

SCENE currentScene = SCENE::SCENE_INTRO;

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
	~IntroScene()
	{
		delete olc_logo;
		delete flowe_logo;
	}

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
	~MainMenu()
	{
		delete title;
		delete singlePlayerButton;
		delete multiPlayerButton;
		delete quitButton;
	}

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
		// Reseed the world generator
		srand(static_cast<unsigned int>(std::time(0)));

		// Draw the menu
		engine->Clear(olc::DARK_BLUE);
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
				playMode = PLAY_MODE::SINGLE_PLAYER;
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
	~MultiplayerLobby()
	{
		delete hostButton;
		delete joinButton;
		delete backButton;
	}

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

		engine->Clear(olc::DARK_BLUE);
		hostButton->Draw();
		joinButton->Draw();
		backButton->Draw();

		// Host button
		if (hostButton->MouseOver())
		{
			hostButton->fillColor = highlightColor;

			if (engine->GetMouse(0).bReleased)
			{
				currentScene = SCENE::SCENE_MULTI_PLAYER;
				playMode = PLAY_MODE::SERVER;
			}
		}
		else hostButton->fillColor = defaultColor;

		// Join button
		if (joinButton->MouseOver())
		{
			joinButton->fillColor = highlightColor;

			if (engine->GetMouse(0).bReleased)
			{
				currentScene = SCENE::SCENE_MULTI_PLAYER;
				playMode = PLAY_MODE::CLIENT;
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

	MiniMap* minimap;

	std::vector<Item> items;

public:
	~SinglePlayer()
	{
		delete renderer;
		delete worldData;
		delete localPlayer;
	}

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
#ifdef NEW_RENDERER
			localPlayer->SetPosition({ xLoop, yLoop });
#endif // OLD_RENDERER

#ifdef OLD_RENDERER

			olc::vf2d targetCameraPosition = localPlayer->GetPosition() - ANIMATION::spriteScale * 5;
			olc::vf2d cameraOffset = targetCameraPosition - renderer->cameraPosition;

			localPlayer->SetPosition({ xLoop, yLoop });

			// This is gonna look like shit
			renderer->SnapCamera(localPlayer->GetPosition() - cameraOffset);
#endif // OLD_RENDERER
		}
	}

	void TryHit()
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
	}

public:
	bool OnLoad() override
	{
		worldData = new WorldData();

		// Initialize the renderer
		renderer = new Renderer(engine, worldData);

		minimap = new MiniMap();
		minimap->Initialize(renderer->tileSpriteData->Sprite(), worldData, engine, time);

#ifdef OLD_RENDERER
		renderer->SnapCamera(localPlayer->GetPosition());
#endif // OLD_RENDERER

		return !DEBUG;
	}

	bool Update() override
	{
		if (GetKey(olc::ESCAPE).bReleased)
		{
			//FIXME: Toggle pause

			currentScene = SCENE::SCENE_MAIN_MENU;
		}

		engine->Clear(olc::BLACK);

		if (worldData->GetWorldGenerated() == false)
		{
			if (worldData->GetWorldProgress() < 0.99f)
			{
				engine->Clear(olc::DARK_BLUE);
				engine->DrawStringDecal({ 10,10 }, "Loading...", olc::WHITE);
				return true;
			}
			else
			{
				worldData->GenerateMap();

				// Player shit
				localPlayer = new Object(renderer->playerSpriteData);
				localPlayer->SetPosition(renderer->worldData->GetRandomGroundTile());

				return true;
			}
		}

		// Movement code
		MovePlayer(time->elapsedTime);
		LoopPlayer();

		renderer->SetCamera(localPlayer->GetPosition() / SPRITE_SCALE);
		renderer->DrawWorld();

#ifdef OLD_RENDERER
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
#endif

		if (GetKey(olc::SPACE).bPressed)
		{
			TryHit();
		}

		minimap->UpdateMiniMap(localPlayer->GetPosition());

		if (DEBUG)
		{
			engine->DrawStringDecal({ 0,0 }, "pos:" + std::to_string(localPlayer->GetPosition().x) + ", " + std::to_string(localPlayer->GetPosition().y) + "\n" +
				"tileID:" + std::to_string(tileId) + "\n",
				olc::YELLOW, { 0.5f, 0.5f });
		}

		return APPLICATION_RUNNING;
	}
};

class MultiPlayer : public Scene
{
	GameServer* server = nullptr;
	Client* client = nullptr;

	WorldData* worldData;

	Renderer* renderer = nullptr;
	MiniMap* minimap = nullptr;

private:
	olc::vi2d vWorldSize = { MAP_WIDTH, MAP_HEIGHT };

private:
	void MovePlayer()
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
		olc::vf2d playerPos = mapObjects[playerID].position;
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

		mapObjects[playerID].velocity = velocity;
		//localPlayer->Update(fElapsedTime);
	}

private: // Networking stuff
	std::unordered_map<uint32_t, PlayerDescription> mapObjects;
	uint32_t playerID = 0;
	PlayerDescription descPlayer;

	bool waitingForConnection = true;

	bool TryConnect(const char* ip = "127.0.0.1", uint16_t port = SERVER_PORT)
	{
		if (client != nullptr) return false;
		client = new Client();

		return client->Connect(ip, port);
	}

	void HandleNetworkingMessages()
	{
		if (client->IsConnected())
		{
			while (client->Incoming().empty())
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

	void HandleClient()
	{
		olc::net::message<GameMsg> msg;
		msg.header.id = GameMsg::Game_UpdatePlayer;
		msg << mapObjects[playerID];
		client->Send(msg);
	}

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

		minimap = new MiniMap();
		minimap->Initialize(renderer->tileSpriteData->Sprite(), worldData, engine, time);

		if (playMode == PLAY_MODE::CLIENT)
			TryConnect(); // FIXME: wait for user input to connect to

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

		if (worldData->GetWorldGenerated() == false)
		{
			if (worldData->GetWorldProgress() < 0.99f)
			{
				engine->Clear(olc::DARK_BLUE);
				engine->DrawStringDecal({ 10,10 }, "Loading...", olc::WHITE);
				return true;
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
						return true;
					}

					mapObjects.insert_or_assign(0, descPlayer);
					waitingForConnection = false;
				}

				return true;
			}
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
		renderer->SetCamera(mapObjects[playerID].position / SPRITE_SCALE);

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
			renderer->DrawPlayer(object.second.position);

			// Draw Boundary
			renderer->viewPort.DrawCircle(object.second.position, object.second.radius);

			// Draw Velocity
			if (object.second.velocity.mag2() > 0)
				renderer->viewPort.DrawLine(object.second.position, object.second.position + object.second.velocity.norm() * object.second.radius, olc::MAGENTA);

			// Draw Name
			olc::vi2d vNameSize = engine->GetTextSizeProp("ID: " + std::to_string(object.first));
			renderer->viewPort.DrawStringPropDecal(object.second.position - olc::vf2d{ vNameSize.x * 0.5f * 0.25f * 0.125f, -object.second.radius * 1.25f }, "ID: " + std::to_string(object.first), olc::BLUE, { 0.25f, 0.25f });
		}

		minimap->UpdateMiniMap(mapObjects[playerID].position);

		if (playMode != PLAY_MODE::SINGLE_PLAYER)
		{
			// Send player description
			HandleClient();

			if (playMode == PLAY_MODE::SERVER) server->Update(-1, true);
		}

		return true;
	}
};