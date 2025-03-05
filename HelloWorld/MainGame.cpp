#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

int DISPLAY_WIDTH = 1280;
int DISPLAY_HEIGHT = 720;
int DISPLAY_SCALE = 1;
const int leaderboardSize = 10;
float powerUpTimer = 0.0f;
float powerUpDuration = 10.0f;
bool isPowerUpActive = false;
float matchDuration = 0;

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
};

// Function Declarations
void HandlePlayerControls();
void UpdateFan(float& matchDuration);
void UpdateTools();
void UpdateCoinsAndStars();
void UpdateLasers();
void UpdateDestroyed();
void UpdateAgent8();
void ShowSplashScreen();
void ShowMainMenu();
void ShowGameOver();
void TriggerGameStart();
void SaveScore(GameState gameState);
void ShowLeaderboard();
void ResetGameState();
void UpdatePowerUp(float elapsedTime);
void ShowShopMenu();
void UpdateLevel(float elapsedTime);
void DrawProgressBar(float barDuration, Point2D bottomLeftCorner, float barThickness, float barLength);
void ShowLevelCompleteMenu();

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

// Called by the PlayBuffer once every frame (60 times a second!) 
bool MainGameUpdate(float elapsedTime)
{
    Play::DrawBackground();

    switch (gameState.mode)
    {
        case MODE_SPLASH:
            ShowSplashScreen();

            break;

        case MODE_MENU:
            ShowMainMenu();

            break;

        case MODE_PLAYING:
            matchDuration += elapsedTime;

            UpdateAgent8();
            UpdateFan(matchDuration);
            UpdateTools();
            UpdateCoinsAndStars();
            UpdateLasers();
            UpdateDestroyed();
            Play::DrawFontText("72px", "SCORE: " + to_string(gameState.score),
                               { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 7/8 }, Play::CENTRE);

            UpdatePowerUp(elapsedTime);
            UpdateLevel(elapsedTime);
            break;

        case MODE_GAMEOVER:
            ShowGameOver();

            break;

        case MODE_LEADERBOARD:
            ShowLeaderboard();

            break;

        case MODE_SHOP:
            ShowShopMenu();

            break;

        case MODE_LEVELCOMPLETE:
            ShowLevelCompleteMenu();

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

void UpdateFan(float& matchDuration)
{
    GameObject& obj_fan = Play::GetGameObjectByType(TYPE_FAN);
    
    if (Play::RandomRoll(50) == 1 && !isPowerUpActive)
    {
        int id = Play::CreateGameObject(TYPE_TOOL, obj_fan.pos, 50, "driver");
        GameObject& obj_tool = Play::GetGameObject(id);
        obj_tool.velocity = Point2f(-8, Play::RandomRollRange(-1, 1) * 6);
        obj_tool.velocity.x -= (matchDuration * 0.2);

        if (Play::RandomRoll(2) == 1)
        {
            Play::SetSprite(obj_tool, "spanner", 0);
            obj_tool.radius = 100;
            obj_tool.velocity.x = -4;
            obj_tool.velocity.x -= (matchDuration * 0.2);
            obj_tool.rotSpeed = 0.1f;
        }
        // Play::PlayAudio("tool");
    }

    if (Play::RandomRoll(150) == 1 && !isPowerUpActive)
    {
        int id = Play::CreateGameObject(TYPE_COIN, obj_fan.pos, 40, "coin");
        GameObject& obj_coin = Play::GetGameObject(id);
        obj_coin.velocity = { -3, 0 };
        obj_coin.velocity.x -= (matchDuration * 0.2);
        obj_coin.rotSpeed = 0.1f;
    }

    Play::UpdateGameObject(obj_fan);

    if (Play::IsLeavingDisplayArea(obj_fan))
    {
        obj_fan.pos = obj_fan.oldPos;
        obj_fan.velocity.y *= -1;
    }
    Play::DrawObject(obj_fan);
}

void UpdateTools()
{
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
    std::vector<int> vTools = Play::CollectGameObjectIDsByType(TYPE_TOOL);

    for (int id : vTools)
    {
        GameObject& obj_tool = Play::GetGameObject(id);

        /*if (gameState.agentState != STATE_DEAD && Play::IsColliding(obj_tool, obj_agent8))
        {
            Play::StopAudio("music");
            Play::PlayAudio("die");
            gameState.agentState = STATE_DEAD;
        }*/
        Play::UpdateGameObject(obj_tool);

        if (Play::IsLeavingDisplayArea(obj_tool, Play::VERTICAL))
        {
            obj_tool.pos = obj_tool.oldPos;
            obj_tool.velocity.y *= -1;
        }
        Play::DrawObjectRotated(obj_tool);

        if (!Play::IsVisible(obj_tool))
        {
            Play::DestroyGameObject(id);
        }
    }


}

void UpdateCoinsAndStars()
{
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
    std::vector<int> vCoins = Play::CollectGameObjectIDsByType(TYPE_COIN);

    for (int id_coin : vCoins)
    {
        GameObject& obj_coin = Play::GetGameObject(id_coin);
        bool hasCollided = false;

        if (Play::IsColliding(obj_coin, obj_agent8))
        {
            for (float rad{ 0.25f }; rad < 2.0f; rad += 0.5f)
            {
                int id = Play::CreateGameObject(TYPE_STAR, obj_agent8.pos, 0, "star");
                GameObject& obj_star = Play::GetGameObject(id);

                obj_star.rotSpeed = 0.1f;
                obj_star.acceleration = { 0.0f, -0.5f };
                Play::SetGameObjectDirection(obj_star, 16, rad * PLAY_PI);
            }

            hasCollided = true;

            if (!isPowerUpActive)
            {
                gameState.score += 500;
            }
            else
            {
                gameState.score += 750;
            }
            
            // Play::PlayAudio("collect");
        }

        Play::UpdateGameObject(obj_coin);
        Play::DrawObjectRotated(obj_coin);

        if (!Play::IsVisible(obj_coin) || hasCollided)
        {
            Play::DestroyGameObject(id_coin);
        }
    }

    std::vector<int> vStars = Play::CollectGameObjectIDsByType(TYPE_STAR);

    for (int id_star : vStars)
    {
        GameObject& obj_star = Play::GetGameObject(id_star);

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

    for (int id_laser : vLasers)
    {
        GameObject& obj_laser = Play::GetGameObject(id_laser);
        bool hasCollided = false;

        for (int id_tool : vTools)
        {
            GameObject& obj_tool = Play::GetGameObject(id_tool);

            if (Play::IsColliding(obj_laser, obj_tool))
            {
                hasCollided = true;
                obj_tool.type = TYPE_DESTROYED;
                gameState.score += 100;
            }
        }

        for (int id_coin : vCoins)
        {
            GameObject& obj_coin = Play::GetGameObject(id_coin);

            if (Play::IsColliding(obj_laser, obj_coin))
            {
                hasCollided = true;
                obj_coin.type = TYPE_DESTROYED;
                // Play::PlayAudio("error");
                gameState.score -= 300;
            }
        }

        if (gameState.score < 0)
        {
            gameState.score = 0;
        }

        Play::UpdateGameObject(obj_laser);
        Play::DrawObject(obj_laser);

        if (!Play::IsVisible(obj_laser) || hasCollided)
        {
            Play::DestroyGameObject(id_laser);
        }
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

void ShowSplashScreen()
{
    Play::DrawFontText("72px", "Welcome to Spydroid",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 80 }, Play::CENTRE);

    Play::DrawFontText("32px", "A game developed by Michael Lourens CGSI Student Assignment 2",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 200 }, Play::CENTRE);

    Play::DrawFontText("32px", "Game being developed using the PlayBuffer Framework for C++ developed by Sumo Digital Ltd.",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 250 }, Play::CENTRE);

    Play::DrawFontText("32px", "Press the \"M\" key to continue.....",
                       { DISPLAY_WIDTH / 2, 40 }, Play::CENTRE);

    if (Play::KeyPressed(Play::KEY_M) == true)
    {
        gameState.mode = MODE_MENU;
    }
}

void ShowMainMenu()
{
    Play::DrawFontText("72px", "Welcome to Spydroid",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 80 }, Play::CENTRE);

    Play::DrawFontText("32px", "ARROW KEYS TO MOVE UP AND DOWN AND \"SPACE\" TO FIRE",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 170 }, Play::CENTRE);

    Play::DrawFontText("32px", "Press \"ENTER\" to start the game",
                       { 40, 400 }, Play::LEFT);

    Play::DrawFontText("32px", "Press \"L\" to view the leaderboard",
                       { 40, 300 }, Play::LEFT);

    Play::DrawFontText("32px", "Press \"ESC\" to quit the game",
                       { 40, 200 }, Play::LEFT);

    if (Play::KeyPressed(Play::KEY_ENTER)) 
    { 
        TriggerGameStart();
    }

    if (Play::KeyPressed(Play::KEY_L) == true)
    {
        gameState.mode = MODE_LEADERBOARD;
    }
}

void ShowGameOver()
{
    // Save the score
    if (!gameState.scoreSaved)
    {
        SaveScore(gameState);
        gameState.scoreSaved = true;
    }

    Play::DrawFontText("72px", "Game Over",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 80 }, Play::CENTRE);

    Play::DrawFontText("32px", "Final Score: " + to_string(gameState.score),
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 170 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"R\" restart",
                       { DISPLAY_WIDTH * 7/8, 40 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"L\" leaderboard",
                       { DISPLAY_WIDTH * 5/8, 40 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"M\" Main",

                       { DISPLAY_WIDTH * 3/8, 40 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"ESC\" quit game",

                       { DISPLAY_WIDTH * 1/8, 40 }, Play::CENTRE);

    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

    if (Play::KeyPressed(Play::KEY_R) == true)
    {
        ResetGameState();
        TriggerGameStart();
    }

    if (Play::KeyPressed(Play::KEY_L) == true)
    {
        gameState.mode = MODE_LEADERBOARD;
    }

    if (Play::KeyPressed(Play::KEY_M) == true)
    {
        gameState.mode = MODE_MENU;
    }
}

void TriggerGameStart()
{
    gameState.mode = MODE_PLAYING;
    gameState.agentState = STATE_APPEAR;
    gameState.score = 0;
    gameState.scoreSaved = false;
    isPowerUpActive = false;
}

void SaveScore(GameState gameState)
{
    // Declarations
    vector<int> allScores;
    ifstream inputLeaderboardFile("leaderboard.txt");
    int savedScore = 0;
    int currentScore = gameState.score;

    if (!inputLeaderboardFile.is_open())
    {
        cout << "Error opening file" << endl;
        return;
    }

    while (inputLeaderboardFile >> savedScore)
    {
        allScores.push_back(savedScore);
    }

    inputLeaderboardFile.close();

    // Add current score to all saved scores in a vector in descending order
    allScores.push_back(currentScore);
    sort(allScores.begin(), allScores.end(), greater<int>());

    ofstream outputLeaderboardFile("leaderboard.txt");

    if (!outputLeaderboardFile.is_open())
    {
        cout << "Error opening file" << endl;
        return;
    }

    for (int score : allScores)
    {
        outputLeaderboardFile << score << endl;
    }
    outputLeaderboardFile.close();
}

void ShowLeaderboard()
{
    Play::DrawFontText("72px", "Leaderboard",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 80 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"M\" Main",

                       { DISPLAY_WIDTH * 1 / 8, 40 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"Enter\" Start Game",
                       { DISPLAY_WIDTH * 7 / 8, 40 }, Play::CENTRE);

    // Read Leaderboard scores into vector
    ifstream leaderboardFile("leaderboard.txt");
    vector<int> allScores;
    int currentScore = 0;
    int yCoord = DISPLAY_HEIGHT - 150;

    while (leaderboardFile >> currentScore)
    {
        allScores.push_back(currentScore);
    }

    leaderboardFile.close();

    // TODO??: Limit number of output values. Maybe only 10?
    // Had a data assertion error. Through debugging, it was looping through one extra time. Need to add the i <
    // Having more than 10 caused some UI overlap issues (change the loop from i < allscores.size() to a const value)
    for (int i = 0; i < leaderboardSize; i++)
    {
        Play::DrawFontText("32px", to_string(i + 1) + ". " + to_string(allScores[i]),
                           { DISPLAY_WIDTH / 2, yCoord }, Play::CENTRE);
        yCoord -= 50;
    }

    if (Play::KeyPressed(Play::KEY_M) == true)
    {
        gameState.mode = MODE_MENU;
    }

    if (Play::KeyPressed(Play::KEY_ENTER) == true)
    {
        ResetGameState();
        TriggerGameStart();
    }
}

// Was originally in ShowGameOver(), since wanted same functionality in ShowLeaderboard(), function was created to reduce code repetition
void ResetGameState()
{
    // Reset Agent
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
    obj_agent8.pos = { 115, 600 };
    obj_agent8.velocity = { 0, 0 };
    obj_agent8.frame = 0;

    // Destroy tools, coins and lasers
    std::vector<int> vObjectsDestroy;

    std::vector<int> vTools = Play::CollectGameObjectIDsByType(TYPE_TOOL);
    std::vector<int> vCoins = Play::CollectGameObjectIDsByType(TYPE_COIN);
    std::vector<int> vLasers = Play::CollectGameObjectIDsByType(TYPE_LASER);

    vObjectsDestroy.insert(vObjectsDestroy.end(), vTools.begin(), vTools.end());
    vObjectsDestroy.insert(vObjectsDestroy.end(), vCoins.begin(), vCoins.end());
    vObjectsDestroy.insert(vObjectsDestroy.end(), vLasers.begin(), vLasers.end());

    for (int id : vObjectsDestroy)
    {
        Play::DestroyGameObject(id);
    }
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
            obj_powerup.velocity.x -= (matchDuration * 0.2);
            obj_powerup.rotSpeed = 0.1f;
        }
    }
    else
    {
        if (Play::RandomRoll(20) == 1 && isPowerUpActive)
        {
            int id_coin = Play::CreateGameObject(TYPE_COIN, obj_fan.pos, 20, "coin");
            GameObject& obj_coin = Play::GetGameObject(id_coin);
            Play::SetSprite(obj_coin, "coins", 0.1);
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

        DrawProgressBar(powerUpTimer, { 50, 50 }, 25, 200);

        // Play::DrawDebugText({ 50, 50 }, to_string(powerUpTimer).c_str());

        if (powerUpTimer >= powerUpDuration)
        {
            isPowerUpActive = false;
            powerUpTimer = 0.0f;
        }
    }
}

void ShowShopMenu()
{
    int shootingSpeedCost = 1000;
    int punchThroughCost = 1000;
    int invincibilityCost = 5000;

    Play::DrawFontText("72px", "Shop",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 7/8 }, Play::CENTRE);

    Play::DrawFontText("32px", "Current Score: " + to_string(gameState.score),
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 6/8 }, Play::CENTRE);

    Play::DrawFontText("32px", "1. Shooting Speed: +" + to_string(shootingSpeedCost),
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 4/8 }, Play::CENTRE);

    Play::DrawFontText("32px", "2. Punch Through: +" + to_string(punchThroughCost),
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 3/8 }, Play::CENTRE);

    Play::DrawFontText("32px", "3. Invincibility (temporary): " + to_string(invincibilityCost),
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 2/8 }, Play::CENTRE);

    Play::DrawFontText("32px", "Press \"c\" to continue",
                       { DISPLAY_WIDTH * 1/16, DISPLAY_HEIGHT * 1/16 }, Play::LEFT);

    if (Play::KeyPressed(Play::KEY_C))
    {
        gameState.mode = MODE_PLAYING;
    }
}

void DrawProgressBar(float barDuration, Point2D bottomLeftCorner, float barThickness, float barLength)
{
    // Bar Outline -1 to the starting point due to pixel padding
    Play::DrawRect({ bottomLeftCorner.x - 1, bottomLeftCorner.y - 1 }, { bottomLeftCorner.x + barLength, bottomLeftCorner.y + barThickness }, cGrey);

    // This section for bar to countdown -- potentially add a bool value to see if the bar needs to be counted up or down
    float remainingTime = powerUpDuration - barDuration;

    // Had to add .0f to 1000 and 60 to ensure that it incremented correctly. When it was not there, causeed the bar to require ~65sec to be 100%
    float progress = remainingTime * (barLength / powerUpDuration);

    if (progress >= 0)
    {
        Play::DrawRect(bottomLeftCorner, { bottomLeftCorner.x + progress, bottomLeftCorner.y + barThickness }, cMagenta, true);
    }
}

void UpdateLevel(float elapsedTime)
{
    float levelDuration = 60.0f;
    float percentLevelComplete = 0.0f;
    
    gameState.progress += elapsedTime;

    if (gameState.progress > levelDuration)
    {
        // Level 1 complete
        gameState.mode = MODE_LEVELCOMPLETE;
        gameState.currentLevel++;
        gameState.progress = 0.0f;
    }
    else
    {
        percentLevelComplete = gameState.progress / levelDuration * 100;
        stringstream percentFormat;
        percentFormat << fixed << setprecision(2) << percentLevelComplete;
        Play::DrawFontText("32px", "Level " + to_string(gameState.currentLevel) + ": " + percentFormat.str() + "%",
                           { DISPLAY_WIDTH * 2/16 , DISPLAY_HEIGHT * 7 / 8 }, Play::CENTRE);
    }
}

void ShowLevelCompleteMenu()
{
    Play::DrawFontText("72px", "Level " + to_string(gameState.currentLevel - 1) + " complete!",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 7 / 8 }, Play::CENTRE);

    gameState.score + 5000;

    Play::DrawFontText("32px", "You have been awareded 5000 points: " + to_string(gameState.score),
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 6 / 8 }, Play::CENTRE);

    Play::DrawFontText("32px", "Press \"c\" to continue",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 4 / 8 }, Play::CENTRE);

    Play::DrawFontText("32px", "Press \"s\" for shop",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 3 / 8 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"ESC\" quit game (WARNING: lose all progress)",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 2 / 8 }, Play::CENTRE);

    if (Play::KeyPressed(Play::KEY_C))
    {
        gameState.mode = MODE_PLAYING;
    }

    if (Play::KeyPressed(Play::KEY_S))
    {
        gameState.mode = MODE_SHOP;
    }
}

// Notes::
/*
* Potential Added Mechanics:
*   - New Powerups:
*       - Rapid Fire
*       - Laser Fire
*       - Invincibility
*       - Double Points
*       - Money Bag: all coins -- Have implemented this
* 
*   - A boss type tool -- Might add a boss bomb, have a limited time to kill -- maybe create a roguelike screen after x score to add more damage
*   - Levels
*   - 2D movement
*   - Different orientation
*/

// This function causes the progress bar to go up to 100% starting from match duration
//void DrawProgressBar(float& matchDuration, Point2D bottomLeftCorner, float barThickness, float barLength, bool showPercentage)
//{
//    // Bar Outline -1 to the starting point due to pixel padding
//    Play::DrawRect({ bottomLeftCorner.x - 1, bottomLeftCorner.y - 1 }, { bottomLeftCorner.x + barLength, bottomLeftCorner.y + barThickness }, cGrey);
//
//    // Had to add .0f to 1000 and 60 to ensure that it incremented correctly. When it was not there, causeed the bar to require ~65sec to be 100%
//    float progress = matchDuration * (barLength / 60.0f);
//
//    if (progress <= barLength && showPercentage)
//    {
//        Play::DrawRect(bottomLeftCorner, { bottomLeftCorner.x + progress, bottomLeftCorner.y + barThickness }, cBlue, true);
//
//        float percentage = progress / barLength * 100;
//
//        stringstream percentFormat;
//        percentFormat << fixed << setprecision(2) << percentage;
//
//        Play::DrawFontText("32px", percentFormat.str() + "%",
//                           { barLength / 2, DISPLAY_HEIGHT * 7 / 8 }, Play::CENTRE);
//    }
//    else
//    {
//        Play::DrawRect(bottomLeftCorner, { bottomLeftCorner.x + barLength, bottomLeftCorner.y + barThickness }, cBlue, true);
//
//        if (showPercentage)
//        {
//            Play::DrawFontText("32px", "100%",
//                               { barLength / 2, DISPLAY_HEIGHT * 7 / 8 }, Play::CENTRE);
//        }
//    }
//}