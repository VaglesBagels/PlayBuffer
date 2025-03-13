#define PLAY_USING_GAMEOBJECT_MANAGER

#include "CoinStar.h"
#include "GameObjectType.h"
#include "GameState.h"
#include "Play.h"

Coin::Coin() : hasCollided(false)
{
}

void Coin::SpawnCoins(Play::GameObject& obj_fan, bool isPowerUpActive, float matchDuration)
{
    if (Play::RandomRoll(150) == 1 && !isPowerUpActive)
    {
        int id = Play::CreateGameObject(TYPE_COIN, obj_fan.pos, 40, "coin");
        Play::GameObject& obj_coin = Play::GetGameObject(id);
        obj_coin.velocity = { -3, 0 };
        obj_coin.velocity.x -= (matchDuration * 0.1f);
        obj_coin.rotSpeed = 0.1f;
    }
}

void Coin::UpdateScore(bool isPowerUpActive)
{
    if (!isPowerUpActive) {
        gameState.score += 500;
    }
    else {
        gameState.score += 750;
    }

    // Play::PlayAudio("collect");
}

void Coin::HandleCollision(Play::GameObject& obj_coin, Play::GameObject& obj_agent8, bool isPowerUpActive)
{
    if (Play::IsColliding(obj_coin, obj_agent8))
    {
        for (float rad{ 0.25f }; rad < 2.0f; rad += 0.5f)
        {
            int id = Play::CreateGameObject(TYPE_STAR, obj_agent8.pos, 0, "star");
            Play::GameObject& obj_star = Play::GetGameObject(id);

            obj_star.rotSpeed = 0.1f;
            obj_star.acceleration = { 0.0f, -0.5f };
            Play::SetGameObjectDirection(obj_star, 16, rad * Play::PLAY_PI);
        }

        hasCollided = true;
        UpdateScore(isPowerUpActive);
    }
}

void Coin::HandleDeconstruction(Play::GameObject& obj_coin, int id)
{
    if (!Play::IsVisible(obj_coin) || hasCollided)
    {
        Play::DestroyGameObject(id);
    }
}

void Coin::Update()
{
    Play::GameObject& obj_fan = Play::GetGameObjectByType(TYPE_FAN);
    Play::GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
    std::vector<int> vCoins = Play::CollectGameObjectIDsByType(TYPE_COIN);

    Coin::SpawnCoins(obj_fan, gameState.isPowerUpActive, gameState.matchDuration);

    std::vector<Coin> coins;

    for (int id_coin : vCoins)
    {
        // Found this, creates a new coin for each generate coin object
        coins.emplace_back();
    }

    for (int i = 0; i < vCoins.size(); i++)
    {
        Play::GameObject& obj_coin = Play::GetGameObject(vCoins[i]);

        coins[i].HandleCollision(obj_coin, obj_agent8, gameState.isPowerUpActive);
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

