#pragma once
#include "Play.h"
#include "MainGame.cpp"


class Tool 
{
public:
	// Constructor
	Tool(Play::Point2f position, int collisionRadius, const char* spriteName)
		: pos(position)
		, radius(collisionRadius)
		, sprite(spriteName)
	{
		id = Play::CreateGameObject(TYPE_TOOL, pos, radius, sprite);
	}

	// Destructor
	~Tool()
	{
		Play::DestroyGameObject(id);
	}

private:
	// Private variables and functions
	int id = 0;
	Play::Point2f pos;
	int radius;
	const char *sprite;
};