//Clue.h
#include <limits.h>

#ifndef CLUE_H
#define CLUE_H

typedef enum
{
	NO_WAGER = INT_MIN,
	NO_ANSWER,
	CORRECT_ANSWER,
	INCORRECT_ANSWER
} ResponseType;

typedef struct
{
	int value;
	int round;
	int row;
	int column;
	int isDailyDouble;
	double finalJeopardyOdds;
	int answers[3];
	int wagers[3];
} Clue;

Clue clueMake(int value, int round, int row, int column, int isDailyDouble);

double rawOddsOfAnsweringClue(Clue *clue);
double rawOddsOfAnsweringDailyDouble(Clue *clue);
double rawOddsOfRingingIn(Clue *clue);
void clueReset(Clue *clue);
double randomFJPercentage();
#endif