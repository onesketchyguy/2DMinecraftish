// Forrest Lowe 2021
#ifndef ITEM
#define ITEM

#include "Headers/olcPixelGameEngine.h"

class Item
{
public:
	olc::vf2d position = { 0, 0 };
	uint8_t ID = 0;

public:
	Item(olc::vf2d position, uint8_t ID);
	Item() = default;
};

#endif // !ITEM