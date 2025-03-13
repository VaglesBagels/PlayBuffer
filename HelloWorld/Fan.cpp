#define PLAY_USING_GAMEOBJECT_MANAGER

#include "Fan.h"
#include "GameObjectType.h"
#include "GameState.h"
#include "Play.h"

void Fan::SpawnFan()
{
    int id_fan = Play::CreateGameObject(TYPE_FAN, { 1140, 503 }, 0, "fan");
    Play::GetGameObject(id_fan).velocity = { 0, -3 };
    Play::GetGameObject(id_fan).animSpeed = 1.0f;
}

void Fan::Update()
{
    Play::GameObject& obj_fan = Play::GetGameObjectByType(TYPE_FAN);

    Play::UpdateGameObject(obj_fan);

    if (Play::IsLeavingDisplayArea(obj_fan))
    {
        obj_fan.pos = obj_fan.oldPos;
        obj_fan.velocity.y *= -1;
    }

    Play::DrawObject(obj_fan);
}
