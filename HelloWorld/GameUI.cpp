#define PLAY_USING_GAMEOBJECT_MANAGER

#include "Game.h"
#include "GameObjectType.h"
#include "GameState.h"
#include "GameUI.h"
#include "LeaderBoard.h"
#include "Play.h"

GameUI::GameUI(int displayWidth, int displayHeight) : DISPLAY_WIDTH(displayWidth), DISPLAY_HEIGHT(displayHeight)
{
}

void GameUI::ShowSplashScreen()
{
    Play::DrawFontText("72px", "Welcome to Spydroid",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 80 }, Play::CENTRE);

    Play::DrawFontText("32px", "A game developed by Michael Lourens CGSI Student Assignment 2",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 200 }, Play::CENTRE);

    Play::DrawFontText("32px", "Game being developed using the PlayBuffer Framework for C++ developed by Sumo Digital Ltd.",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 250 }, Play::CENTRE);

    Play::DrawFontText("32px", "Press the \"M\" key to continue.....",
                       { DISPLAY_WIDTH / 2, 40 }, Play::CENTRE);

    if (Play::KeyPressed(Play::KEY_M) == true)
    {
        gameState.mode = MODE_MENU;
    }
}

void GameUI::ShowMainMenu(bool& isPowerUpActive, float& matchDuration)
{
    Play::DrawFontText("72px", "Welcome to Spydroid",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 80 }, Play::CENTRE);

    Play::DrawFontText("32px", "ARROW KEYS TO MOVE UP AND DOWN AND \"SPACE\" TO FIRE",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 170 }, Play::CENTRE);

    Play::DrawFontText("32px", "Press \"ENTER\" to start the game",
                       { 40, 400 }, Play::LEFT);

    Play::DrawFontText("32px", "Press \"L\" to view the leaderboard",
                       { 40, 300 }, Play::LEFT);

    Play::DrawFontText("32px", "Press \"ESC\" to quit the game",
                       { 40, 200 }, Play::LEFT);

    if (Play::KeyPressed(Play::KEY_ENTER))
    {
        Game::TriggerGameStart(isPowerUpActive, matchDuration);
    }

    if (Play::KeyPressed(Play::KEY_L) == true)
    {
        gameState.mode = MODE_LEADERBOARD;
    }
}

void GameUI::ShowGameOver(bool& isPowerUpActive, float& matchDuration)
{
    // Save the score
    if (!gameState.scoreSaved)
    {
        LeaderBoard::SaveScore();
        gameState.scoreSaved = true;
    }

    Play::DrawFontText("72px", "Game Over",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 80 }, Play::CENTRE);

    Play::DrawFontText("32px", "Final Score: " + std::to_string(gameState.score),
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 170 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"R\" restart",
                       { DISPLAY_WIDTH * 7 / 8, 40 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"L\" leaderboard",
                       { DISPLAY_WIDTH * 5 / 8, 40 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"M\" Main",

                       { DISPLAY_WIDTH * 3 / 8, 40 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"ESC\" quit game",

                       { DISPLAY_WIDTH * 1 / 8, 40 }, Play::CENTRE);

    if (Play::KeyPressed(Play::KEY_R) == true)
    {
        Game::ResetGameState();
        Game::TriggerGameStart(isPowerUpActive, matchDuration);
    }

    if (Play::KeyPressed(Play::KEY_L) == true)
    {
        gameState.mode = MODE_LEADERBOARD;
    }

    if (Play::KeyPressed(Play::KEY_M) == true)
    {
        gameState.mode = MODE_MENU;
    }
}

void GameUI::ShowLeaderboard(bool& isPowerUpActive, float& matchDuration)
{
    Play::DrawFontText("72px", "Leaderboard",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 80 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"M\" Main",

                       { DISPLAY_WIDTH * 1 / 8, 40 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"Enter\" Start Game",
                       { DISPLAY_WIDTH * 7 / 8, 40 }, Play::CENTRE);

    // Read Leaderboard scores into vector
    std::ifstream leaderboardFile("leaderboard.txt");
    std::vector<int> allScores;
    int currentScore = 0;
    int yCoord = DISPLAY_HEIGHT - 150;

    while (leaderboardFile >> currentScore)
    {
        allScores.push_back(currentScore);
    }

    leaderboardFile.close();

    // TODO??: Limit number of output values. Maybe only 10?
    // Had a data assertion error. Through debugging, it was looping through one extra time. Need to add the i <
    // Having more than 10 caused some UI overlap issues (change the loop from i < allscores.size() to a const value)
    for (int i = 0; i < allScores.size(); i++)
    {
        Play::DrawFontText("32px", std::to_string(i + 1) + ". " + std::to_string(allScores[i]),
                           { DISPLAY_WIDTH / 2, yCoord }, Play::CENTRE);
        yCoord -= 50;
    }

    if (Play::KeyPressed(Play::KEY_M) == true)
    {
        gameState.mode = MODE_MENU;
    }

    if (Play::KeyPressed(Play::KEY_ENTER) == true)
    {
        Game::ResetGameState();
        Game::TriggerGameStart(isPowerUpActive, matchDuration);
    }
}

void GameUI::ShowShopMenu(bool& isExplosive, bool& isInvincible)
{
    int explosiveLasersCost = 2000;
    int invincibilityCost = 5000;

    // Need to add shop purchases - score and changes to the code of passives

    Play::DrawFontText("72px", "Shop",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 7 / 8 }, Play::CENTRE);

    Play::DrawFontText("32px", "Current Score: " + std::to_string(gameState.score),
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 6 / 8 }, Play::CENTRE);

    Play::DrawFontText("32px", "1. Explosive Rounds: +" + std::to_string(explosiveLasersCost),
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 4 / 8 }, Play::CENTRE);

    Play::DrawFontText("32px", "2. Invincibility (temporary): " + std::to_string(invincibilityCost),
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 3 / 8 }, Play::CENTRE);

    Play::DrawFontText("32px", "Press \"c\" to continue",
                       { DISPLAY_WIDTH * 1 / 16, DISPLAY_HEIGHT * 1 / 16 }, Play::LEFT);

    if (Play::KeyPressed(Play::KEY_1) && !isExplosive && gameState.score > explosiveLasersCost)
    {
        isExplosive = true;

        gameState.score -= explosiveLasersCost;
    }

    if (Play::KeyPressed(Play::KEY_2) && !isInvincible && gameState.score > invincibilityCost)
    {
        isInvincible = true;

        gameState.score -= invincibilityCost;
    }

    if (isInvincible)
    {
        Play::DrawFontText("32px", "Already Purchased!",
                           { DISPLAY_WIDTH / 2 , DISPLAY_HEIGHT * 7 / 16 }, Play::CENTRE);
    }

    if (isExplosive)
    {
        Play::DrawFontText("32px", "Bought! Cyan Timer on game screen",
                           { DISPLAY_WIDTH / 2 , DISPLAY_HEIGHT * 3 / 16 }, Play::CENTRE);
    }

    if (Play::KeyPressed(Play::KEY_C))
    {
        gameState.mode = MODE_PLAYING;
    }
}

void GameUI::ShowLevelCompleteMenu()
{
    Play::DrawFontText("72px", "Level " + std::to_string(gameState.currentLevel - 1) + " complete!",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 7 / 8 }, Play::CENTRE);

    gameState.score + 5000;

    Play::DrawFontText("32px", "You have been awareded 5000 points: " + std::to_string(gameState.score),
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 6 / 8 }, Play::CENTRE);

    Play::DrawFontText("32px", "Press \"c\" to continue",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 4 / 8 }, Play::CENTRE);

    Play::DrawFontText("32px", "Press \"s\" for shop",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 3 / 8 }, Play::CENTRE);

    Play::DrawFontText("32px", "\"ESC\" quit game (WARNING: lose all progress)",
                       { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT * 2 / 8 }, Play::CENTRE);

    if (Play::KeyPressed(Play::KEY_C))
    {
        gameState.mode = MODE_PLAYING;
    }

    if (Play::KeyPressed(Play::KEY_S))
    {
        gameState.mode = MODE_SHOP;
    }
}
