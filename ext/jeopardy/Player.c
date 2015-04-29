//Player.c
#include <stdlib.h>
#include "math.h"
#include "MinMax.h"
#include "Player.h"

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

ResponseType playerAnsweredClue(Player *player, Clue *clue)
{
	return oddsPlayerAnsweredClue(player, clue) >= drand() ? CORRECT_ANSWER : INCORRECT_ANSWER;
}

double oddsPlayerAnsweredDailyDouble(Player *player, Clue *clue)
{
	double rawOdds = rawOddsOfAnsweringDailyDouble(clue);
	return adjustedOdds(rawOdds, player->ddFJRating);
}

ResponseType playerAnsweredDailyDouble(Player *player, Clue *clue)
{
	return oddsPlayerAnsweredDailyDouble(player, clue) >= drand() ? CORRECT_ANSWER : INCORRECT_ANSWER;
}

double oddsPlayerAnsweredFinalJeopardy(Player *player, Clue *clue)
{
	double rawOdds = clue->finalJeopardyOdds;
	return adjustedOdds(rawOdds, player->ddFJRating);
}

ResponseType playerAnsweredFinalJeopardy(Player *player, Clue *clue)
{
	return oddsPlayerAnsweredFinalJeopardy(player, clue) >= drand() ? CORRECT_ANSWER : INCORRECT_ANSWER;
}

double oddsPlayerAttemptedToRingIn(Player *player, Clue *clue)
{
	double rawOdds = rawOddsOfRingingIn(clue);
	return adjustedOdds(rawOdds, player->confidenceRating);
}

int playerAttemptedToRingIn(Player *player, Clue *clue)
{
	return oddsPlayerAttemptedToRingIn(player, clue) >= drand();
}

void otherPlayersMinMaxScores(Player *players, int playerIndex, int *minScore, int *maxScore)
{
	*maxScore = -INFINITY;
	*minScore = INFINITY;
	for (int i = 0; i <3; i++)
	{
		if (i != playerIndex)
		{
			*maxScore = MAX(*maxScore, players[i].score);
			*minScore = MIN(*minScore, players[i].score);
		}
	}
}

int dailyDoubleWager(Player *players, int playerInControlIndex, int moneyLeft)
{
	int minScore, maxScore;
	otherPlayersMinMaxScores(players, playerInControlIndex, &minScore, &maxScore);
	
	Player player = players[playerInControlIndex];
	
	/* Wager the minimum if the game is already a lock even if the player in second gets every remaining question */
	if ((moneyLeft + maxScore)*2 < player.score && player.score >= 0)
	{
		return 5;
	}
	
	/* Wager everything in the first round */
	if (moneyLeft >= 36000)
	{	
		return MAX(1000, player.score);
	}
	
	
	/* Otherwise, return a random value between 2k and 6k, since that's what your average contestant will do anyways, regardless of the score */
	return MAX(2000, MIN(2000 + rand()%20*200, players[playerInControlIndex].score));
}

int finalJeopardyWager(Player *players, int playerIndex)
{
	int minScore, maxScore;
	otherPlayersMinMaxScores(players, playerIndex, &minScore, &maxScore);
	
	Player player = players[playerIndex];
	
	/* Bet nothing if we have no money to wager */
	if (player.score <= 0)
	{
		return 0;
	}
	
	/* Bet everything but a dollar if we are the only ones playing final jeopardy */
	if (maxScore <=0)
	{
		return player.score-1;
	}
	
	/* First place */
	if (player.score >= maxScore)
	{
		int coverWager = maxScore*2 - player.score + 1;
		int safeWager = player.score - maxScore*2 - 1;
		return MIN(player.score, MAX(coverWager, safeWager));
	}
	
	/* Second place */
	int firstPlaceIndex = players[0].score == maxScore ? 0 : players[1].score == maxScore ? 1 : 2;
	int firstWager = finalJeopardyWager(players, firstPlaceIndex);
	int firstMiss = maxScore - firstWager;
	if (player.score >= minScore)
	{
		int canWin = firstMiss <= 2 * player.score;
		int coverFirst = canWin && firstMiss > player.score ? player.score : 0;
		int coverThird = player.score > minScore * 2 ? 0 : canWin ? 2 * minScore - player.score + 1 : 2 * minScore - player.score; /* Go for the outright win if possible, tie for second if not*/
		return MIN(player.score, MAX(coverFirst, coverThird));
	}
	
	int secondPlaceIndex = 3 - firstPlaceIndex - playerIndex;
	int secondWager = finalJeopardyWager(players, secondPlaceIndex);
	int secondMiss = minScore - secondWager;
	return secondMiss > player.score || firstMiss > player.score ? player.score : 0;
}
