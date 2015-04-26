//Clue.c
#include "Clue.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

double sampleNormal()
{
    double u, v, r;
    do
    {
        u = drand48() * 2.0 - 1.0;
        v = drand48() * 2.0 - 1.0;
        r = u * u + v * v;
    } while (r == 0.0 || r > 1.0);
    double c = sqrt(-2.0 * log(r) / r);
    return u * c;
}

double randomFJPercentage()
{
	return sampleNormal()*0.22 + 0.4924242424;
}

Clue clueMake(int value, int round, int row, int column, int isDailyDouble)
{
	Clue clue;
	clue.value = value;
	clue.round = round;
	clue.row = row;
	clue.column = column;
	clue.isDailyDouble = isDailyDouble;
	clueReset(&clue);
	return clue;
}

void clueReset(Clue *clue)
{
    for (int j = 0; j<3; j++)
    {
        clue->answers[j] = NO_ANSWER;
        clue->wagers[j] = NO_WAGER;
    }
    if (clue->round == 3)
    {
        clue->finalJeopardyOdds = randomFJPercentage();
    }
}

int previousAnswerers(Clue *clue)
{
	return (clue->answers[0] > -1) + (clue->answers[1] > -1) + (clue->answers[2] > -1);
}

double rawOddsOfAnsweringClue(Clue *clue)
{
	const double odds[2][3][5] = {
		{
			{0.9316826244,	0.9020692974,	0.8808619551,	0.8568982492,	0.8262623452}, 
			{0.9344023324,	0.9148061105,	0.8989547038,	0.8820861678,	0.8332403793},
      {0.9861111111,	0.9428571429,	0.9482758621,	0.9449541284,	0.9761904762}
    },
    {
      {0.9191489362,	0.8851981352,	0.846296456,	0.8150716176,	0.7819324269},
      {0.9168356998,	0.8936294565,	0.8767288034,	0.8435064935,	0.8086092715},
      {0.972972973,	0.9652173913,	0.9595959596,	0.9368421053,	0.9444444444}
    }
  };
	return odds[clue->round - 1][previousAnswerers(clue)][clue->row];
}

double rawOddsOfAnsweringDailyDouble(Clue *clue)
{
  const double odds[2][5] = {
    {0.76, 0.7364864865, 0.7255985267, 0.6594800254, 0.6321167883},
    {0.75, 0.7361647362, 0.6603773585, 0.6360884045, 0.5720502901}
  };
	return odds[clue->round - 1][clue->row];
}

double rawOddsOfRingingIn(Clue *clue)
{
	const double odds[2][3][5] = {
    {
      {0.726542166,  0.6600194005,  0.6006297272,  0.5522971451,  0.4693421163}, 
      {0.5396228103,  0.4495181174,  0.3649201599,  0.3127371374,  0.2778616104}, 
      {0.8,  0.724137931,  0.6666666667,  0.5240384615,  0.2809364548}
    }, 
    {
      {0.6867450161,  0.6042841339,  0.5466591934,  0.4895921923,  0.3971898935}, 
      {0.4845967439,  0.3838645318,  0.2695752355,  0.2125177663,  0.1979007232}, 
      {0.6016260163,  0.6318681319,  0.4829268293,  0.3941908714,  0.3114186851}
    }
  };
	return odds[clue->round - 1][previousAnswerers(clue)][clue->row];
	
}

