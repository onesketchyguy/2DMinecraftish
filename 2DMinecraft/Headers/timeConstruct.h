// Forrest Lowe 2021

#pragma once

#ifndef TIME
#define TIME

struct TimeConstruct
{
public:
	// Amount of time since the last frame
	float elapsedTime = 0.0f;

	// Number of frames passed since the application has started. Note that this loops.
	int32_t frameCount = 0;

	void Update(float elapsedTime)
	{
		this->elapsedTime = elapsedTime;

		frameCount++;
		frameCount %= 4294967294; // Keep the frame count within the range of its values.
	}
};

#endif // !TIME