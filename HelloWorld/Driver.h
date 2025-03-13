#pragma once

#include "Tool.h"
#include "Play.h"

class Driver : public Tool
{
public:
    Driver(Play::GameObject& obj_fan, float matchDuration);
    Play::GameObject& GetGameObject() { return *obj_tool; };

private:
    Play::GameObject *obj_tool;
};