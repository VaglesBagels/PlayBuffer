#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

#include "Lasers.h"
#include "GameState.h"
#include "GameObjectType.h"

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
