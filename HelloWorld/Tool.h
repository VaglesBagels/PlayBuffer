#pragma once

class Tool
{
public:
	Tool();
	virtual ~Tool() {}

	static void SpawnTools(Play::GameObject& obj_fan, bool isPowerUpActive, float matchDuration);
	void HandleCollisions(Play::GameObject& obj_tool, Play::GameObject& obj_agent8, bool isInvincible);
	void HandleDeconstruction(Play::GameObject& obj_tool, int id);
	void HandleDisplayArea(Play::GameObject& obj_tool);

	static void Update();

private:

	bool hasCollided;
};

