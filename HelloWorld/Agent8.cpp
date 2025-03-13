#define PLAY_USING_GAMEOBJECT_MANAGER

#include "Agent8.h"
#include "GameObjectType.h"
#include "GameState.h"
#include "Play.h"

void Agent8::SpawnAgent()
{
	Play::CreateGameObject(TYPE_AGENT8, { 115, 600 }, 50, "agent8");
}

void Agent8::HandlePlayerControls()
{
    Play::GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
    if (Play::KeyDown(Play::KEY_UP))
    {
        obj_agent8.velocity = { 0, 4 };
        Play::SetSprite(obj_agent8, "agent8_climb", 0.25f);
    }
    else if (Play::KeyDown(Play::KEY_DOWN))
    {
        obj_agent8.acceleration = { 0, -1 };
        Play::SetSprite(obj_agent8, "agent8_fall", 0);
    }
    else
    {
        if (obj_agent8.velocity.y < -5)
        {
            gameState.agentState = STATE_HALT;
            Play::SetSprite(obj_agent8, "agent8_halt", 0.333f);
            obj_agent8.acceleration = { 0,0 };
        }
        else
        {
            Play::SetSprite(obj_agent8, "agent8_hang", 0.02f);
            obj_agent8.velocity *= 0.5f;
            obj_agent8.acceleration = { 0, 0 };
        }
    }

    if (Play::KeyPressed(Play::KEY_SPACE))
    {
        Play::Vector2D firePos = obj_agent8.pos + Play::Vector2D(155, 75);
        int id = Play::CreateGameObject(TYPE_LASER, firePos, 30, "laser");
        Play::GetGameObject(id).velocity = { 32, 0 };
        // Play::PlayAudio("shoot");
    }
    Play::UpdateGameObject(obj_agent8);

    if (Play::IsLeavingDisplayArea(obj_agent8))
        obj_agent8.pos = obj_agent8.oldPos;

    Play::DrawLine({ obj_agent8.pos.x, 720 }, obj_agent8.pos, Play::cWhite);
    Play::DrawObjectRotated(obj_agent8);

    if (Play::KeyPressed(Play::KEY_S))
    {
        gameState.mode = MODE_SHOP;
    }
}

void Agent8::Update(int DISPLAY_HEIGHT)
{
    Play::GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

    switch (gameState.agentState)
    {
        case STATE_APPEAR:
            obj_agent8.velocity = { 0, -12 };
            obj_agent8.acceleration = { 0, -0.5f };
            Play::SetSprite(obj_agent8, "agent8_fall", 0);
            obj_agent8.rotation = 0;

            if (obj_agent8.pos.y <= DISPLAY_HEIGHT * 0.66f) { gameState.agentState = STATE_PLAY; }
            break;

        case STATE_HALT:
            obj_agent8.velocity *= 0.9f;

            if (Play::IsAnimationComplete(obj_agent8)) { gameState.agentState = STATE_PLAY; }
            break;

        case STATE_PLAY:
            HandlePlayerControls();
            break;

        case STATE_DEAD:
            obj_agent8.acceleration = { -0.3f , 0.5f };
            obj_agent8.rotation += 0.25f;

            if (Play::IsAnimationComplete(obj_agent8)) { gameState.mode = MODE_GAMEOVER; }
            break;
    }

    Play::UpdateGameObject(obj_agent8);

    if (Play::IsLeavingDisplayArea(obj_agent8) && gameState.agentState != STATE_DEAD)
    {
        obj_agent8.pos = obj_agent8.oldPos;
    }

    Play::DrawLine({ obj_agent8.pos.x, 720 }, obj_agent8.pos, Play::cWhite);
    Play::DrawObjectRotated(obj_agent8);
}
