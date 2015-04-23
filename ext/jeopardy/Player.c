//Player.c
#include <stdlib.h>
#include "Player.h"
#include "math.h"
#include "MinMax.h"

Player playerMake(int score, double buzzerRating, double confidenceRating, double knowledgeRating, double ddFJRating)
{
	Player player;
	player.score = score;
	player.buzzerRating = buzzerRating;
	player.confidenceRating = confidenceRating;
	player.knowledgeRating = knowledgeRating;
	player.ddFJRating = ddFJRating;
	return player;
}

Player averagePlayer()
{
    return playerMake(0, 1, 0, 0, 0);
}

double adjustedOdds(double odds, double rating)
{
	if (rating > 0.0)
	{
		return odds + (1.0-odds) * rating;
	}
	
	return odds - odds*rating;
}

double oddsPlayerAnsweredClue(Player *player, Clue *clue)
{
	double rawOdds = rawOddsOfAnsweringClue(clue);
	return adjustedOdds(rawOdds, player->knowledgeRating);
}

int playerAnsweredClue(Player *player, Clue *clue)
{
	return oddsPlayerAnsweredClue(player, clue) >= drand48();
}

double oddsPlayerAnsweredDailyDouble(Player *player, Clue *clue)
{
	double rawOdds = rawOddsOfAnsweringDailyDouble(clue);
	return adjustedOdds(rawOdds, player->ddFJRating);
}

int playerAnsweredDailyDouble(Player *player, Clue *clue)
{
	return oddsPlayerAnsweredDailyDouble(player, clue) >= drand48();
}

double oddsPlayerAnsweredFinalJeopardy(Player *player, Clue *clue)
{
	double rawOdds = clue->finalJeopardyOdds;
	return adjustedOdds(rawOdds, player->ddFJRating);
}

int playerAnsweredFinalJeopardy(Player *player, Clue *clue)
{
	return oddsPlayerAnsweredFinalJeopardy(player, clue) >= drand48();
}

double oddsPlayerAttemptedToRingIn(Player *player, Clue *clue)
{
	double rawOdds = rawOddsOfRingingIn(clue);
	return adjustedOdds(rawOdds, player->confidenceRating);
}

int playerAttemptedToRingIn(Player *player, Clue *clue)
{
	return oddsPlayerAttemptedToRingIn(player, clue) >= drand48();
}

int dailyDoubleWager(Player *player, Player *otherPlayers, int moneyLeft)
{
    int maxScore = -INFINITY;
    int minScore = INFINITY;
    for (int i = 0; i <3; i++)
    {
        Player *otherPlayer = &otherPlayers[i];
        if (otherPlayer != player && maxScore < otherPlayer->score)
        {
            maxScore = otherPlayer->score;
        }
        if (otherPlayer != player && minScore > otherPlayer->score)
        {
            minScore = otherPlayer->score;
        }
    }
    
	if (player->score > 2 * (moneyLeft + maxScore))
	{
		return 5;
	}
	else if (player->score * 2 < maxScore)
	{
		return player->score;
	}
	int minMaxWager = moneyLeft >= 36000 ? 1000 : 2000;
	
	return MAX(minMaxWager, MIN(2000 + (rand()%50)*100, player->score));
}

int *finalJeopardyWagers(Player *players)
{
	int maxScore = MAX(players[0].score, MAX(players[1].score, players[2].score));
	int minScore = MIN(players[0].score, MIN(players[1].score, players[2].score));
	int middleScore = NAN;
	for (int i = 0; i<3; i++)
	{
		if (players[i].score != maxScore && players[i].score != minScore)
		{
			middleScore = players[i].score;
		}
	}
	
	if (isnan(middleScore))
	{
		int sumScores = players[0].score + players[1].score + players[2].score;
		middleScore = maxScore*2 + minScore == sumScores ? maxScore : minScore;
	}
	
	int maxWager, middleWager, minWager;
	
	if (maxScore <= 0)
	{
		maxWager = 0;
	}
	else if (maxScore == middleScore)
	{
		maxWager = maxScore;
	}
	else if (maxScore > 2*middleScore)
	{
		maxWager = maxScore - 2*middleScore - 1;
	}
	else if (maxScore == 2*middleScore)
	{
		maxWager = 1;
	}
	else if (maxScore * 2 == middleScore * 3)
	{
		maxWager = maxScore - middleScore;
	}
	else
	{
		maxWager = 2*middleScore - maxScore + 1;
	}
	
	int maxMiss = maxScore - maxWager;
	
	if (middleScore <= 0)
	{
		middleWager = 0;
	}
	else if (maxMiss > 2*middleScore)
	{
		middleWager = MAX(0, 2*minScore - middleScore);
	}
	else if (maxMiss > middleScore || maxScore == middleScore || middleScore == minScore)
	{
		middleWager = middleScore;
	}
	else
	{
		int target = MAX(maxMiss, minScore*2);
		middleWager = MIN(target - middleScore + 1, middleScore);
	}
	
	int middleMiss = middleScore - middleWager;
	if (minScore <= 0)
	{
		minWager = 0;
	}
	else if (minScore == middleScore)
	{
		minWager = middleScore;
	}
	else if (minScore > middleMiss && minScore > maxMiss)
	{
		minWager = 0;
	}
	else
	{
		minWager = minScore;
	}
	
	static int wagers[3];
	for (int i = 0; i<3; i++)
	{
		wagers[i] = players[i].score == maxScore ? maxWager : players[i].score == middleScore ? middleWager : minWager;
	}
	return wagers;
}
