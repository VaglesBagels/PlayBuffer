#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

#include "Tool.h"
#include "GameState.h"
#include "GameObjectType.h"
#include "Spanner.h"
#include "Driver.h"

Driver::Driver(Play::GameObject& obj_fan, float matchDuration)
{
    int id = Play::CreateGameObject(TYPE_TOOL, obj_fan.pos, 50, "driver");
    obj_tool = &Play::GetGameObject(id);
    obj_tool->velocity = Play::Point2f(-8, Play::RandomRollRange(-1, 1) * 6);
    obj_tool->velocity.x -= (matchDuration * 0.1f);
}