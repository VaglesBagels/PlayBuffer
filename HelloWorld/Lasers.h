#pragma once

class Lasers
{
public:
	Lasers();

	void HandleExplosiveShot(Play::GameObject& obj_laser, Play::GameObject& obj_tool, int id_tool, bool isExplosive);
	void HandleCoinCollision(Play::GameObject& obj_laser, Play::GameObject& obj_coin);
	void HandleDeconstruction(Play::GameObject& obj_laser, int id);

	static void Update();

private:
	bool hasCollided;
};

