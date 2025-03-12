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

extern std::vector<std::unique_ptr<Tool>> toolInstances;

int DISPLAY_WIDTH = 1280;
int DISPLAY_HEIGHT = 720;
int DISPLAY_SCALE = 1;
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
};

GameState gameState;

enum GameObjectType
{
	TYPE_NULL = -1,
	TYPE_AGENT8,
    TYPE_FAN,
    TYPE_TOOL,
    TYPE_COIN,
    TYPE_STAR,
    TYPE_LASER,
    TYPE_DESTROYED,
    TYPE_POWERUP,
    TYPE_CASH,
    TYPE_EXPLOSIONS,
};

// Function Declarations
void HandlePlayerControls();
void UpdateFan();
//void UpdateTools();
void UpdateCoinsAndStars();
void UpdateLasers();
void UpdateDestroyed();
void UpdateAgent8();
void UpdatePowerUp(float elapsedTime);
void UpdatePassiveUpgrades(float elapsedTime);
void UpdateExplosions();

// The entry point for a PlayBuffer program 
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
    Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
    Play::CentreAllSpriteOrigins();
    Play::LoadBackground("Data\\Backgrounds\\background.png");
    // Play::StartAudioLoop("music");
    Play::CreateGameObject(TYPE_AGENT8, { 115, 600 }, 50, "agent8");
    int id_fan = Play::CreateGameObject(TYPE_FAN, { 1140, 503 }, 0, "fan");
    Play::GetGameObject(id_fan).velocity = { 0, -3 };
    Play::GetGameObject(id_fan).animSpeed = 1.0f;
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
            gameUI.ShowMainMenu(isPowerUpActive, matchDuration);

            break;

        case MODE_PLAYING:
            matchDuration += elapsedTime;

            UpdateAgent8();
            UpdateFan();
            Tool::Update();
            UpdateCoinsAndStars();
            UpdateLasers();
            UpdateDestroyed();
            Play::DrawFontText("72px", "SCORE: " + to_string(gameState.score),
                               { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 7/8 }, Play::CENTRE);

            UpdatePowerUp(elapsedTime);
            UpdatePassiveUpgrades(elapsedTime);
            UpdateExplosions();
            break;

        case MODE_GAMEOVER:
            gameUI.ShowGameOver(isPowerUpActive, matchDuration);

            break;

        case MODE_LEADERBOARD:
            gameUI.ShowLeaderboard(isPowerUpActive, matchDuration);

            break;

        case MODE_SHOP:
            gameUI.ShowShopMenu(isExplosive, isInvincible);

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

void HandlePlayerControls()
{
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
    if (Play::KeyDown(Play::KEY_UP))
    {
        obj_agent8.velocity = { 0, 4 };
        Play::SetSprite(obj_agent8, "agent8_climb", 0.25f);
    }
    else if (Play::KeyDown(Play::KEY_DOWN))
    {
        obj_agent8.acceleration = { 0, -1 };
        Play::SetSprite(obj_agent8, "agent8_fall", 0);
    }
    else
    {
        if (obj_agent8.velocity.y < -5)
        {
            gameState.agentState = STATE_HALT;
            Play::SetSprite(obj_agent8, "agent8_halt", 0.333f);
            obj_agent8.acceleration = { 0,0 };
        }
        else
        {
            Play::SetSprite(obj_agent8, "agent8_hang", 0.02f);
            obj_agent8.velocity *= 0.5f;
            obj_agent8.acceleration = { 0, 0 };
        }
    }

    if (Play::KeyPressed(Play::KEY_SPACE))
    {
        Vector2D firePos = obj_agent8.pos + Vector2D(155, 75);
        int id = Play::CreateGameObject(TYPE_LASER, firePos, 30, "laser");
        Play::GetGameObject(id).velocity = { 32, 0 };
        // Play::PlayAudio("shoot");
    }
    Play::UpdateGameObject(obj_agent8);

    if (Play::IsLeavingDisplayArea(obj_agent8))
        obj_agent8.pos = obj_agent8.oldPos;

    Play::DrawLine({ obj_agent8.pos.x, 720 }, obj_agent8.pos, Play::cWhite);
    Play::DrawObjectRotated(obj_agent8);

    if (Play::KeyPressed(Play::KEY_S))
    {
        gameState.mode = MODE_SHOP;
    }
}

void UpdateFan()
{
    GameObject& obj_fan = Play::GetGameObjectByType(TYPE_FAN);

    Play::UpdateGameObject(obj_fan);

    if (Play::IsLeavingDisplayArea(obj_fan))
    {
        obj_fan.pos = obj_fan.oldPos;
        obj_fan.velocity.y *= -1;
    }

    Play::DrawObject(obj_fan);
}

void UpdateCoinsAndStars()
{
    Play::GameObject& obj_fan = Play::GetGameObjectByType(TYPE_FAN);
    Play::GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
    std::vector<int> vCoins = Play::CollectGameObjectIDsByType(TYPE_COIN);

    Coin::SpawnCoins(obj_fan, isPowerUpActive, matchDuration);

    std::vector<Coin> coins;

    for (int id_coin : vCoins)
    {
        // Found this, creates a new coin for each generate coin object
        coins.emplace_back();
    }

    for (int i = 0; i < vCoins.size(); i++)
    {
        Play::GameObject& obj_coin = Play::GetGameObject(vCoins[i]);

        coins[i].HandleCollision(obj_coin, obj_agent8, isPowerUpActive);
        Play::UpdateGameObject(obj_coin);
        Play::DrawObjectRotated(obj_coin);
        coins[i].HandleDeconstruction(obj_coin, vCoins[i]);
    }

    std::vector<int> vStars = Play::CollectGameObjectIDsByType(TYPE_STAR);

    for (int id_star : vStars)
    {
        Play::GameObject& obj_star = Play::GetGameObject(id_star);

        Play::UpdateGameObject(obj_star);
        Play::DrawObjectRotated(obj_star);

        if (!Play::IsVisible(obj_star))
        {
            Play::DestroyGameObject(id_star);
        }
    }
}

void UpdateLasers()
{
    std::vector<int> vLasers = Play::CollectGameObjectIDsByType(TYPE_LASER);
    std::vector<int> vTools = Play::CollectGameObjectIDsByType(TYPE_TOOL);
    std::vector<int> vCoins = Play::CollectGameObjectIDsByType(TYPE_COIN);
    std::vector<int> vExplosions = Play::CollectGameObjectIDsByType(TYPE_EXPLOSIONS);

    Lasers lasers;

    for (int id_laser : vLasers)
    {
        GameObject& obj_laser = Play::GetGameObject(id_laser);
        bool hasCollided = false;

        for (int id_tool : vTools)
        {
            GameObject& obj_tool = Play::GetGameObject(id_tool);
            lasers.HandleExplosiveShot(obj_laser, obj_tool, id_tool, isExplosive);
        }

        for (int id_coin : vCoins)
        {
            GameObject& obj_coin = Play::GetGameObject(id_coin);
            lasers.HandleCoinCollision(obj_laser, obj_coin);
        }

        if (gameState.score < 0)
        {
            gameState.score = 0;
        }

        Play::UpdateGameObject(obj_laser);
        Play::DrawObject(obj_laser);

        lasers.HandleDeconstruction(obj_laser, id_laser);
    }
}

void UpdateDestroyed()
{
    std::vector<int> vDead = Play::CollectGameObjectIDsByType(TYPE_DESTROYED);

    for (int id_dead : vDead)
    {
        GameObject& obj_dead = Play::GetGameObject(id_dead);
        obj_dead.animSpeed = 0.2f;
        Play::UpdateGameObject(obj_dead);

        if (obj_dead.frame % 2)
        {
            Play::DrawObjectRotated(obj_dead, (10 - obj_dead.frame) / 10.0f);
        }

        if (!Play::IsVisible(obj_dead) || obj_dead.frame >= 10)
        {
            Play::DestroyGameObject(id_dead);
        }
    }
}

void UpdateAgent8()
{
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

    switch (gameState.agentState)
    {
        case STATE_APPEAR:
            obj_agent8.velocity = { 0, -12 };
            obj_agent8.acceleration = { 0, -0.5f };
            Play::SetSprite(obj_agent8, "agent8_fall", 0);
            obj_agent8.rotation = 0;

            if (obj_agent8.pos.y <= DISPLAY_HEIGHT * 0.66f) { gameState.agentState = STATE_PLAY; }
            break;

        case STATE_HALT:
            obj_agent8.velocity *= 0.9f;

            if (Play::IsAnimationComplete(obj_agent8)) { gameState.agentState = STATE_PLAY; }  
            break;

        case STATE_PLAY:
            HandlePlayerControls();
            break;

        case STATE_DEAD:
            obj_agent8.acceleration = { -0.3f , 0.5f };
            obj_agent8.rotation += 0.25f;
            
            if (Play::IsAnimationComplete(obj_agent8)) { gameState.mode = MODE_GAMEOVER; }
            break;
    }

    Play::UpdateGameObject(obj_agent8);

    if (Play::IsLeavingDisplayArea(obj_agent8) && gameState.agentState != STATE_DEAD)
    {
        obj_agent8.pos = obj_agent8.oldPos;
    }

    Play::DrawLine({ obj_agent8.pos.x, 720 }, obj_agent8.pos, Play::cWhite);
    Play::DrawObjectRotated(obj_agent8);
}

void UpdatePowerUp(float elapsedTime)
{
    GameObject& obj_fan = Play::GetGameObjectByType(TYPE_FAN);
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

    std::vector<int> vPowerUps = Play::CollectGameObjectIDsByType(TYPE_POWERUP);

    bool hasCollided = false;

    if (vPowerUps.size() != 1 && !isPowerUpActive)
    {
        if (Play::RandomRoll(500) == 1)
        {
            int id_money = Play::CreateGameObject(TYPE_POWERUP, obj_fan.pos, 20, "money");
            GameObject& obj_powerup = Play::GetGameObject(id_money);
            obj_powerup.velocity = { -6, 0 };
            obj_powerup.velocity.x -= (matchDuration * 0.1f);
            obj_powerup.rotSpeed = 0.1f;
        }
    }
    else
    {
        if (Play::RandomRoll(100) == 1 && isPowerUpActive)
        {
            int id_coin = Play::CreateGameObject(TYPE_COIN, obj_fan.pos, 20, "coin");
            GameObject& obj_coin = Play::GetGameObject(id_coin);
            Play::SetSprite(obj_coin, "coins", 0.1f);
            obj_coin.velocity = { -10, 0 };
            obj_coin.rotSpeed = 0.1f;
        }
    }

    for (int id_money : vPowerUps)
    {
        GameObject& obj_powerup = Play::GetGameObject(id_money);

        if (Play::IsColliding(obj_agent8, obj_powerup))
        {
            for (float rad{ 0.125f }; rad < 2.0f; rad += 0.125f)
            {
                int id_cash = Play::CreateGameObject(TYPE_CASH, obj_agent8.pos, 0, "cash");
                GameObject& obj_cash = Play::GetGameObject(id_cash);

                obj_cash.rotSpeed = 0.1f;
                obj_cash.acceleration = { 0.0f, 0.0f };
                Play::SetGameObjectDirection(obj_cash, 16, rad * PLAY_PI);
            }

            hasCollided = true;
            // Play::PlayAudio("collect");

            isPowerUpActive = true;
        }

        Play::UpdateGameObject(obj_powerup);
        Play::DrawObjectRotated(obj_powerup);

        if (!Play::IsVisible(obj_powerup) || hasCollided)
        {
            Play::DestroyGameObject(id_money);
        }
    }

    std::vector<int> vCash = Play::CollectGameObjectIDsByType(TYPE_CASH);

    for (int id_cash : vCash)
    {
        GameObject& obj_cash = Play::GetGameObject(id_cash);

        Play::UpdateGameObject(obj_cash);
        Play::DrawObjectRotated(obj_cash);

        if (!Play::IsVisible(obj_cash))
        {
            Play::DestroyGameObject(id_cash);
        }
    }

    if (isPowerUpActive)
    {
        powerUpTimer += elapsedTime;

        Game::DrawProgressBar(powerUpTimer, powerUpDuration, { 50, 50 }, 25, 200, cMagenta);

        // Play::DrawDebugText({ 50, 50 }, to_string(powerUpTimer).c_str());

        if (powerUpTimer >= powerUpDuration)
        {
            isPowerUpActive = false;
            powerUpTimer = 0.0f;
        }
    }
}

void UpdatePassiveUpgrades(float elapsedTime)
{
    if (gameState.mode != MODE_SHOP && isInvincible)
    {
        invincibleTimer += elapsedTime;

        Game::DrawProgressBar(invincibleTimer, invincibleDuration, { 50, DISPLAY_HEIGHT * 5 /8 }, 25, 200, cCyan);

        if (invincibleTimer > invincibleDuration)
        {
            invincibleTimer = 0.0f;
            isInvincible = false;
        }
    }
}

void UpdateExplosions()
{
    std::vector<int> vExplosions = Play::CollectGameObjectIDsByType(TYPE_EXPLOSIONS);
    std::vector<int> vTools = Play::CollectGameObjectIDsByType(TYPE_TOOL);

    for (int id_explosion : vExplosions)
    {
        GameObject& obj_explosion = Play::GetGameObject(id_explosion);
        bool hasCollided = false;

        for (int id_tool : vTools)
        {
            GameObject& obj_tool = Play::GetGameObject(id_tool);

            if (Play::IsColliding(obj_explosion, obj_tool))
            {
                hasCollided = true;
                Play::DestroyGameObject(id_tool);
                gameState.score += 100;
            }
        }

        Play::UpdateGameObject(obj_explosion);
        Play::DrawObjectRotated(obj_explosion);

        if (!Play::IsVisible(obj_explosion) || IsAnimationComplete(obj_explosion))
        {
            Play::DestroyGameObject(id_explosion);
        }
    }
}

// Notes::
/*
* Need to update leaderboard intake to take in the level and score
*/