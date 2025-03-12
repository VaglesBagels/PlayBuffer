#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Tool.h"
#include "GameState.h"
#include "GameObjectType.h"
#include "Driver.h"
#include "Spanner.h"
#include <memory>

// Need to use unique_pointer - enforces one pointer for each tool especially from derived classes spanner and driver
std::vector<std::unique_ptr<Tool>> toolInstances;

Tool::Tool() : hasCollided(false)
{
}

void Tool::SpawnTools(Play::GameObject& obj_fan, bool isPowerUpActive, float matchDuration)
{
    if (Play::RandomRoll(50) == 1 && !isPowerUpActive)
    {
        toolInstances.push_back(std::make_unique<Driver>(obj_fan, matchDuration));
        if (Play::RandomRoll(2) == 1)
        {
            Driver* driverPtr = dynamic_cast<Driver*>(toolInstances.back().get());
            if (driverPtr)
            {
                toolInstances.back() = std::make_unique<Spanner>(driverPtr->GetGameObject(), matchDuration);
            }
        }
        // Play::PlayAudio("tool");
    }
}

void Tool::HandleCollisions(Play::GameObject& obj_tool, Play::GameObject& obj_agent8, bool isInvincible)
{
    if (Play::IsColliding(obj_tool, obj_agent8) && gameState.agentState != STATE_DEAD)
    {
        if (isInvincible)
        {
            gameState.score += 100;
            hasCollided = true;
        }
        else
        {
            Play::StopAudio("music");
            Play::PlayAudio("die");
            gameState.agentState = STATE_DEAD;
        }
    }
}

void Tool::HandleDeconstruction(Play::GameObject& obj_tool, int id)
{
    if (!Play::IsVisible(obj_tool) || hasCollided)
    {
        Play::DestroyGameObject(id);
    }
}

void Tool::HandleDisplayArea(Play::GameObject& obj_tool)
{
    if (Play::IsLeavingDisplayArea(obj_tool, Play::VERTICAL))
    {
        obj_tool.pos = obj_tool.oldPos;
        obj_tool.velocity.y *= -1;
    }
}

void Tool::Update()
{
    Play::GameObject& obj_fan = Play::GetGameObjectByType(TYPE_FAN);
    Play::GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

    std::vector<int> vTools = Play::CollectGameObjectIDsByType(TYPE_TOOL);

    SpawnTools(obj_fan, gameState.isPowerUpActive, gameState.matchDuration);

    // This had to be a for loop as in Tool.cpp toolinstances is a vector itself
    for (size_t i = 0; i < vTools.size(); ++i)
    {
        Play::GameObject& obj_tool = Play::GetGameObject(vTools[i]);

        toolInstances[i]->HandleCollisions(obj_tool, obj_agent8, gameState.isInvincible);

        Play::UpdateGameObject(obj_tool);

        toolInstances[i]->HandleDisplayArea(obj_tool);

        Play::DrawObjectRotated(obj_tool);

        toolInstances[i]->HandleDeconstruction(obj_tool, vTools[i]);
    }
}

