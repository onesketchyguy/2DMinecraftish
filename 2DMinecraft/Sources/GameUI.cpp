#include "../Headers/gameScene.h"
/*
* Handles all the UI NOT being handled by the inventory
*/

void GameScene::DisplayNotification(std::string notification)
{
	notifications.push_back(notification);
	totalNotificationTime += NOTIFICATION_SHOW_TIME;
}

void GameScene::HandleNotifications()
{
	if (totalNotificationTime > 0.0f)
	{
		totalNotificationTime -= time->elapsedTime * notifications.size();
		currentNotificationTime = totalNotificationTime / notifications.size();
	}
	else
	{
		totalNotificationTime = 0.0f;
	}

	bool showNext = currentNotificationTime <= NOTIFICATION_SHOW_TIME && notifications.size() > 1 ||
		currentNotificationTime <= 0.0f && notifications.size() == 1;

	if (showNext && notifications.size() > 0)
	{
		// Remove the oldest element by reversing the vector
		std::reverse(notifications.begin(), notifications.end());
		notifications.pop_back(); // removing the last element in the vector

		// then returning the vector to it's origonal length
		if (notifications.size() > 0) std::reverse(notifications.begin(), notifications.end());
	}

	if (notifications.size() > 0)
	{
		float offsetY = 0.0f;
		olc::vf2d textScale{ 0.6f, 0.6f };

		for (auto& currentNotification : notifications)
		{
			if (currentNotification.length() > 0)
			{
				float backWidth = 8 * textScale.x * currentNotification.length();
				float backHeight = 8 * textScale.y * 1.5f;

				DrawDecal(olc::vf2d{ 0.0f, offsetY }, renderer->squareDecal, olc::vf2d{ backWidth, backHeight }, olc::BLACK);

				DrawStringPropDecal(olc::vf2d{ 16 * textScale.x, offsetY + (8 * textScale.y * 0.5f) }, currentNotification, olc::WHITE, textScale * 0.9f);
				offsetY += 16 * textScale.y;
			}
		}
	}
}

void GameScene::HandleToolTip()
{
	if (toolTipText.length() > 0)
	{
		olc::vf2d textScale{ 0.6f, 0.6f };

		float backWidth = 10 * textScale.x * toolTipText.length();
		float backHeight = 8 * textScale.y * 1.5f;

		DrawDecal({ toolTipPos.x, toolTipPos.y }, renderer->squareDecal, olc::vf2d{ backWidth, backHeight }, olc::BLACK);
		DrawStringDecal(toolTipPos, toolTipText, olc::WHITE, textScale);

		toolTipText.pop_back();
	}
}