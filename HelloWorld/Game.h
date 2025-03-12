#pragma once
class Game
{
public:
	static void TriggerGameStart(bool& isPowerUpActive, float& matchDuration);
	static void ResetGameState();
	static void DrawProgressBar(float barDuration, float totalDuration, Play::Point2D bottomLeftCorner, float barThickness, float barLength, Play::Colour colour);
};

