#define PLAY_USING_GAMEOBJECT_MANAGER

#include "GameObjectType.h"
#include "GameState.h"
#include "Play.h"
#include "Spanner.h"
#include "Tool.h"

Spanner::Spanner(Play::GameObject& obj_tool, float matchDuration)
{
    Play::SetSprite(obj_tool, "spanner", 0);
    obj_tool.radius = 100;
    obj_tool.velocity.x = -4;
    obj_tool.velocity.x -= (matchDuration * 0.1f);
    obj_tool.rotSpeed = 0.1f;
}