// Forrest Lowe 2020-2021

#pragma once
#ifndef UI_OBJECTS
#define UI_OBJECTS

class UIElement
{
public:
	UIElement* child = nullptr;

	int x = 0, y = 0;
	int width = 0, height = 0;
	olc::vi2d pos = { 0,0 }, scale = { 0,0 };

	olc::Pixel color = olc::NONE;
	olc::PixelGameEngine* gameEngine = nullptr;

	bool interactable = false;

	virtual bool MouseOver() {
		int mouseX = gameEngine->GetMouseX();
		int mouseY = gameEngine->GetMouseY();

		return mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height;
	}

	void TryDrawChild() {
		if (child != nullptr) child->Draw();
	}

	virtual void Draw() {
		gameEngine->FillRectDecal(pos, scale, color);
		TryDrawChild();
	}

	char id = 0;

	UIElement(olc::PixelGameEngine* gameEngine, int x, int y, int width, int height, olc::Pixel color = olc::NONE)
	{
		this->gameEngine = gameEngine;
		this->color = color;

		this->x = x;
		this->y = y;

		this->width = width;
		this->height = height;

		this->pos = { x, y };
		this->scale = { width, height };
	}

	~UIElement() {
		delete child;
	}
protected:
	UIElement() = default;
};

class TextBox : public UIElement
{
public:
	olc::vf2d textScale;
	olc::vi2d textOffset;

	olc::Pixel fillColor = olc::NONE;

	void Draw() override
	{
		if (fillColor != olc::NONE) gameEngine->FillRectDecal(pos - textOffset, scale, fillColor);
		gameEngine->DrawStringDecal(pos, text, color, textScale);

		TryDrawChild();
	}

	bool MouseOver() override {
		int mouseX = gameEngine->GetMouseX();
		int mouseY = gameEngine->GetMouseY();

		return mouseX > x - textOffset.x && mouseX < x + width - textOffset.x
			&& mouseY > y - textOffset.y && mouseY < y + height - textOffset.y;
	}

	void SetString(const char* text)
	{
		this->text = text;
	}

	void SetString(std::string text)
	{
		this->text = text;
	}

	std::string GetString()
	{
		return text;
	}

	TextBox(olc::PixelGameEngine* gameEngine, std::string text, int x, int y, int width, int height, float textWidth, float textHeight, int textOffsetX = 0, int textOffsetY = 0, olc::Pixel color = olc::BLACK, olc::Pixel fillColor = olc::NONE)
	{
		this->gameEngine = gameEngine;
		this->color = color;

		this->text = text;

		this->x = x;
		this->y = y;

		this->width = width;
		this->height = height;

		this->pos = { x, y };
		this->scale = { width, height };

		this->textScale = { textWidth, textHeight };
		this->textOffset = { textOffsetX, textOffsetY };
		this->fillColor = fillColor;
	}

protected:
	std::string text;

	TextBox() = default;
};

class SliderBar : public UIElement
{
private:
	float value = 0;

	olc::vf2d GetSliderValue() {
		return { float(scale.x - border.x * 2) * value, float(scale.y - border.y * 2) };
	}
public:
	olc::Pixel backColor = olc::BLACK;

	float borderSize = 1;
	olc::vf2d border = { borderSize, borderSize };

	void Draw() override
	{
		SetValue();

		gameEngine->FillRectDecal(pos, scale, backColor);
		gameEngine->FillRectDecal(pos + border, GetSliderValue(), color);

		TryDrawChild();
	}

	float GetValue()
	{
		return this->value;
	}

	void SetValue(float value)
	{
		this->value = value;
	}

	void SetValue()
	{
		if (interactable == false) return;

		if (MouseOver()) {
			if (gameEngine->GetMouse(0).bHeld) {
				int mx = gameEngine->GetMouseX();
				value = 1 - ((x + width) - mx) / float(width);
			}
		}
	}

	SliderBar(olc::PixelGameEngine* gameEngine, int x, int y, int width, int height, float value = 1, float borderSize = 1, olc::Pixel color = olc::WHITE, olc::Pixel backColor = olc::BLACK, bool interactable = false)
	{
		this->gameEngine = gameEngine;
		this->color = color;

		this->x = x;
		this->y = y;

		this->width = width;
		this->height = height;

		this->pos = { x, y };
		this->scale = { width, height };

		this->value = value;
		this->borderSize = borderSize;
		this->backColor = backColor;

		this->border = { borderSize, borderSize };
		this->interactable = interactable;
	}
};

class Menu
{
public:
	bool open = false;

	std::vector<UIElement*> elements;

	UIElement* Get(char item) {
		for (auto element : elements)
			if (element->id == item) return element;

		return nullptr;
	}

	void Draw()
	{
		for (auto element : elements)
		{
			element->Draw();
		}
	}

	void Initialize(std::vector<UIElement*> elements, int offsetPerItem = 1, int offsetFromTop = 0, bool skipFirstElementWhenSorting = true) {
		this->elements = elements;

		int itemOffset = 0;

		int c = -1;

		for (auto element : elements) {
			c++;

			element->id = c;

			if (c == 0 && skipFirstElementWhenSorting) continue;
			element->y = offsetFromTop + itemOffset;
			element->pos = { element->x, element->y };
			itemOffset += offsetPerItem + element->height;
		}
	}

	Menu(std::vector<UIElement*> elements, int offsetPerItem = 1, int offsetFromTop = 0, bool skipFirstElementWhenSorting = true) {
		Initialize(elements, offsetPerItem, offsetFromTop, skipFirstElementWhenSorting);
	}
};

class Notification : public TextBox {
public:
	float timeLeft;

	Notification(olc::PixelGameEngine* gameEngine, std::string text, float messageTime, int x, int y, int width, int height, float textWidth, float textHeight, int textOffsetX = 0, int textOffsetY = 0, olc::Pixel color = olc::BLACK, olc::Pixel fillColor = olc::NONE)
	{
		this->gameEngine = gameEngine;
		this->color = color;

		this->text = text;

		this->x = x;
		this->y = y;

		this->width = width;
		this->height = height;

		this->pos = { x, y };
		this->scale = { width, height };

		this->textScale = { textWidth, textHeight };
		this->textOffset = { textOffsetX, textOffsetY };
		this->fillColor = fillColor;

		this->timeLeft = messageTime;
	}
};

#endif // !UI_OBJECTS