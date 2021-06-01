#include "Headers/sceneDefinition.h"

void Scene::Initialize(TimeConstruct* time, olc::PixelGameEngine* engine)
{
	this->time = time;
	this->engine = engine;
}

bool Scene::UpdateScene()
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

olc::HWButton Scene::GetKey(olc::Key key) const
{
	return engine->GetKey(key);
}

olc::HWButton Scene::GetMouse(uint32_t button) const
{
	return engine->GetMouse(button);
}

int32_t Scene::ScreenWidth() const
{
	return engine->ScreenWidth();
}

int32_t Scene::ScreenHeight() const
{
	return engine->ScreenHeight();
}

uint32_t Scene::GetFPS() const
{
	return engine->GetFPS();
}

int32_t Scene::GetMouseX() const
{
	return engine->GetMouseX();
}

int32_t Scene::GetMouseY() const
{
	return engine->GetMouseY();
}

const olc::vi2d& Scene::GetMousePos() const
{
	return engine->GetMousePos();
}

int32_t Scene::GetMouseWheel() const
{
	return engine->GetMouseWheel();
}

float Scene::GetElapsedTime() const
{
	return engine->GetElapsedTime();
}

const olc::vi2d& Scene::GetWindowSize() const
{
	return engine->GetWindowSize();
}

const olc::vi2d& Scene::GetPixelSize() const
{
	return engine->GetPixelSize();
}

const olc::vi2d& Scene::GetScreenPixelSize() const
{
	return engine->GetScreenPixelSize();
}

const olc::vi2d& Scene::GetWindowMouse() const
{
	return engine->GetWindowMouse();
}

bool Scene::Draw(const olc::vi2d& pos, olc::Pixel p)
{
	return engine->Draw(pos, p);
}

void Scene::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
	olc::Pixel p, uint32_t pattern)
{
	engine->DrawLine(x1, y1, x2, y2, p, pattern);
}
void Scene::DrawLine(const olc::vi2d& pos1, const olc::vi2d& pos2, olc::Pixel p,
	uint32_t pattern)
{
	engine->DrawLine(pos1, pos2, p, pattern);
}

void Scene::DrawCircle(int32_t x, int32_t y, int32_t radius, olc::Pixel p,
	uint8_t mask)
{
	engine->DrawCircle(x, y, radius, p, mask);
}
void Scene::DrawCircle(const olc::vi2d& pos, int32_t radius, olc::Pixel p,
	uint8_t mask)
{
	engine->DrawCircle(pos, radius, p, mask);
}

void Scene::FillCircle(int32_t x, int32_t y, int32_t radius, olc::Pixel p)
{
	engine->FillCircle(x, y, radius, p);
}
void Scene::FillCircle(const olc::vi2d& pos, int32_t radius, olc::Pixel p)
{
	engine->FillCircle(pos, radius, p);
}

void Scene::DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, olc::Pixel p)
{
	engine->DrawRect(x, y, w, h, p);
}
void Scene::DrawRect(const olc::vi2d& pos, const olc::vi2d& size, olc::Pixel p)
{
	engine->DrawRect(pos, size, p);
}

void Scene::FillRect(int32_t x, int32_t y, int32_t w, int32_t h, olc::Pixel p)
{
	engine->FillRect(x, y, w, h, p);
}
void Scene::FillRect(const olc::vi2d& pos, const olc::vi2d& size, olc::Pixel p)
{
	engine->FillRect(pos, size, p);
}

void Scene::DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
	int32_t x3, int32_t y3, olc::Pixel p)
{
	engine->DrawTriangle(x1, y1, x2, y2, x3, y3, p);
}
void Scene::DrawTriangle(const olc::vi2d& pos1, const olc::vi2d& pos2,
	const olc::vi2d& pos3, olc::Pixel p)
{
	engine->DrawTriangle(pos1, pos2, pos3, p);
}

void Scene::FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
	int32_t x3, int32_t y3, olc::Pixel p)
{
	engine->FillTriangle(x1, y1, x2, y2, x3, y3, p);
}
void Scene::FillTriangle(const olc::vi2d& pos1, const olc::vi2d& pos2,
	const olc::vi2d& pos3, olc::Pixel p)
{
	engine->FillTriangle(pos1, pos2, pos3, p);
}

void Scene::DrawSprite(int32_t x, int32_t y, olc::Sprite* sprite, uint32_t scale,
	uint8_t flip)
{
	engine->DrawSprite(x, y, sprite, scale, flip);
}
void Scene::DrawSprite(const olc::vi2d& pos, olc::Sprite* sprite, uint32_t scale,
	uint8_t flip)
{
	engine->DrawSprite(pos, sprite, scale, flip);
}

void Scene::DrawPartialSprite(int32_t x, int32_t y, olc::Sprite* sprite,
	int32_t ox, int32_t oy, int32_t w, int32_t h, uint32_t scale, uint8_t flip)
{
	engine->DrawPartialSprite(x, y, sprite, ox, oy, w, h, scale, flip);
}
void Scene::DrawPartialSprite(const olc::vi2d& pos, olc::Sprite* sprite,
	const olc::vi2d& sourcepos, const olc::vi2d& size, uint32_t scale, uint8_t flip)
{
	engine->DrawPartialSprite(pos, sprite, sourcepos, size, scale, flip);
}

void Scene::DrawString(int32_t x, int32_t y, const std::string& sText,
	olc::Pixel col, uint32_t scale)
{
	engine->DrawString(x, y, sText, col, scale);
}
void Scene::DrawString(const olc::vi2d& pos, const std::string& sText,
	olc::Pixel col, uint32_t scale)
{
	engine->DrawString(pos, sText, col, scale);
}
olc::vi2d Scene::GetTextSize(const std::string& s)
{
	return engine->GetTextSize(s);
}

void Scene::DrawStringProp(int32_t x, int32_t y, const std::string& sText,
	olc::Pixel col, uint32_t scale)
{
	engine->DrawStringProp(x, y, sText, col, scale);
}
void Scene::DrawStringProp(const olc::vi2d& pos, const std::string& sText,
	olc::Pixel col, uint32_t scale)
{
	engine->DrawStringProp(pos, sText, col, scale);
}
olc::vi2d Scene::GetTextSizeProp(const std::string& s)
{
	return engine->GetTextSizeProp(s);
}

void Scene::SetDecalMode(const olc::DecalMode& mode)
{
	engine->SetDecalMode(mode);
}

void Scene::DrawDecal(const olc::vf2d& pos, olc::Decal* decal,
	const olc::vf2d& scale, const olc::Pixel& tint)
{
	engine->DrawDecal(pos, decal, scale, tint);
}

void Scene::DrawPartialDecal(const olc::vf2d& pos, olc::Decal* decal,
	const olc::vf2d& source_pos, const olc::vf2d& source_size,
	const olc::vf2d& scale, const olc::Pixel& tint)
{
	engine->DrawPartialDecal(pos, decal, source_pos, source_size, scale, tint);
}

void Scene::DrawPartialDecal(const olc::vf2d& pos, const olc::vf2d& size,
	olc::Decal* decal, const olc::vf2d& source_pos, const olc::vf2d& source_size,
	const olc::Pixel& tint)
{
	engine->DrawPartialDecal(pos, size, decal, source_pos, source_size, tint);
}

void Scene::DrawExplicitDecal(olc::Decal* decal, const olc::vf2d* pos,
	const olc::vf2d* uv, const olc::Pixel* col, uint32_t elements)
{
	engine->DrawExplicitDecal(decal, pos, uv, col, elements);
}

void Scene::DrawWarpedDecal(olc::Decal* decal, const olc::vf2d(&pos)[4],
	const olc::Pixel& tint)
{
	// FIXME: fix this
	//print("ERROR: Function not implemented! -> DrawWarpedDecal(olc::Decal*, const olc::vf2d(&)[4], const olc::Pixel& )");
	//engine->DrawWarpedDecal(decal, pos, tint);
}
void Scene::DrawWarpedDecal(olc::Decal* decal, const olc::vf2d* pos,
	const olc::Pixel& tint)
{
	engine->DrawWarpedDecal(decal, pos, tint);
}
void Scene::DrawWarpedDecal(olc::Decal* decal, const std::array<olc::vf2d, 4>& pos,
	const olc::Pixel& tint)
{
	engine->DrawWarpedDecal(decal, pos, tint);
}

void Scene::DrawPartialWarpedDecal(olc::Decal* decal, const olc::vf2d(&pos)[4],
	const olc::vf2d& source_pos, const olc::vf2d& source_size,
	const olc::Pixel& tint)
{
	// FIXME: fix this
	//print("ERROR: Function not implemented! -> DrawPartialWarpedDecal(olc::Decal*, const olc::vf2d()[4], const olc::vf2d&, const olc::vf2d&,const olc::Pixel&)");
	//engine->DrawPartialWarpedDecal(decal, pos, source_pos, source_size, tint);
}
void Scene::DrawPartialWarpedDecal(olc::Decal* decal, const olc::vf2d* pos,
	const olc::vf2d& source_pos, const olc::vf2d& source_size,
	const olc::Pixel& tint)
{
	engine->DrawPartialWarpedDecal(decal, pos, source_pos, source_size, tint);
}
void Scene::DrawPartialWarpedDecal(olc::Decal* decal, const std::array<olc::vf2d,
	4>& pos, const olc::vf2d& source_pos, const olc::vf2d& source_size,
	const olc::Pixel& tint)
{
	engine->DrawPartialWarpedDecal(decal, pos, source_pos, source_size, tint);
}

void Scene::DrawRotatedDecal(const olc::vf2d& pos, olc::Decal* decal,
	const float fAngle, const olc::vf2d& center, const olc::vf2d& scale,
	const olc::Pixel& tint)
{
	engine->DrawRotatedDecal(pos, decal, fAngle, center, scale, tint);
}
void Scene::DrawPartialRotatedDecal(const olc::vf2d& pos, olc::Decal* decal,
	const float fAngle, const olc::vf2d& center, const olc::vf2d& source_pos,
	const olc::vf2d& source_size, const olc::vf2d& scale, const olc::Pixel& tint)
{
	engine->DrawPartialRotatedDecal(pos, decal, fAngle, center, source_pos, source_size, scale, tint);
}

void Scene::DrawStringDecal(const olc::vf2d& pos, const std::string& sText,
	const  olc::Pixel col, const olc::vf2d& scale)
{
	engine->DrawStringDecal(pos, sText, col, scale);
}
void Scene::DrawStringPropDecal(const olc::vf2d& pos, const std::string& sText,
	const  olc::Pixel col, const olc::vf2d& scale)
{
	engine->DrawStringDecal(pos, sText, col, scale);
}

void Scene::FillRectDecal(const olc::vf2d& pos, const olc::vf2d& size,
	const olc::Pixel col)
{
	engine->FillRectDecal(pos, size, col);
}

void Scene::GradientFillRectDecal(const olc::vf2d& pos, const olc::vf2d& size,
	const olc::Pixel colTL, const olc::Pixel colBL, const olc::Pixel colBR, const olc::Pixel colTR)
{
	engine->GradientFillRectDecal(pos, size, colTL, colBL, colBR, colTR);
}

void Scene::DrawPolygonDecal(olc::Decal* decal, const std::vector<olc::vf2d>& pos,
	const std::vector<olc::vf2d>& uv, const olc::Pixel tint)
{
	engine->DrawPolygonDecal(decal, pos, uv, tint);
}

void Scene::Clear(olc::Pixel p)
{
	engine->Clear(p);
}

void Scene::ClearBuffer(olc::Pixel p, bool bDepth)
{
	engine->ClearBuffer(p, bDepth);
}

olc::Sprite* Scene::GetFontSprite()
{
	return engine->GetFontSprite();
}