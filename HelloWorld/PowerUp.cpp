#define PLAY_USING_GAMEOBJECT_MANAGER

#include "Game.h"
#include "GameObjectType.h"
#include "GameState.h"
#include "Play.h"
#include "PowerUp.h"

void PowerUp::Update(float elapsedTime)
{
    Play::GameObject& obj_fan = Play::GetGameObjectByType(TYPE_FAN);
    Play::GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

    std::vector<int> vPowerUps = Play::CollectGameObjectIDsByType(TYPE_POWERUP);

    bool hasCollided = false;

    if (vPowerUps.size() != 1 && !gameState.isPowerUpActive)
    {
        if (Play::RandomRoll(500) == 1)
        {
            int id_money = Play::CreateGameObject(TYPE_POWERUP, obj_fan.pos, 20, "money");
            Play::GameObject& obj_powerup = Play::GetGameObject(id_money);
            obj_powerup.velocity = { -6, 0 };
            obj_powerup.velocity.x -= (gameState.matchDuration * 0.1f);
            obj_powerup.rotSpeed = 0.1f;
        }
    }
    else
    {
        if (Play::RandomRoll(100) == 1 && gameState.isPowerUpActive)
        {
            int id_coin = Play::CreateGameObject(TYPE_COIN, obj_fan.pos, 20, "coin");
            Play::GameObject& obj_coin = Play::GetGameObject(id_coin);
            Play::SetSprite(obj_coin, "coins", 0.1f);
            obj_coin.velocity = { -10, 0 };
            obj_coin.rotSpeed = 0.1f;
        }
    }

    for (int id_money : vPowerUps)
    {
        Play::GameObject& obj_powerup = Play::GetGameObject(id_money);

        if (Play::IsColliding(obj_agent8, obj_powerup))
        {
            for (float rad{ 0.125f }; rad < 2.0f; rad += 0.125f)
            {
                int id_cash = Play::CreateGameObject(TYPE_CASH, obj_agent8.pos, 0, "cash");
                Play::GameObject& obj_cash = Play::GetGameObject(id_cash);

                obj_cash.rotSpeed = 0.1f;
                obj_cash.acceleration = { 0.0f, 0.0f };
                Play::SetGameObjectDirection(obj_cash, 16, rad * Play::PLAY_PI);
            }

            hasCollided = true;
            // Play::PlayAudio("collect");

            gameState.isPowerUpActive = true;
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
        Play::GameObject& obj_cash = Play::GetGameObject(id_cash);

        Play::UpdateGameObject(obj_cash);
        Play::DrawObjectRotated(obj_cash);

        if (!Play::IsVisible(obj_cash))
        {
            Play::DestroyGameObject(id_cash);
        }
    }

    if (gameState.isPowerUpActive)
    {
        gameState.powerUpTimer += elapsedTime;

        Game::DrawProgressBar(gameState.powerUpTimer, gameState.powerUpDuration, { 50, 50 }, 25, 200, Play::cMagenta);

        if (gameState.powerUpTimer >= gameState.powerUpDuration)
        {
            gameState.isPowerUpActive = false;
            gameState.powerUpTimer = 0.0f;
        }
    }
}
