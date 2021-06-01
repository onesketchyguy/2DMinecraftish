#pragma once

#ifndef INTRO_SCENE_H
#define INTRO_SCENE_H

#include "sceneDefinition.h"

class IntroScene : public Scene
{
	olc::Renderable* olc_logo;
	olc::Renderable* flowe_logo;
	olc::vf2d logoPos;
	olc::vf2d olc_logoSize = { 0.25f, 0.25f };
	olc::vf2d flowe_logoSize = { 1.25f, 1.25f };

	float logoTime = 3;
	float halfTime = 0;
	float showTime = 2;

public:
	~IntroScene()
	{
		delete olc_logo;
		delete flowe_logo;
	}

public:
	bool OnLoad() override;
	bool Update() override;
};

#endif // !INTRO_SCENE_H