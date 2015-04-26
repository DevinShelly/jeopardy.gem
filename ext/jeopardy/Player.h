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
ResponseType playerAnsweredClue(Player *player, Clue *clue);

double oddsPlayerAnsweredDailyDouble(Player *player, Clue *clue);
ResponseType playerAnsweredDailyDouble(Player *player, Clue *clue);

double oddsPlayerAnsweredFinalJeopardy(Player *player, Clue *clue);
ResponseType playerAnsweredFinalJeopardy(Player *player, Clue *clue);

double oddsPlayerAttemptedToRingIn(Player *player, Clue *clue);
int playerAttemptedToRingIn(Player *player, Clue *clue);

int dailyDoubleWager(Player *players, int playerInControl, int moneyLeft);
int finalJeopardyWager(Player *players, int playerIndex);

#endif