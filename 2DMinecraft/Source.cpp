#define OLC_PGE_APPLICATION
#include "Headers/olcPixelGameEngine.h"
#include "Headers/objectDefinitions.h"

// Override base class with your custom functionality
class MiniMinecraft : public olc::PixelGameEngine
{
public:
	Object* player;

public:
	MiniMinecraft()
	{
		// Name your application
		sAppName = "Mini minecraft";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));
		return true;
	}
};

int main()
{
	// Hide console
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	// Show game
	MiniMinecraft demo;
	if (demo.Construct(256 / 2, 200 / 2, 4 * 2, 4 * 2))
		demo.Start();
	return 0;
}