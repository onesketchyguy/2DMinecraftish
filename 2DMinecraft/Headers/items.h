#pragma once
#include <iostream>
#include <fstream>
#include "json.hpp";

using json = nlohmann::json;

const std::string ITEMS_FILE = "items.json";

struct Item
{
public:
	std::string itemName;

	std::string uisprDir;
	int uisprWidth;
	int uisprHeight;
	int uisprXoff;
	int uisprYoff;

	std::string sprDir;
	int sprWidth;
	int sprHeight;
	int sprXoff;
	int sprYoff;

	std::string itemType;
	int effectiveness;

	int health;

	std::string craftingComponents;

public:
	Item() = default;
	Item(std::string itemName, std::string sprDir, int sprWidth, int sprHeight, int sprXoff, int sprYoff,
		std::string uisprDir, int uisprWidth, int uisprHeight, int uisprXoff, int uisprYoff,
		std::string itemType, int effectiveness, int health, std::string craftingComponents)
	{
		this->itemName = itemName;

		this->uisprDir = uisprDir;
		this->uisprWidth = uisprWidth;
		this->uisprHeight = uisprHeight;
		this->uisprXoff = uisprXoff;
		this->uisprYoff = uisprYoff;

		this->sprDir = sprDir;
		this->sprWidth = sprWidth;
		this->sprHeight = sprHeight;
		this->sprXoff = sprXoff;
		this->sprYoff = sprYoff;

		this->itemType = itemType;
		this->effectiveness = effectiveness;
		this->health = health;

		this->craftingComponents = craftingComponents;
	}
};

void WriteData(const Item& item)
{
	json j;

	j["itemName"] = item.itemName;

	j["uisprDir"] = item.uisprDir;
	j["uisprWidth"] = item.uisprWidth;
	j["uisprHeight"] = item.uisprHeight;
	j["uisprXoff"] = item.uisprXoff;
	j["uisprYoff"] = item.uisprYoff;

	j["sprDir"] = item.sprDir;
	j["sprWidth"] = item.sprWidth;
	j["sprHeight"] = item.sprHeight;
	j["sprXoff"] = item.sprXoff;
	j["sprYoff"] = item.sprYoff;

	j["health"] = item.health;

	j["itemType"] = item.itemType;
	j["effectiveness"] = item.effectiveness;
	j["craftingComponents"] = item.craftingComponents;

	std::ofstream fs;
	fs.open(ITEMS_FILE);

	fs << j;
}

void LoadData(std::vector<Item>& people)
{
	//json j;
	std::string content = "";

	std::ifstream fs;
	fs.open(ITEMS_FILE);

	while (fs.eof() == false)
	{
		Item item;

		std::getline(fs, content);

		auto j = json::parse(content);

		item.itemName = j.at("itemName");

		item.uisprDir = j.at("uisprDir");
		item.uisprWidth = j.at("uisprWidth");
		item.uisprHeight = j.at("uisprHeight");
		item.uisprXoff = j.at("uisprXoff");
		item.uisprYoff = j.at("uisprYoff");

		item.sprDir = j.at("sprDir");
		item.sprWidth = j.at("sprWidth");
		item.sprHeight = j.at("sprHeight");
		item.sprXoff = j.at("sprXoff");
		item.sprYoff = j.at("sprYoff");

		item.health = j.at("health");

		item.itemType = j.at("itemType");
		item.effectiveness = j.at("effectiveness");
		item.craftingComponents = j.at("craftingComponents");

		people.push_back(item);

		content.clear();
	}
}