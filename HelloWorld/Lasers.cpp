#define PLAY_USING_GAMEOBJECT_MANAGER

#include "GameObjectType.h"
#include "GameState.h"
#include "Lasers.h"
#include "Play.h"

Lasers::Lasers() : hasCollided(false)
{
}

void Lasers::HandleExplosiveShot(Play::GameObject& obj_laser, Play::GameObject& obj_tool, int id_tool, bool isExplosive)
{
    if (!isExplosive && Play::IsColliding(obj_laser, obj_tool))
    {
        hasCollided = true;
        obj_tool.type = TYPE_DESTROYED;
        gameState.score += 100;
    }
    else if (isExplosive && Play::IsColliding(obj_laser, obj_tool))
    {
        int id_explosion = Play::CreateGameObject(TYPE_EXPLOSIONS, obj_tool.pos, 30, "explosion");
        Play::GameObject& obj_explosion = Play::GetGameObject(id_explosion);
        Play::SetSprite(obj_explosion, "explosions", 0.1f);
        obj_explosion.velocity = { 0, 0 };
        obj_explosion.rotSpeed = 0.1f;

        hasCollided = true;
        Play::DestroyGameObject(id_tool);
        gameState.score += 100;
    }
}

void Lasers::HandleCoinCollision(Play::GameObject& obj_laser, Play::GameObject& obj_coin)
{
    if (Play::IsColliding(obj_laser, obj_coin))
    {
        hasCollided = true;
        obj_coin.type = TYPE_DESTROYED;
        // Play::PlayAudio("error");
        gameState.score -= 300;
    }
}

void Lasers::HandleDeconstruction(Play::GameObject& obj_laser, int id)
{
    if (!Play::IsVisible(obj_laser) || hasCollided)
    {
        Play::DestroyGameObject(id);
    }
}

void Lasers::Update()
{
    std::vector<int> vLasers = Play::CollectGameObjectIDsByType(TYPE_LASER);
    std::vector<int> vTools = Play::CollectGameObjectIDsByType(TYPE_TOOL);
    std::vector<int> vCoins = Play::CollectGameObjectIDsByType(TYPE_COIN);
    std::vector<int> vExplosions = Play::CollectGameObjectIDsByType(TYPE_EXPLOSIONS);

    std::vector<Lasers> lasers;

    // Like Coins, this is a vector of laser id with each new laser having its own instance
    // This allows the remaining functionality to work independently
    for (int id_laser : vLasers)
    {
        lasers.emplace_back();
    }

    for (int i = 0; i < vLasers.size(); i++)
    {
        Play::GameObject& obj_laser = Play::GetGameObject(vLasers[i]);
        bool hasCollided = false;

        for (int id_tool : vTools)
        {
            Play::GameObject& obj_tool = Play::GetGameObject(id_tool);
            lasers[i].HandleExplosiveShot(obj_laser, obj_tool, id_tool, gameState.isExplosive);
        }

        for (int id_coin : vCoins)
        {
            Play::GameObject& obj_coin = Play::GetGameObject(id_coin);
            lasers[i].HandleCoinCollision(obj_laser, obj_coin);
        }

        if (gameState.score < 0)
        {
            gameState.score = 0;
        }

        Play::UpdateGameObject(obj_laser);
        Play::DrawObject(obj_laser);

        lasers[i].HandleDeconstruction(obj_laser, vLasers[i]);
    }
}
