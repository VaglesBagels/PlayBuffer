#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

#include "Explosion.h"
#include "Game.h"
#include "GameState.h"
#include "GameObjectType.h"

void Explosion::Update()
{
    std::vector<int> vExplosions = Play::CollectGameObjectIDsByType(TYPE_EXPLOSIONS);
    std::vector<int> vTools = Play::CollectGameObjectIDsByType(TYPE_TOOL);

    for (int id_explosion : vExplosions)
    {
        Play::GameObject& obj_explosion = Play::GetGameObject(id_explosion);
        bool hasCollided = false;

        for (int id_tool : vTools)
        {
            Play::GameObject& obj_tool = Play::GetGameObject(id_tool);

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
