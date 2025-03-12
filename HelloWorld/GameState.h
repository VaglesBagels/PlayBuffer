#pragma once

enum Agent8State
{
    STATE_APPEAR = 0,
    STATE_HALT,
    STATE_PLAY,
    STATE_DEAD,
};

enum GameMode
{
    MODE_SPLASH,
    MODE_MENU,
    MODE_PLAYING,
    MODE_GAMEOVER,
    MODE_LEADERBOARD,
    MODE_SHOP,
    MODE_LEVELCOMPLETE,
};

struct GameState
{
    int score{ 0 };
    bool scoreSaved{ false };
    Agent8State agentState{ STATE_APPEAR };
    GameMode mode{ MODE_SPLASH };
    float progress = 0.0f;
    int currentLevel = 1;
    const int leaderboardSize = 10;
    float powerUpTimer = 0.0f;
    float powerUpDuration = 10.0f;
    bool isPowerUpActive = false;
    float matchDuration = 0;
    float invincibleTimer = 0.0f;
    float invincibleDuration = 10.0f;
    bool isInvincible = false;
    bool isExplosive = false;
    int timer = 0;
};

extern GameState gameState;