#define PLAY_USING_GAMEOBJECT_MANAGER

#include "Game.h"
#include "GameObjectType.h"
#include "GameState.h"
#include "Play.h"

void Game::TriggerGameStart(bool& isPowerUpActive, float& matchDuration)
{
    gameState.mode = MODE_PLAYING;
    gameState.agentState = STATE_APPEAR;
    gameState.score = 0;
    gameState.scoreSaved = false;
    isPowerUpActive = false;
    matchDuration = 0;
}

void Game::ResetGameState()
{
    // Reset Agent
    Play::GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
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

void Game::DrawProgressBar(float barDuration, float totalDuration, Play::Point2D bottomLeftCorner, float barThickness, float barLength, Play::Colour colour)
{
    // Bar Outline -1 to the starting point due to pixel padding
    Play::DrawRect({ bottomLeftCorner.x - 1, bottomLeftCorner.y - 1 }, { bottomLeftCorner.x + barLength, bottomLeftCorner.y + barThickness }, Play::cGrey);

    // This section for bar to countdown -- potentially add a bool value to see if the bar needs to be counted up or down
    float remainingTime = totalDuration - barDuration;

    // Had to add .0f to 1000 and 60 to ensure that it incremented correctly. When it was not there, causeed the bar to require ~65sec to be 100%
    float progress = remainingTime * (barLength / totalDuration);

    if (progress >= 0)
    {
        Play::DrawRect(bottomLeftCorner, { bottomLeftCorner.x + progress, bottomLeftCorner.y + barThickness }, colour, true);
    }
}

void Game::UpdatePassiveUpgrades(float elapsedTime, int DISPLAY_HEIGHT)
{
    if (gameState.mode != MODE_SHOP && gameState.isInvincible)
    {
        gameState.invincibleTimer += elapsedTime;

        Game::DrawProgressBar(gameState.invincibleTimer, gameState.invincibleDuration, { 50, DISPLAY_HEIGHT * 5 / 8 }, 25, 200, Play::cCyan);

        if (gameState.invincibleTimer > gameState.invincibleDuration)
        {
            gameState.invincibleTimer = 0.0f;
            gameState.isInvincible = false;
        }
    }
}

void Game::UpdateDestroyed()
{
    std::vector<int> vDead = Play::CollectGameObjectIDsByType(TYPE_DESTROYED);

    for (int id_dead : vDead)
    {
        Play::GameObject& obj_dead = Play::GetGameObject(id_dead);
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
