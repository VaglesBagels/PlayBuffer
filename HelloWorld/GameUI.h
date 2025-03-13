#pragma once

class GameUI
{
public:
	GameUI(int displayWidth, int displayHeight);

	void ShowSplashScreen();
	void ShowMainMenu(bool& isPowerUpActive, float& matchDuration);
	void ShowGameOver(bool& isPowerUpActive, float& matchDuration);
	void ShowLeaderboard(bool& isPowerUpActive, float& matchDuration);
	void ShowShopMenu(bool& isExplosive, bool& isInvincible);
	void ShowLevelCompleteMenu();

private:
	int DISPLAY_WIDTH;
	int DISPLAY_HEIGHT;
};

