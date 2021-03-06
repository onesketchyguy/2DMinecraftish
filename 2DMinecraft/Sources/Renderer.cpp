#define OLC_PGEX_TRANSFORMEDVIEW
#include "../Headers/renderer.h"

Renderer::Renderer(olc::PixelGameEngine* engine)
{
	this->engine = engine;

	viewPort = olc::TileTransformedView({ engine->ScreenWidth(), engine->ScreenHeight() },
		{ SPRITE_SCALE, SPRITE_SCALE });

	print("Created viewport.");

	const std::string DATA_LOCATION = "packfile.dat";
	const std::string RESOURCE_KEY = "resourceKEY";

	resourcePack = new olc::ResourcePack();
	bool loaded = resourcePack->LoadPack(DATA_LOCATION, RESOURCE_KEY);

	if (loaded == false)
	{
		print("Generating resource pack...");

		resourcePack->AddFile("Data/player.png");
		resourcePack->AddFile("Data/items.png");
		resourcePack->AddFile("Data/tiles.png");
		resourcePack->AddFile("Data/tools.png");
		resourcePack->AddFile("Data/worldTools.png");
		resourcePack->AddFile("Data/item_slot.png");
		resourcePack->AddFile("Data/items.png");
		resourcePack->AddFile("Data/olc_logo.png");
		resourcePack->AddFile("Data/flowe_logo.png");
		resourcePack->AddFile("Data/colorPalette.png");
		resourcePack->AddFile("Data/selectionCursor.png");

		resourcePack->SavePack(DATA_LOCATION, RESOURCE_KEY);
		resourcePack->LoadPack(DATA_LOCATION, RESOURCE_KEY);
	}

	print("Loading all sprites...");

	// Player shit
	playerSpriteData = new olc::Renderable();
	LoadSprites(playerSpriteData, "Data/player.png");

	// Item shit
	itemSpriteData = new olc::Renderable();
	LoadSprites(itemSpriteData, "Data/items.png");
	worldToolsRenderable = new olc::Renderable();
	LoadSprites(worldToolsRenderable, "Data/worldTools.png");

	// UI shit
	toolsRenderable = new olc::Renderable();
	LoadSprites(toolsRenderable, "Data/tools.png");
	itemSlotRenderable = new olc::Renderable();
	LoadSprites(itemSlotRenderable, "Data/item_slot.png");
	selectionCursor = new olc::Renderable();
	LoadSprites(selectionCursor, "Data/selectionCursor.png");

	// World shit
	tileSpriteData = new olc::Renderable();
	LoadSprites(tileSpriteData, "Data/tiles.png");

	// Debug shit
	squareSprite = new olc::Sprite(1, 1);
	squareSprite->SetPixel(0, 0, olc::WHITE);

	squareDecal = new olc::Decal(squareSprite);
}

void Renderer::LoadSprites(olc::Renderable* renderable, std::string dir)
{
	renderable->Load(dir, resourcePack);
	print("Loading sprites from: " + dir);

	if (renderable->Decal() == nullptr)
	{
		print("Could not load sprites!");
		DEBUG = true;
		APPLICATION_RUNNING = true;
	}
	else
	{
		print("Loaded " + dir);
	}
}

void Renderer::DrawDecal(olc::vf2d pos, olc::vf2d scale,
	olc::Decal* decal, olc::Pixel color)
{
	viewPort.DrawDecal(pos, decal, scale, color);
}

void Renderer::DrawPartialDecal(olc::vf2d pos, olc::vf2d scale, olc::Decal* decal,
	olc::vi2d cell, olc::Pixel color)
{
	viewPort.DrawPartialDecal(pos, scale, decal, cell, spriteScale, color);
}

void Renderer::DrawQueue()
{
	for (DecalData& data : drawQueue)
	{
		viewPort.DrawPartialDecal(data.pos, data.scale, data.decal, data.cell,
			spriteScale, data.color);
	}

	drawQueue.clear();
}

void Renderer::EnqueueDrawPartialDecal(olc::vf2d pos, olc::vf2d scale,
	olc::Decal* decal, olc::vi2d cell, olc::Pixel color)
{
	DecalData data(pos, scale, decal, cell, color);

	drawQueue.push_back(data);
}

void Renderer::DrawPartialDecalInScreenSpace(olc::vf2d pos, olc::vf2d scale,
	olc::Decal* decal, olc::vi2d cell, olc::vf2d sprScale, olc::Pixel color)
{
	olc::vf2d worldPos = viewPort.ScreenToWorld(pos);
	viewPort.DrawPartialDecal(worldPos, scale, decal, cell, sprScale, color);
}

void Renderer::DrawPlayer(PlayerDescription& player)
{
	engine->SetPixelMode(olc::Pixel::NORMAL);

	olc::vf2d sprScale{ static_cast<float>(spriteScale.x), static_cast<float>(spriteScale.y) };
	olc::vf2d pos = player.position - olc::vf2d{ 0.5f, 0.5f };

	if (player.flip_x)
	{
		pos.x += 1;
		sprScale.x *= -1;
	}

	DrawPartialDecal(pos, sprScale, playerSpriteData->Decal(),
		{
			static_cast<int>(player.avatarIndex_x * SPRITE_SCALE),
			static_cast<int>(player.avatarIndex_y * SPRITE_SCALE)
		});

	if (DEBUG == true)
	{
		// Draw colision area
		DrawDecal(player.position - olc::vf2d{ 0.5f, 0.5f }, olc::vf2d{ 1.0f, 1.0f } *player.radius * 2.0f * SPRITE_SCALE, squareDecal, olc::Pixel(255, 0, 0, 100));

		// Draw Velocity
		if (player.velocity.mag2() > 0)
		{
			DrawDecal(player.position, olc::vf2d{ 1.0f, 1.0f } + player.velocity * SPRITE_SCALE, squareDecal, olc::Pixel(255, 0, 255, 100));
		}
	}

	if (playMode != PLAY_MODE::SINGLE_PLAYER || DEBUG == true)
	{
		// Draw Name
		std::string userName = "ID: " + std::to_string(player.uniqueID);

		olc::vi2d vNameSize = engine->GetTextSizeProp(userName);
		viewPort.DrawStringPropDecal(player.position - olc::vf2d{ vNameSize.x * 0.5f * 0.25f * 0.125f, -player.radius * 2.0f * SPRITE_SCALE * 1.25f },
			userName, olc::BLUE, { 0.25f, 0.25f });
	}
}

void Renderer::DrawTile(int mapIndex, float x, float y)
{
	engine->SetPixelMode(olc::Pixel::NORMAL);

	olc::vf2d pos = { x, y };

	int foliage = worldData->foliageData[mapIndex];
	int tile = worldData->tileData[mapIndex];

	int cellIndex_x = tile % WORLD_TILES_WIDTH;
	int cellIndex_y = tile / WORLD_TILES_WIDTH;
	olc::vi2d tileSpriteCell = {
		spriteScale.x * cellIndex_x,
		spriteScale.y * cellIndex_y };

	// Draw tile
	DrawPartialDecal(pos, spriteScale + olc::vf2d{ 1.5f, 1.5f }, tileSpriteData->Decal(), tileSpriteCell);

	if (foliage > 0) // 0 = no foliage
	{
		foliage -= 1; // Subtract 1 to put this layer back into sprite space

		olc::vi2d foliageSpriteCell = { spriteScale.x * foliage, spriteScale.x * 2 };

		// Draw foliage
		DrawPartialDecal(pos, spriteScale, tileSpriteData->Decal(), tileSpriteCell);
	}
}

void Renderer::EnqueueDrawTile(int mapIndex, float x, float y)
{
	engine->SetPixelMode(olc::Pixel::NORMAL);

	olc::vf2d pos = { x, y };

	int foliage = worldData->foliageData[mapIndex];
	int tile = worldData->tileData[mapIndex];

	int cellIndex_x = tile % WORLD_TILES_WIDTH;
	int cellIndex_y = tile / WORLD_TILES_WIDTH;
	olc::vi2d tileSpriteCell = {
		spriteScale.x * cellIndex_x,
		spriteScale.y * cellIndex_y 
	};

	// Draw tile
	EnqueueDrawPartialDecal(pos, spriteScale + olc::vf2d{ 1.5f, 1.5f },
		tileSpriteData->Decal(), tileSpriteCell);

	if (foliage > 0) // 0 = no foliage
	{
		foliage -= 1; // Subtract 1 to put this layer back into sprite space

		olc::vi2d foliageSpriteCell = { spriteScale.x * foliage, spriteScale.y * 2 };

		// Draw foliage
		EnqueueDrawPartialDecal(pos, spriteScale, tileSpriteData->Decal(), foliageSpriteCell);
	}
}

/*
void Renderer::DrawItem(Item obj) {
	engine->SetPixelMode(olc::Pixel::NORMAL);

	olc::vi2d spriteCell{ obj.ID % WORLD_ITEMS_WIDTH, obj.ID / WORLD_ITEMS_WIDTH };

	DrawPartialDecal(obj.position, { SPRITE_SCALE * 0.75f,SPRITE_SCALE * 0.75f },
		itemSpriteData->Decal(), spriteCell);
}
*/

void Renderer::DrawWorld()
{
	// Clear World
	engine->Clear(olc::BLACK);

	// Draw World
	topLeft = viewPort.GetTopLeftTile().max({ 0,0 });
	bottomRight = viewPort.GetBottomRightTile().min({ worldData->GetMapWidth(), worldData->GetMapHeight() });
	for (int y = topLeft.y; y < bottomRight.y; y++)
	{
		for (int x = topLeft.x; x < bottomRight.x; x++)
		{
			olc::vi2d tile{ x, y };

			int mapIndex = tile.y * worldData->GetMapWidth() + tile.x;

			EnqueueDrawTile(mapIndex, static_cast<float>(tile.x), static_cast<float>(tile.y));
		}
	}

	DrawQueue();
}

void Renderer::SetZoomScaleToMax()
{
	viewPort.SetZoom(MAX_ZOOM, camTarget);
	screenCenter = viewPort.ScaleToWorld(olc::vf2d{ engine->ScreenWidth() / 2.0f, engine->ScreenHeight() / 2.0f });
}

void Renderer::SetZoomScaleToMin()
{
	viewPort.SetZoom(MIN_ZOOM, camTarget);
	screenCenter = viewPort.ScaleToWorld(olc::vf2d{ engine->ScreenWidth() / 2.0f, engine->ScreenHeight() / 2.0f });
}

void Renderer::UpdateZoom()
{
	if (engine->GetMouseWheel() > 0) viewPort.ZoomAtScreenPos(1.5f, engine->GetMousePos());
	if (engine->GetMouseWheel() < 0) viewPort.ZoomAtScreenPos(0.5f, engine->GetMousePos());

	// Clamp the zoom
	if (viewPort.GetWorldScale().x <= MIN_ZOOM)
		viewPort.SetZoom(MIN_ZOOM, camTarget);

	if (viewPort.GetWorldScale().x >= MAX_ZOOM)
		viewPort.SetZoom(MAX_ZOOM, camTarget);

	screenCenter = viewPort.ScaleToWorld(olc::vf2d{ engine->ScreenWidth() / 2.0f, engine->ScreenHeight() / 2.0f });
}

void Renderer::SetCamera(olc::vf2d pos)
{
	//pos += olc::vf2d(SPRITE_SCALE * 0.5f, SPRITE_SCALE * 0.5f);
	//pos /= SPRITE_SCALE;
	pos.x += 0.5f;

	camTarget = pos - screenCenter;
	viewPort.SetWorldOffset(camTarget);
}

#undef OLC_PGEX_TRANSFORMEDVIEW