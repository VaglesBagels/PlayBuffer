#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

#include "LeaderBoard.h"
#include "GameState.h"
#include "GameObjectType.h"
#include "Game.h"

void LeaderBoard::SaveScore()
{
    // Declarations
    std::vector<int> allScores;
    std::ifstream inputLeaderboardFile("leaderboard.txt");
    int savedScore = 0;
    int currentScore = gameState.score;

    if (!inputLeaderboardFile.is_open())
    {
        std::cout << "Error opening file" << std::endl;
        return;
    }

    while (inputLeaderboardFile >> savedScore)
    {
        allScores.push_back(savedScore);
    }

    inputLeaderboardFile.close();

    // Add current score to all saved scores in a vector in descending order
    allScores.push_back(currentScore);
    sort(allScores.begin(), allScores.end(), std::greater<int>());

    std::ofstream outputLeaderboardFile("leaderboard.txt");

    if (!outputLeaderboardFile.is_open())
    {
        std::cout << "Error opening file" << std::endl;
        return;
    }

    for (int score : allScores)
    {
        outputLeaderboardFile << score << std::endl;
    }
    outputLeaderboardFile.close();
}
