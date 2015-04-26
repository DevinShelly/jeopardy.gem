//Game.h
#include "Player.h"

#ifndef GAME_H
#define GAME_H

typedef struct
{
	Player players[3];
	int numJeopardyClues;
	Clue *jeopardyClues;
	int numDoubleJeopardyClues;
	Clue *doubleJeopardyClues;
	int previousDailyDoubleColumn;
	Clue finalJeopardyClue;
	int playerInControlIndex;
} Game;

Game gameMake(Player *players, int numJeopardyClues, Clue *jeopardyClues, int numDoubleJeopardyClues, Clue *doubleJeopardyClues, Clue finalJeopardyClue, int playerInControlIndex);
Game averageGame();
void gameFree(Game game);
void gameReset(Game *game, int *scores);
void addDailyDouble(Game *game, int round);
void simulateGames(Game *game, int trials, int *wins);
void shuffleClues(Clue *clues, int numClues);
int winningScore(Game *game);
int winningPlayerIndex(Game *game);

#endif