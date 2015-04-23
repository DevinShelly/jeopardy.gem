//Game.c

#include "Game.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "MinMax.h"
#include <stdio.h>

Game gameMake(Player *players, int numJeopardyClues, Clue *jeopardyClues, int numDoubleJeopardyClues, Clue *doubleJeopardyClues, Clue finalJeopardyClue, int playerInControlIndex)
{
	Game game;
    memcpy(game.players, players, sizeof(game.players));
	game.numJeopardyClues = numJeopardyClues;
    game.jeopardyClues = malloc(sizeof(Clue)*numJeopardyClues);
    memcpy(game.jeopardyClues, jeopardyClues, sizeof(Clue)*numJeopardyClues);
	game.doubleJeopardyClues = malloc(sizeof(Clue)*numDoubleJeopardyClues);
    memcpy(game.doubleJeopardyClues, doubleJeopardyClues, sizeof(Clue)*numDoubleJeopardyClues);
	game.numDoubleJeopardyClues = numDoubleJeopardyClues;
	game.finalJeopardyClue = finalJeopardyClue;
	game.playerInControlIndex = playerInControlIndex;
	return game;
}

void gameFree(Game *game)
{
    free(game->jeopardyClues);
    free(game->doubleJeopardyClues);
    free(game);
}

void resetDailyDoubles(Game *game)
{
	int numJeopardyDailyDoubles = 0;
	for (int i = 0; i<game->numJeopardyClues; i++)
	{
		numJeopardyDailyDoubles += game->jeopardyClues[i].isDailyDouble;
	}

	int numDoubleJeopardyDailyDoubles = 0;
	for (int i = 0; i<game->numDoubleJeopardyClues; i++)
	{
		numDoubleJeopardyDailyDoubles += game->doubleJeopardyClues[i].isDailyDouble;
	}
	
	for (int i = 0; i<game->numJeopardyClues; i++)
	{
		game->jeopardyClues[i].isDailyDouble = 0;
	}
	
	for (int i = 0; i<game->numDoubleJeopardyClues; i++)
	{
		game->doubleJeopardyClues[i].isDailyDouble = 0;
	}
	
	if (numJeopardyDailyDoubles)
	{
		addDailyDouble(game, 1);
	}

	for (int i = 0; i<numDoubleJeopardyDailyDoubles; i++)
	{
		addDailyDouble(game, 2);
	}
}

void gameReset(Game *game, int *scores)
{	
    for (int i = 0; i<3; i++)
    {
        game->players[i].score = scores[i];
    }
    
    for (int i = 0; i<game->numJeopardyClues; i++)
    {
        Clue *clue = &game->jeopardyClues[i];
        clueReset(clue);
    }
    
    for (int i = 0; i<game->numDoubleJeopardyClues; i++)
    {
        Clue *clue = &game->doubleJeopardyClues[i];
        clueReset(clue);
    }
    
    clueReset(&game->finalJeopardyClue);
    game->playerInControlIndex = 0;
		
		resetDailyDoubles(game);
    
}

Game averageGame()
{
    Player players[3] = {averagePlayer(), averagePlayer(), averagePlayer()};
    int numClues = 30;
    Clue jClues[numClues];
    Clue djClues[numClues];
    for (int i = 0; i<numClues; i++)
    {
        int value = (i%5+1)*200;
        jClues[i] = clueMake(value, 1, i%5, i/5, 0);
        djClues[i] = clueMake(value*2, 2, i%5, i/5, 0);
    }
    Clue fjClue = clueMake(0, 3, 0, 0, 0);
    return gameMake(players, numClues, jClues, numClues, djClues, fjClue, 0);
}

void addDailyDouble(Game *game, int round)
{
    int clueCounts[5] = {0, 0, 0, 0, 0};
    double odds[5];
    Clue *clues = NULL;
    int numClues = 0;
    if (round == 1)
    {
        clues = game->jeopardyClues;
        numClues = game->numJeopardyClues;
        double denom = 4300.0;
        odds[0] = 1.0/denom;
        odds[1] = 296.0/denom;
        odds[2] = 1086.0/denom;
        odds[3] = 1577.0/denom;
        odds[4] = 1370.0/denom;
    }
    else if (round == 2)
    {
        clues = game->doubleJeopardyClues;
        numClues = game->numDoubleJeopardyClues;
        double denom = 8598.0;
        odds[0] = 12.0/denom;
        odds[1] = 777.0/denom;
        odds[2] = 2438.0/denom;
        odds[3] = 3303.0/denom;
        odds[4] = 2068.0/denom;
    }
		
    for (int i = 0; i<numClues; i++)
    {
        if (round == 2 && clues[i].column == game->firstDJDDColumn)
        {
            clueCounts[clues[i].row]--;
        }
    }
    
    double weightedOdds[5] = {0, 0, 0, 0, 0};
    double sumWeightedOdds = 0.0;
    for (int i = 0; i<5; i++)
    {
        weightedOdds[i] = clueCounts[i] * odds[i];
        sumWeightedOdds += weightedOdds[i];
    }
    
    for (int i = 0; i<5; i++)
    {
        weightedOdds[i] /= sumWeightedOdds;
    }
    
    double dailyDoubleRandomizer = drand48();
    double totalOdds = 0.0;
    
    
    
    int rowToInsertDailyDouble = -1;
    for (int i = 0; i<5; i++)
    {
        totalOdds += weightedOdds[i];
        if (totalOdds >= dailyDoubleRandomizer)
        {
            rowToInsertDailyDouble = i;
            break;
        }
    }
    
    for (int i = 0; i<numClues; i++)
    {
        Clue *clue = &clues[i];
        if (clue->row == rowToInsertDailyDouble && !clue->isDailyDouble)
        {
            clue->isDailyDouble = 1;
            return;
        }
    }
}

void shuffleClues(Clue *clues, int numClues)
{
    for (int i = numClues-1; i>0; i--)
    {
        Clue lastClue = clues[i];
        int randomIndex = rand()%i;
        Clue randomClue = clues[randomIndex];
        clues[i] = randomClue;
        clues[randomIndex] = lastClue;
    }
}

int winningScore(Game *game)
{
    return MAX(game->players[0].score, MAX(game->players[1].score, game->players[2].score));
}

int winningPlayerIndex(Game *game)
{
	double numWinners = 0;
	int topScore = winningScore(game);
	
	if (topScore <= 0)
	{
		return -1;
	}
	
	for (int i = 0; i<3; i++)
	{
		if (game->players[i].score == topScore)
		{
			numWinners += 1.0;
		}
	}
	
	double randomIncrement = 1.0/numWinners;
	double currentValue = 0.0;
	double randomValue = drand48();
	
	for (int i = 0; i<3; i++)
	{
		if (game->players[i].score == topScore)
		{
			currentValue += randomIncrement;
		}
		
		if (currentValue >= randomValue)
		{
			return i;
		}
	}
	
	return -1;
}

//returns the players index who rang in, or -1 if no one does
int indexOfPlayerWhoRangInFirst(Game *game, Clue *clue)
{
	int attemptsToRingIn[3];
	double sumBuzzerRatings = 0.0;
	for (int i = 0; i<3; i++)
	{
		// Someone has already answered correctly
		if (clue->answers[i] == 1)
		{
			return -1;
		}
		
		Player *player = &game->players[i];
		attemptsToRingIn[i] = clue->answers[i] == -1 && playerAttemptedToRingIn(player, clue); //only attempt to ring in if you haven't already given an answer
		if (attemptsToRingIn[i])
		{
			sumBuzzerRatings += player->buzzerRating;
		}
	}
	
	double ringInRandomizer = drand48();
	double oddsOfRingingIn = 0.0;
	for (int i = 0; i<3; i++)
	{
		Player *player = &game->players[i];
		oddsOfRingingIn += (double)attemptsToRingIn[i] * player->buzzerRating / sumBuzzerRatings;
		if (oddsOfRingingIn >= ringInRandomizer || (sumBuzzerRatings == 0.0 && attemptsToRingIn[i])) //check to make sure that a lone player with a rating of 0.0 will ring in
		{
			return i;
		}
	}
	
	return -1;
}

int moneyLeft(Clue *clues, int numClues)
{
	int money = 0;
	for (int i = 0; i<numClues; i++)
	{
		money += clues[i].value; 
	}
	
	return money;
}

void simulateClue(Game *game, Clue *clue)
{
	int indexOfPlayerWhoRangIn = indexOfPlayerWhoRangInFirst(game, clue);
	while (indexOfPlayerWhoRangIn != -1)
	{
		Player *player = &game->players[indexOfPlayerWhoRangIn];
		clue->answers[indexOfPlayerWhoRangIn] = playerAnsweredClue(player, clue);
		if (clue->answers[indexOfPlayerWhoRangIn])
		{
			player->score += clue->value;
			game->playerInControlIndex = indexOfPlayerWhoRangIn;
		} 
		else
		{
			player->score -= clue->value;
		}
		indexOfPlayerWhoRangIn = indexOfPlayerWhoRangInFirst(game, clue);
	}
}

void simulateDailyDouble(Game *game, Clue *dailyDouble, int moneyLeft)
{
    int wager = dailyDoubleWager(&game->players[game->playerInControlIndex], game->players, moneyLeft);
	Player *player = &game->players[game->playerInControlIndex];
	dailyDouble->wagers[game->playerInControlIndex] = wager;
	dailyDouble->answers[game->playerInControlIndex] = playerAnsweredDailyDouble(player, dailyDouble);
	if (dailyDouble->answers[game->playerInControlIndex])
	{
		player->score += wager;
	}
	else
	{
		player->score -= wager;
	}
}

void simulateFinalJeopardy(Game *game)
{
	int *wagers = finalJeopardyWagers(game->players);
	for (int i = 0; i<3; i++)
	{
		game->finalJeopardyClue.wagers[i] = wagers[i];
	}
	
	for (int i = 0; i<3; i++)
	{
		Player *player = &game->players[i];
		Clue *finalJeopardyClue = &game->finalJeopardyClue;
		finalJeopardyClue->answers[i] = playerAnsweredFinalJeopardy(player, finalJeopardyClue);
		player->score += finalJeopardyClue->answers[i] ? finalJeopardyClue->wagers[i] : -finalJeopardyClue->wagers[i];
	}
}

void simulateGame(Game *game)
{
	for (int i = 0; i<game->numJeopardyClues; i++)
	{
		Clue *clue = &game->jeopardyClues[i];
		if (clue->isDailyDouble)
		{
			int remainingMoney = i + 1 == game->numJeopardyClues ? 36000 : 36000 + moneyLeft(&clue[i+1], game->numJeopardyClues - i - 1);
			simulateDailyDouble(game, clue, remainingMoney);
		}
		else
		{
			simulateClue(game, clue);
		}
	}

	//The player in last place starts off in control of the DJ board
	int minScore = MIN(game->players[0].score, MIN(game->players[1].score, game->players[2].score));
	int minPlayerIndex = game->players[0].score == minScore ? 0 : game->players[1].score == minScore ? 1: 2;
	game->playerInControlIndex = minPlayerIndex;

	for (int i = 0; i<game->numDoubleJeopardyClues; i++)
	{
		Clue *clue = &game->doubleJeopardyClues[i];
		if (clue->isDailyDouble)
		{
			int remainingMoney = i + 1 == game->numDoubleJeopardyClues ? 0 : moneyLeft(&clue[i+1], game->numDoubleJeopardyClues - i - 1);
			simulateDailyDouble(game, clue, remainingMoney);
		}
		else
		{
			simulateClue(game, clue);
		}
	}

	simulateFinalJeopardy(game);
}

void simulateGames(Game *game, int trials, int *wins)
{
	wins[0] = 0;
	wins[1] = 0;
	wins[2] = 0;
	
	
	
	int scores[3] = {game->players[0].score, game->players[1].score, game->players[2].score};
	
	for (int i = 0; i<trials; i++)
	{
		simulateGame(game);
		int winningIndex = winningPlayerIndex(game);
		if (winningIndex != -1)
		{
			wins[winningIndex] += 1;
		}

		gameReset(game, scores);
	}
}