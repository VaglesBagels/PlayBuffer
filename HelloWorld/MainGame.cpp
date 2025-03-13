#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

#include "Tool.h"
#include "Driver.h"
#include "Spanner.h"
#include "CoinStar.h"
#include "Lasers.h"
#include "GameUI.h"
#include "Game.h"
#include "Fan.h"
#include "Agent8.h"
#include "PowerUp.h"
#include "GameState.h"
#include "GameObjectType.h"
#include "Explosion.h"

extern std::vector<std::unique_ptr<Tool>> toolInstances;

int DISPLAY_WIDTH = 1280;
int DISPLAY_HEIGHT = 720;
int DISPLAY_SCALE = 1;

GameState gameState;

// The entry point for a PlayBuffer program 
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
    Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
    Play::CentreAllSpriteOrigins();
    Play::LoadBackground("Data\\Backgrounds\\background.png");
    // Play::StartAudioLoop("music");

    Agent8::SpawnAgent();
    Fan::SpawnFan();
}
GameUI gameUI(DISPLAY_WIDTH, DISPLAY_HEIGHT);

// Called by the PlayBuffer once every frame (60 times a second!) 
bool MainGameUpdate(float elapsedTime)
{
    Play::DrawBackground();

    switch (gameState.mode)
    {
        case MODE_SPLASH:
            gameUI.ShowSplashScreen();

            break;

        case MODE_MENU:
            gameUI.ShowMainMenu(gameState.isPowerUpActive, gameState.matchDuration);

            break;

        case MODE_PLAYING:
            gameState.matchDuration += elapsedTime;

            Agent8::Update(DISPLAY_HEIGHT);
            Fan::Update();
            Tool::Update();
            Coin::Update();
            Lasers::Update();
            Game::UpdateDestroyed();
            Game::UpdatePassiveUpgrades(elapsedTime, DISPLAY_HEIGHT);
            PowerUp::Update(elapsedTime);
            Explosion::Update();

            Play::DrawFontText("72px", "SCORE: " + to_string(gameState.score),
                               { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 7/8 }, Play::CENTRE);
            break;

        case MODE_GAMEOVER:
            gameUI.ShowGameOver(gameState.isPowerUpActive, gameState.matchDuration);

            break;

        case MODE_LEADERBOARD:
            gameUI.ShowLeaderboard(gameState.isPowerUpActive, gameState.matchDuration);

            break;

        case MODE_SHOP:
            gameUI.ShowShopMenu(gameState.isExplosive, gameState.isInvincible);

            break;

        case MODE_LEVELCOMPLETE:
            gameUI.ShowLevelCompleteMenu();

            break;

        default:
            cout << "ERROR 500: Internal Server Error" << endl;
            return Play::KeyDown(Play::KEY_ESCAPE);
    }

    Play::PresentDrawingBuffer();
    return Play::KeyDown(Play::KEY_ESCAPE);
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

// Notes::
/*
* Need to update leaderboard intake to take in the level and score
*/