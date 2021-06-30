#include "../Headers/gameScene.h"

olc::vi2d GetToolSprite(Tools tool)
{
	return olc::vi2d{ int(tool) - 1 , 0 };
}

// NOTE: later we will need this to be a generic ITEM
Tools GameScene::GetToolItemFromHotbar(const uint32_t i)
{
	if (i >= hotbarSlotCount)
		return Tools::None;

	wchar_t c = hotbarInventory[i];

	return (Tools)c;
}

void GameScene::RemoveToolItemFromHotbar(const uint32_t i)
{
	if (i < hotbarSlotCount)
	{
		hotbarInventory[i] = 0;
	}
}

// NOTE: later we will need this to be a generic ITEM
Tools GameScene::GetToolItemFromInventory(const uint32_t i)
{
	if (i >= playerInventory.length()) return Tools::None;

	wchar_t c = playerInventory.at(i);

	return (Tools)c;
}

// NOTE: later we will need this to be a generic ITEM
bool GameScene::AddToolItemToInventory(const Tools item)
{
	if (playerInventory.length() >= inventorySlotCount)
	{
		print("Inventory full, could not add item.");
		DisplayNotification("Inventory full.");

		return false;
	}

	wchar_t itemChar = static_cast<wchar_t>(item);

	playerInventory.push_back(itemChar);

	std::string itemName = ParseToolToString(item);

	print("Added \'" + itemName + "\' to inventory.");
	DisplayNotification("Picked up \'" + itemName + "\'");

	return true;
}

// NOTE: later we will need this to be a generic ITEM
bool GameScene::AddToolItemToHotbar(const Tools item)
{
	for (uint8_t i = 0; i < hotbarSlotCount; i++)
	{
		bool addedItem = AddToolItemToHotbarSlot(item, i, false);

		if (addedItem) return true;
	}

	print("Hotbar full, could not add item.");
	DisplayNotification("Hotbar full.");

	return false;
}

bool GameScene::AddToolItemToHotbarSlot(const Tools item, int slot, bool overrideItem)
{
	wchar_t itemChar = static_cast<wchar_t>(item);
	std::string itemName = ParseToolToString(item);

	if (hotbarInventory[slot] == 0 || overrideItem == true)
	{
		for (uint8_t i = 0; i < hotbarSlotCount; i++)
		{
			if (hotbarInventory[i] == itemChar)
			{
				// item already exists in hotbar
				hotbarInventory[i] = 0;
			}
		}

		hotbarInventory[slot] = itemChar;

		print("Added \'" + itemName + "\' to hotbar.");
		//DisplayNotification("Picked up \'" + itemName + "\'");

		return true;
	}

	return false;
}

void GameScene::ModifyHotbarSlots(int mod)
{
	hotbarSlotCount += mod;

	if (hotbarSlotCount > MAX_HOTBAR_SLOTS)
	{
		hotbarSlotCount = MAX_HOTBAR_SLOTS;
	}

	if (hotbarSlotCount < MIN_HOTBAR_SLOTS)
	{
		hotbarSlotCount = MIN_HOTBAR_SLOTS;
	}

	if (currentHotbarSlot != -1)
	{
		if (currentHotbarSlot >= hotbarSlotCount)
		{
			currentHotbarSlot = -1;
		}
	}
}

void GameScene::ModifyInventorySlots(int mod)
{
	inventorySlotCount += mod;

	if (inventorySlotCount > MAX_INVENTORY_SLOTS)
	{
		inventorySlotCount = MAX_INVENTORY_SLOTS;
	}

	if (inventorySlotCount < MIN_INVENTORY_SLOTS)
	{
		inventorySlotCount = MIN_INVENTORY_SLOTS;
	}
}

void GameScene::ChangeCurrentHotbarSlot(int index)
{
	if (currentHotbarSlot != index)
	{
		// Equip the item in the slot from index
		currentHotbarSlot = index;
	}
	else
	{
		// If feeding in the current slot then just unequip the current item
		currentHotbarSlot = -1;
		localPlayer->currentTool = Tools::None;
	}
}

void GameScene::DrawInventory()
{
	Tools& currentTool = localPlayer->currentTool;
	olc::vi2d slotHudScale = olc::vi2d{ SPRITE_SCALE * 2,  SPRITE_SCALE * 2 };
	const olc::vi2d SLOT_HUD_SOURCE_SCALE = olc::vi2d{ SPRITE_SCALE * 2, SPRITE_SCALE * 2 };
	olc::vf2d pos{};
	olc::vi2d itemSlotSpritePos{};
	olc::vi2d mousePos = GetMousePos();

	for (uint8_t i = 0; i < inventorySlotCount; i++)
	{
		Tools currentSlotTool = GetToolItemFromInventory(i);

		int slotPointX = SPRITE_SCALE * 2 * (i % MAX_HOTBAR_SLOTS);
		int slotPointY = SPRITE_SCALE * 2 * (i / MAX_HOTBAR_SLOTS);

		// Draw the inventory
		pos = olc::vf2d{ static_cast<float>(slotHudScale.x + slotPointX),
						static_cast<float>(slotHudScale.y + slotPointY) };

		// Input stuff
		bool mouseOver = (mousePos.x > pos.x && mousePos.x < pos.x + slotHudScale.x &&
			mousePos.y > pos.y && mousePos.y < pos.y + slotHudScale.y);

		itemSlotSpritePos = { mouseOver ? 1 : 0 , 0 };

		engine->DrawPartialDecal(pos, slotHudScale, renderer->itemSlotRenderable->Decal(),
			itemSlotSpritePos * (SPRITE_SCALE * 2), SLOT_HUD_SOURCE_SCALE);

		if (currentSlotTool != Tools::None)
		{
			// Draw the tool itself
			engine->DrawPartialDecal(pos, slotHudScale, renderer->toolsRenderable->Decal(),
				GetToolSprite(currentSlotTool) * (SPRITE_SCALE * 2), SLOT_HUD_SOURCE_SCALE);
		}

		// Input stuff
		if (mouseOver == true)
		{
			// Place this item in a hotbar slot once the respective key is pressed
			for (uint8_t k = 0; k < hotbarSlotCount; k++)
			{
				if (GetKey(olc::Key(28 + k)).bPressed)
				{
					AddToolItemToHotbarSlot(currentSlotTool, k, true);
				}
			}

			// Place this item in the first available hotbar slot
			if (GetMouse(0).bPressed)
			{
				// FIXME: pickup this item and hold it until the user releases their cursor then drop it in the slot released over
				if (currentSlotTool != Tools::None)
				{
					AddToolItemToHotbar(currentSlotTool);
				}
			}

			if (currentSlotTool != Tools::None)
			{
				toolTipText = ParseToolToString(currentSlotTool);
				toolTipPos = pos;
			}
		}
	}
}

void GameScene::DrawToolbarArea()
{
	Tools& currentTool = localPlayer->currentTool;
	olc::vi2d slotHudScale = olc::vi2d{ SPRITE_SCALE * 2,  SPRITE_SCALE * 2 };
	const olc::vi2d SLOT_HUD_SOURCE_SCALE = olc::vi2d{ SPRITE_SCALE * 2, SPRITE_SCALE * 2 };
	olc::vf2d pos{};
	olc::vi2d itemSlotPos{};

	float halfTotalSlotCount = static_cast<float>(hotbarSlotCount) * 0.5f;
	halfTotalSlotCount *= SPRITE_SCALE * 2.0f;

	for (uint8_t i = 0; i < hotbarSlotCount; i++)
	{
		Tools currentSlotTool = GetToolItemFromHotbar(i);

		int slotPoint = SPRITE_SCALE * 2 * i;

		// Draw the hotbar from left to right in the middle of the screen
		pos = olc::vf2d{ ScreenWidth() / 2.0f - halfTotalSlotCount + slotPoint,
			ScreenHeight() - SPRITE_SCALE * 2.0f };
		itemSlotPos = { i == currentHotbarSlot ? 1 : 0 , 0 };

		if (i == currentHotbarSlot)
		{
			currentTool = currentSlotTool;
			pos.y -= SPRITE_SCALE * 0.15f;
		}

		engine->DrawPartialDecal(pos, slotHudScale, renderer->itemSlotRenderable->Decal(),
			itemSlotPos * (SPRITE_SCALE * 2), SLOT_HUD_SOURCE_SCALE);

		if (currentSlotTool != Tools::None)
		{
			// Draw the tool itself
			engine->DrawPartialDecal(pos, slotHudScale, renderer->toolsRenderable->Decal(),
				GetToolSprite(currentSlotTool) * (SPRITE_SCALE * 2), SLOT_HUD_SOURCE_SCALE);
		}

		// Input stuff

		if (GetKey(olc::Key(28 + i)).bPressed)
		{
			ChangeCurrentHotbarSlot(i);
		}

		olc::vi2d mousePos = GetMousePos();

		if (mousePos.x > pos.x && mousePos.x < pos.x + slotHudScale.x &&
			mousePos.y > pos.y && mousePos.y < pos.y + slotHudScale.y)
		{
			if (currentSlotTool != Tools::None)
			{
				toolTipText = ParseToolToString(currentSlotTool);
				toolTipPos = pos;
			}

			// NOTE: Maybe send a "on tool changed event" here
			if (GetMouse(0).bPressed)
			{
				ChangeCurrentHotbarSlot(i);
			}

			if (GetMouse(1).bPressed)
			{
				RemoveToolItemFromHotbar(i);
			}
		}
	}
}