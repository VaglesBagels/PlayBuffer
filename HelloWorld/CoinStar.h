#pragma once

#include "Play.h"


class Coin
{
public:
    Coin();

    static void SpawnCoins(Play::GameObject& obj_fan, bool isPowerUpActive, float matchDuration);
    void UpdateScore(bool isPowerUpActive);
    void HandleCollision(Play::GameObject& obj_coin, Play::GameObject& obj_agent8, bool isPowerUpActive);
    void HandleDeconstruction(Play::GameObject& obj_coin, int id);

private:
    bool hasCollided;
};

