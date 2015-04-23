//Player.h
#include "Clue.h"

#ifndef PLAYER_H
#define PLAYER_H

typedef struct
{
	int score;
	double buzzerRating;
	double confidenceRating;
	double knowledgeRating;
	double ddFJRating;
} Player;

Player playerMake(int score, double buzzerRating, double confidenceRating, double knowledgeRating, double ddFJRating);
Player averagePlayer();

double oddsPlayerAnsweredClue(Player *player, Clue *clue);
int playerAnsweredClue(Player *player, Clue *clue);

double oddsPlayerAnsweredDailyDouble(Player *player, Clue *clue);
int playerAnsweredDailyDouble(Player *player, Clue *clue);

double oddsPlayerAnsweredFinalJeopardy(Player *player, Clue *clue);
int playerAnsweredFinalJeopardy(Player *player, Clue *clue);

double oddsPlayerAttemptedToRingIn(Player *player, Clue *clue);
int playerAttemptedToRingIn(Player *player, Clue *clue);

int dailyDoubleWager(Player *player, Player *otherPlayers, int moneyLeft);
int *finalJeopardyWagers(Player *players);

#endif