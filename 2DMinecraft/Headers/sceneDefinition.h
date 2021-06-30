// Forrest Lowe 2021

#pragma once
#ifndef SCENE_DEFINITIONS_H
#define SCENE_DEFINITIONS_H

#include "olcPixelGameEngine.h"

#define OLC_PGEX_NETWORK
#include "olcPGEX_Network.h"
#include "networkCommon.h"

#include "ConstantData.h"

//#include "debugger.h"

#include "uiObjects.h"

#include "timeConstruct.h"
#include "renderer.h"

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

class Scene
{
public:
	Scene() = default;

protected: // FIXME: make this private
	olc::PixelGameEngine* engine = nullptr;
	Renderer* renderer = nullptr;

public:
	void Initialize(TimeConstruct* time, olc::PixelGameEngine* engine, Renderer* renderer);

protected:
	TimeConstruct* time = nullptr;

	bool cursorVisable = true;
	const float CURSOR_FLASH_TIME = 0.25f;
	float flashTime = 0;

	olc::HWButton GetKey(olc::Key key) const;
	olc::HWButton GetMouse(uint32_t button) const;
	int32_t ScreenWidth() const;
	int32_t ScreenHeight() const;
	uint32_t GetFPS() const;
	int32_t GetMouseX() const;
	int32_t GetMouseY() const;
	const olc::vi2d& GetMousePos() const;
	int32_t GetMouseWheel() const;
	float GetElapsedTime() const;
	const olc::vi2d& GetWindowSize() const;
	const olc::vi2d& GetPixelSize() const;
	const olc::vi2d& GetScreenPixelSize() const;
	const olc::vi2d& GetWindowMouse() const;
	void RecieveNumericalInput(std::string& input);
	void RecieveStringInput(std::string& input);

protected: // DRAWING ROUTINES

	// Draws a single Pixel
	bool Draw(const olc::vi2d& pos, olc::Pixel p = olc::WHITE);
	// Draws a line from (x1,y1) to (x2,y2)
	void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, olc::Pixel p = olc::WHITE, uint32_t pattern = 0xFFFFFFFF);
	void DrawLine(const olc::vi2d& pos1, const olc::vi2d& pos2, olc::Pixel p = olc::WHITE, uint32_t pattern = 0xFFFFFFFF);
	// Draws a circle located at (x,y) with radius
	void DrawCircle(int32_t x, int32_t y, int32_t radius, olc::Pixel p = olc::WHITE, uint8_t mask = 0xFF);
	void DrawCircle(const olc::vi2d& pos, int32_t radius, olc::Pixel p = olc::WHITE, uint8_t mask = 0xFF);
	// Fills a circle located at (x,y) with radius
	void FillCircle(int32_t x, int32_t y, int32_t radius, olc::Pixel p = olc::WHITE);
	void FillCircle(const olc::vi2d& pos, int32_t radius, olc::Pixel p = olc::WHITE);
	// Draws a rectangle at (x,y) to (x+w,y+h)
	void DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, olc::Pixel p = olc::WHITE);
	void DrawRect(const olc::vi2d& pos, const olc::vi2d& size, olc::Pixel p = olc::WHITE);
	// Fills a rectangle at (x,y) to (x+w,y+h)
	void FillRect(int32_t x, int32_t y, int32_t w, int32_t h, olc::Pixel p = olc::WHITE);
	void FillRect(const olc::vi2d& pos, const olc::vi2d& size, olc::Pixel p = olc::WHITE);
	// Draws a triangle between points (x1,y1), (x2,y2) and (x3,y3)
	void DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, olc::Pixel p = olc::WHITE);
	void DrawTriangle(const olc::vi2d& pos1, const olc::vi2d& pos2, const olc::vi2d& pos3, olc::Pixel p = olc::WHITE);
	// Flat fills a triangle between points (x1,y1), (x2,y2) and (x3,y3)
	void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, olc::Pixel p = olc::WHITE);
	void FillTriangle(const olc::vi2d& pos1, const olc::vi2d& pos2, const olc::vi2d& pos3, olc::Pixel p = olc::WHITE);
	// Draws an entire sprite at location (x,y)
	void DrawSprite(int32_t x, int32_t y, olc::Sprite* sprite, uint32_t scale = 1, uint8_t flip = olc::Sprite::NONE);
	void DrawSprite(const olc::vi2d& pos, olc::Sprite* sprite, uint32_t scale = 1, uint8_t flip = olc::Sprite::NONE);
	// Draws an area of a sprite at location (x,y), where the
	// selected area is (ox,oy) to (ox+w,oy+h)
	void DrawPartialSprite(int32_t x, int32_t y, olc::Sprite* sprite, int32_t ox, int32_t oy, int32_t w, int32_t h, uint32_t scale = 1, uint8_t flip = olc::Sprite::NONE);
	void DrawPartialSprite(const olc::vi2d& pos, olc::Sprite* sprite, const olc::vi2d& sourcepos, const olc::vi2d& size, uint32_t scale = 1, uint8_t flip = olc::Sprite::NONE);
	// Draws a single line of text - traditional monospaced
	void DrawString(int32_t x, int32_t y, const std::string& sText, olc::Pixel col = olc::WHITE, uint32_t scale = 1);
	void DrawString(const olc::vi2d& pos, const std::string& sText, olc::Pixel col = olc::WHITE, uint32_t scale = 1);
	olc::vi2d GetTextSize(const std::string& s);
	// Draws a single line of text - non-monospaced
	void DrawStringProp(int32_t x, int32_t y, const std::string& sText, olc::Pixel col = olc::WHITE, uint32_t scale = 1);
	void DrawStringProp(const olc::vi2d& pos, const std::string& sText, olc::Pixel col = olc::WHITE, uint32_t scale = 1);
	olc::vi2d GetTextSizeProp(const std::string& s);

	// Decal Quad functions
	void SetDecalMode(const olc::DecalMode& mode);
	// Draws a whole decal, with optional scale and tinting
	void DrawDecal(const olc::vf2d& pos, olc::Decal* decal, const olc::vf2d& scale = { 1.0f,1.0f }, const olc::Pixel& tint = olc::WHITE);
	// Draws a region of a decal, with optional scale and tinting
	void DrawPartialDecal(const olc::vf2d& pos, olc::Decal* decal, const olc::vf2d& source_pos, const olc::vf2d& source_size, const olc::vf2d& scale = { 1.0f,1.0f }, const olc::Pixel& tint = olc::WHITE);
	void DrawPartialDecal(const olc::vf2d& pos, const olc::vf2d& size, olc::Decal* decal, const olc::vf2d& source_pos, const olc::vf2d& source_size, const olc::Pixel& tint = olc::WHITE);
	// Draws fully user controlled 4 vertices, pos(pixels), uv(pixels), colours
	void DrawExplicitDecal(olc::Decal* decal, const olc::vf2d* pos, const olc::vf2d* uv, const olc::Pixel* col, uint32_t elements = 4);
	// Draws a decal with 4 arbitrary points, warping the texture to look "correct"
	void DrawWarpedDecal(olc::Decal* decal, const olc::vf2d(&pos)[4], const olc::Pixel& tint = olc::WHITE);
	void DrawWarpedDecal(olc::Decal* decal, const olc::vf2d* pos, const olc::Pixel& tint = olc::WHITE);
	void DrawWarpedDecal(olc::Decal* decal, const std::array<olc::vf2d, 4>& pos, const olc::Pixel& tint = olc::WHITE);
	// As above, but you can specify a region of a decal source sprite
	void DrawPartialWarpedDecal(olc::Decal* decal, const olc::vf2d(&pos)[4], const olc::vf2d& source_pos, const olc::vf2d& source_size, const olc::Pixel& tint = olc::WHITE);
	void DrawPartialWarpedDecal(olc::Decal* decal, const olc::vf2d* pos, const olc::vf2d& source_pos, const olc::vf2d& source_size, const olc::Pixel& tint = olc::WHITE);
	void DrawPartialWarpedDecal(olc::Decal* decal, const std::array<olc::vf2d, 4>& pos, const olc::vf2d& source_pos, const olc::vf2d& source_size, const olc::Pixel& tint = olc::WHITE);
	// Draws a decal rotated to specified angle, wit point of rotation offset
	void DrawRotatedDecal(const olc::vf2d& pos, olc::Decal* decal, const float fAngle, const olc::vf2d& center = { 0.0f, 0.0f }, const olc::vf2d& scale = { 1.0f,1.0f }, const olc::Pixel& tint = olc::WHITE);
	void DrawPartialRotatedDecal(const olc::vf2d& pos, olc::Decal* decal, const float fAngle, const olc::vf2d& center, const olc::vf2d& source_pos, const olc::vf2d& source_size, const olc::vf2d& scale = { 1.0f, 1.0f }, const olc::Pixel& tint = olc::WHITE);
	// Draws a multiline string as a decal, with tiniting and scaling
	void DrawStringDecal(const olc::vf2d& pos, const std::string& sText, const  olc::Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });
	void DrawStringPropDecal(const olc::vf2d& pos, const std::string& sText, const  olc::Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });
	// Draws a single shaded filled rectangle as a decal
	void FillRectDecal(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col = olc::WHITE);
	// Draws a corner shaded rectangle as a decal
	void GradientFillRectDecal(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel colTL, const olc::Pixel colBL, const olc::Pixel colBR, const olc::Pixel colTR);
	// Draws an arbitrary convex textured polygon using GPU
	void DrawPolygonDecal(olc::Decal* decal, const std::vector<olc::vf2d>& pos, const std::vector<olc::vf2d>& uv, const olc::Pixel tint = olc::WHITE);

	// Clears entire draw target to Pixel
	void Clear(olc::Pixel p);
	// Clears the rendering back buffer
	void ClearBuffer(olc::Pixel p, bool bDepth = true);
	// Returns the font image
	olc::Sprite* GetFontSprite();

private:
	bool sceneLoaded = false;

public:
	bool UpdateScene();

	virtual bool OnLoad() = 0;
	virtual bool Update() = 0;
};

#endif // !SCENE_DEFINITIONS_H