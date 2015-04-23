#include "ruby.h"
#include "Game.h"
#include <time.h>

VALUE Jeopardy ;
VALUE cGame;
VALUE cClue;
VALUE cPlayer;

static void rb_hash_set_value_if_nil(VALUE hash, VALUE key, VALUE val)
{
	if (rb_hash_aref(hash, key) == Qnil)
	{
		rb_hash_aset(hash, key, val);
	}
}

#pragma mark - Player

static VALUE allocate_player(VALUE self)
{
	Player *p = ALLOC(Player);
	return (Data_Wrap_Struct(self, NULL, free, p));
}

static VALUE initialize_player(int argc, VALUE *argv, VALUE self)
{
	VALUE hash;

	rb_scan_args(argc, argv, "01", &hash);

	if (hash == Qnil)
	{
		hash = rb_hash_new();
	}

	Check_Type(hash, T_HASH);

	VALUE score = ID2SYM(rb_intern("score"));
	rb_hash_set_value_if_nil(hash, score, INT2NUM(0));

	VALUE buzzer_rating = ID2SYM(rb_intern("buzzer_rating"));
	rb_hash_set_value_if_nil(hash, buzzer_rating, DBL2NUM(1.0));

	VALUE confidence_rating = ID2SYM(rb_intern("confidence_rating"));
	rb_hash_set_value_if_nil(hash, confidence_rating, DBL2NUM(0.0));

	VALUE knowledge_rating = ID2SYM(rb_intern("knowledge_rating"));
	rb_hash_set_value_if_nil(hash, knowledge_rating, DBL2NUM(0.0));

	VALUE dd_fj_rating = ID2SYM(rb_intern("dd_fj_rating"));
	rb_hash_set_value_if_nil(hash, dd_fj_rating, DBL2NUM(0.0));

	Player *p;
	Data_Get_Struct(self, Player, p);
	p->score = NUM2INT(rb_hash_aref(hash, score));
	p->buzzerRating = NUM2DBL(rb_hash_aref(hash, buzzer_rating));
	p->confidenceRating = NUM2DBL(rb_hash_aref(hash, confidence_rating));;
	p->knowledgeRating = NUM2DBL(rb_hash_aref(hash, knowledge_rating));;
	p->ddFJRating = NUM2DBL(rb_hash_aref(hash, dd_fj_rating));;
	return Qnil;
}

static VALUE score(VALUE self)
{
	Player *p;
	Data_Get_Struct(self, Player, p);
	return INT2NUM(p->score);
}

static VALUE set_score(VALUE self, VALUE score)
{
	Player *p;
	Data_Get_Struct(self, Player, p);
	p->score = NUM2INT(score);
	return Qnil;
}

static VALUE buzzer_rating(VALUE self)
{
	Player *p;
	Data_Get_Struct(self, Player, p);
	return DBL2NUM(p->buzzerRating);
}

static VALUE set_buzzer_rating(VALUE self, VALUE buzzer_rating)
{
	Player *p;
	Data_Get_Struct(self, Player, p);
	p->buzzerRating = NUM2DBL(buzzer_rating);
	return Qnil;
}

static VALUE knowledge_rating(VALUE self)
{
	Player *p;
	Data_Get_Struct(self, Player, p);
	return DBL2NUM(p->knowledgeRating);
}

static VALUE set_knowledge_rating(VALUE self, VALUE knowledge_rating)
{
	Player *p;
	Data_Get_Struct(self, Player, p);
	p->knowledgeRating = NUM2DBL(knowledge_rating);
	return Qnil;
}

static VALUE confidence_rating(VALUE self)
{
	Player *p;
	Data_Get_Struct(self, Player, p);
	return DBL2NUM(p->confidenceRating);
}

static VALUE set_confidence_rating(VALUE self, VALUE confidence_rating)
{
	Player *p;
	Data_Get_Struct(self, Player, p);
	p->confidenceRating = NUM2DBL(confidence_rating);
	return Qnil;
}

static VALUE dd_fj_rating(VALUE self)
{
	Player *p;
	Data_Get_Struct(self, Player, p);
	return DBL2NUM(p->ddFJRating);
}

static VALUE set_dd_fj_rating(VALUE self, VALUE dd_fj_rating)
{
	Player *p;
	Data_Get_Struct(self, Player, p);
	p->ddFJRating = NUM2DBL(dd_fj_rating);
	return Qnil;
}

#pragma mark - Clue

static VALUE allocate_clue(VALUE self)
{
	Clue *c = ALLOC(Clue);
	return (Data_Wrap_Struct(self, NULL, free, c));
}

static VALUE initialize_clue(int argc, VALUE *argv, VALUE self)
{
	VALUE hash;

	rb_scan_args(argc, argv, "01", &hash);

	if (hash == Qnil)
	{
		hash = rb_hash_new();
	}

	Check_Type(hash, T_HASH);

	VALUE val = ID2SYM(rb_intern("value"));
	rb_hash_set_value_if_nil(hash, val, INT2NUM(0));

	VALUE rnd = ID2SYM(rb_intern("round"));
	rb_hash_set_value_if_nil(hash, rnd, INT2NUM(1));

	VALUE row = ID2SYM(rb_intern("row"));
	rb_hash_set_value_if_nil(hash, row, INT2NUM(0));

	VALUE column = ID2SYM(rb_intern("column"));
	rb_hash_set_value_if_nil(hash, column, INT2NUM(0));

	VALUE is_daily_double = ID2SYM(rb_intern("is_daily_double"));
	rb_hash_set_value_if_nil(hash, is_daily_double, Qfalse);
	
	VALUE final_jeopardy_odds = ID2SYM(rb_intern("final_jeopardy_odds"));
	rb_hash_set_value_if_nil(hash, final_jeopardy_odds, DBL2NUM(randomFJPercentage()));
	
	VALUE defaultAnswersAndWagers = rb_ary_new();
	rb_ary_push(defaultAnswersAndWagers, INT2NUM(-1));
	rb_ary_push(defaultAnswersAndWagers, INT2NUM(-1));
	rb_ary_push(defaultAnswersAndWagers, INT2NUM(-1));
	
	VALUE answers = ID2SYM(rb_intern("answers"));
	rb_hash_set_value_if_nil(hash, answers, defaultAnswersAndWagers);
	
	VALUE wagers = ID2SYM(rb_intern("wagers"));
	rb_hash_set_value_if_nil(hash, wagers, defaultAnswersAndWagers);

	Clue *c;
	Data_Get_Struct(self, Clue, c);
	c->value = NUM2INT(rb_hash_aref(hash, val));
	c->round = NUM2INT(rb_hash_aref(hash, rnd));
	c->row = NUM2INT(rb_hash_aref(hash, row));
	c->column = NUM2INT(rb_hash_aref(hash, column));
	c->isDailyDouble = (rb_hash_aref(hash, is_daily_double));
	c->finalJeopardyOdds = NUM2DBL(rb_hash_aref(hash, final_jeopardy_odds));
	
	VALUE answersArray = rb_hash_aref(hash, answers);
	VALUE wagersArray = rb_hash_aref(hash, wagers);
	for (int i = 0; i<3; i++)
	{
		VALUE iV = INT2NUM(i);
		c->answers[i] = NUM2INT(rb_ary_aref(1, &iV, answersArray));
		c->wagers[i] = NUM2INT(rb_ary_aref(1, &iV, wagersArray));
	}
	return Qnil;
}

static VALUE val(VALUE self)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	return INT2NUM(clue->value);
}

static VALUE set_val(VALUE self, VALUE val)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	clue->value = NUM2INT(val);
	return Qnil;
}

static VALUE rnd(VALUE self)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	return INT2NUM(clue->round);
}

static VALUE set_rnd(VALUE self, VALUE rnd)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	clue->round = NUM2INT(rnd);
	return Qnil;
}

static VALUE row(VALUE self)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	return INT2NUM(clue->row);
}

static VALUE set_row(VALUE self, VALUE row)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	clue->row = NUM2INT(row);
	return Qnil;
}

static VALUE column(VALUE self)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	return INT2NUM(clue->column);
}

static VALUE set_column(VALUE self, VALUE column)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	clue->column = NUM2INT(column);
	return Qnil;
}

static VALUE is_daily_double(VALUE self)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	return clue->isDailyDouble ? Qtrue: Qfalse;
}

static VALUE set_is_daily_double(VALUE self, VALUE is_daily_double)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	clue->isDailyDouble = is_daily_double == Qtrue ? 1 : 0;
	return Qnil;
}

static VALUE final_jeopardy_odds(VALUE self)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	return DBL2NUM(clue->finalJeopardyOdds);
}

static VALUE set_final_jeopardy_odds(VALUE self, VALUE final_jeopardy_odds)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	clue->finalJeopardyOdds = NUM2DBL(final_jeopardy_odds);
	return Qnil;
}

static VALUE answers(VALUE self)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	VALUE answersArray = rb_ary_new();
	
	for (int i = 0; i<3; i++)
	{
		if (clue->answers[i] == 1)
		{
			rb_ary_push(answersArray, Qtrue);
		}
		else if (clue->answers[i] == 0)
		{
			rb_ary_push(answersArray, Qfalse);
		}
		else
		{
			rb_ary_push(answersArray, Qnil);
		}
	}
	
	return answersArray;
}

static VALUE set_answers(VALUE self, VALUE answers)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	for (int i = 0; i<3; i++)
	{
		VALUE iV = INT2NUM(i);
		VALUE answer = rb_ary_aref(1, &iV, answers);
		if (answer == Qtrue)
		{
			clue->answers[i] = 1;
		}
		else if (answer == Qfalse)
		{
			clue->answers[i] = 0;
		}
		else
		{
			clue->answers[i] = -1;
		}
	}
	return Qnil;
}

static VALUE wagers(VALUE self)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	VALUE wagersArray = rb_ary_new();
	
	for (int i = 0; i<3; i++)
	{
		if (clue->wagers[i] == -1)
		{
			rb_ary_push(wagersArray, Qnil);
		}
		else
		{
			rb_ary_push(wagersArray, INT2NUM(clue->wagers[i]));
		}
	}
	
	return wagersArray;
}

static VALUE set_wagers(VALUE self, VALUE wagers)
{
	Clue *clue;
	Data_Get_Struct(self, Clue, clue);
	for (int i = 0; i<3; i++)
	{
		VALUE iV = INT2NUM(i);
		VALUE wager = rb_ary_aref(1, &iV, wagers);
		if (wager == Qtrue)
		{
			clue->wagers[i] = 1;
		}
		else if (wager == Qfalse)
		{
			clue->wagers[i] = 0;
		}
		else
		{
			clue->wagers[i] = Qnil;
		}
	}
	return Qnil;
}

static VALUE allocate_game(VALUE self)
{
	Game *g = ALLOC(Game);
	return Data_Wrap_Struct(self, NULL, gameFree, g);
}

VALUE default_clue_locations;
VALUE j_clue_locations_key;
VALUE dj_clue_locations_key;
VALUE j_daily_doubles_left_key;
VALUE dj_daily_doubles_left_key;
VALUE j_daily_double_location_key;
VALUE dj_daily_double_locations_key;
VALUE previous_dj_dd_column_key;
VALUE first_double_jeopardy_daily_double_column_key;
VALUE players_key;

static void load_default_game_hash(VALUE hash)
{
	default_clue_locations = rb_ary_new();
	for (int i = 0; i<30; i++)
	{
		rb_ary_push(default_clue_locations, INT2NUM(i));
	}	

	j_clue_locations_key = ID2SYM(rb_intern("j_clue_locations"));
	rb_hash_set_value_if_nil(hash, j_clue_locations_key, default_clue_locations);
	
	dj_clue_locations_key = ID2SYM(rb_intern("dj_clue_locations"));
	rb_hash_set_value_if_nil(hash, dj_clue_locations_key, default_clue_locations);

	j_daily_doubles_left_key = ID2SYM(rb_intern("j_daily_doubles_left"));
	rb_hash_set_value_if_nil(hash, j_daily_doubles_left_key, INT2NUM(1));

	dj_daily_doubles_left_key = ID2SYM(rb_intern("dj_daily_doubles_left"));
	rb_hash_set_value_if_nil(hash, dj_daily_doubles_left_key, INT2NUM(2));

	j_daily_double_location_key = ID2SYM(rb_intern("j_daily_double_location"));
	rb_hash_set_value_if_nil(hash, j_daily_double_location_key, INT2NUM(-1));

	dj_daily_double_locations_key = ID2SYM(rb_intern("dj_daily_double_locations"));
	rb_hash_set_value_if_nil(hash, dj_daily_double_locations_key, rb_ary_new());
	
	previous_dj_dd_column_key = ID2SYM(rb_intern("previous_dj_dd_column_key"));
	rb_hash_set_value_if_nil(hash, previous_dj_dd_column_key, INT2NUM(-1));
	
	first_double_jeopardy_daily_double_column_key = ID2SYM(rb_intern("first_double_jeopardy_daily_double_column"));
	rb_hash_set_value_if_nil(hash, first_double_jeopardy_daily_double_column_key, INT2NUM(-1));

	players_key = ID2SYM(rb_intern("players"));
	VALUE avg_players = rb_ary_new();
	for (int i = 0; i<3; i++)
	{
		Player *p = ALLOC(Player);
		Player avg = averagePlayer();
		memcpy(p, &avg, sizeof(Player));
		rb_ary_push(avg_players, Data_Wrap_Struct(cPlayer, NULL, free, p));
	}
	rb_hash_set_value_if_nil(hash, players_key, avg_players);
}

static void load_players(Game *game, VALUE hash)
{
	VALUE players = rb_hash_aref(hash, players_key);
	for (int i = 0; i<3; i++)
	{
		VALUE iV = INT2NUM(i);
		VALUE player = rb_ary_aref(1, &iV, players);
		Player *p;
		Data_Get_Struct(player, Player, p);
		memcpy(&game->players[i], p, sizeof(Player));
		// game->players[i] = averagePlayer();
	}
}

static void *clues(VALUE clue_locations, VALUE daily_double_locations, int round)
{
	int num_clues = RARRAY_LEN(clue_locations);
	Clue *clues = malloc(sizeof(Clue)*num_clues);
	for (int i = 0; i<num_clues; i++)
	{
		VALUE iV = INT2NUM(i);
		int location = NUM2INT(rb_ary_aref(1, &iV, clue_locations));
		Clue clue = clueMake(round*(location%5*200 + 200), round, location%5, location/5, 0);
		if (TYPE(daily_double_locations) == T_ARRAY)
		{
			int num_daily_doubles = RARRAY_LEN(daily_double_locations);
			for (int j = 0; j<num_daily_doubles; j++)
			{
				VALUE jV = INT2NUM(j);
				int location = NUM2INT(rb_ary_aref(1, &jV, daily_double_locations));
				clue.isDailyDouble = clue.isDailyDouble || location == i;
			}
		}
		else
		{
			clue.isDailyDouble = NUM2INT(daily_double_locations) == i;
		}
		memcpy(&clues[i], &clue, sizeof(Clue));
	}
	return clues;
}

void load_jeopardy_clues(Game *g, VALUE hash)
{
	VALUE j_clue_locations = rb_hash_aref(hash, j_clue_locations_key);
	VALUE j_daily_double_location = rb_hash_aref(hash, j_daily_double_location_key);
	g->jeopardyClues = clues(j_clue_locations, j_daily_double_location, 1);
	g->numJeopardyClues = RARRAY_LEN(j_clue_locations);

	VALUE j_daily_doubles_left = rb_hash_aref(hash, j_daily_doubles_left_key);
	int existing_j_daily_doubles = (NUM2INT(j_daily_double_location) > -1 && NUM2INT(j_daily_double_location) < 30);
	if (NUM2INT(j_daily_doubles_left) - existing_j_daily_doubles)
	{
		addDailyDouble(g, 1);
	}
}

void load_double_jeopardy_clues(Game *g, VALUE hash)
{
	VALUE dj_clue_locations = rb_hash_aref(hash, dj_clue_locations_key);
	VALUE dj_daily_double_locations = rb_hash_aref(hash, dj_daily_double_locations_key);
	g->doubleJeopardyClues = clues(dj_clue_locations, dj_daily_double_locations, 2);
	g->numDoubleJeopardyClues = RARRAY_LEN(dj_clue_locations);
	
	g->firstDJDDColumn = NUM2INT(rb_hash_aref(hash, first_double_jeopardy_daily_double_column_key));
	VALUE dj_daily_doubles_left = rb_hash_aref(hash, dj_daily_doubles_left_key);
	int existing_dj_daily_doubles = RARRAY_LEN(dj_daily_double_locations);
	int dj_daily_doubles_to_add = NUM2INT(dj_daily_doubles_left) - existing_dj_daily_doubles;
	for (int i = 0; i<dj_daily_doubles_to_add; i++)
	{
		addDailyDouble(g, 2);
	}
}

static VALUE initialize_game(int argc, VALUE *argv, VALUE self)
{	
	VALUE hash;
	rb_scan_args(argc, argv, "01", &hash);
		
	if (hash == Qnil)
	{
		hash = rb_hash_new();
	}
	
	Game *g;
	Data_Get_Struct(self, Game, g);
	
	load_default_game_hash(hash);
	load_players(g, hash);
	load_jeopardy_clues(g, hash);
	load_double_jeopardy_clues(g, hash);
	g->finalJeopardyClue = clueMake(0, 3, 0, 0, 0);
	g->playerInControlIndex = 0;
	return Qnil;
}

static VALUE players(VALUE self)
{
	Game *g;
	Data_Get_Struct(self, Game, g);
	VALUE players_array = rb_ary_new();
	for (int i = 0; i<3; i++)
	{
		rb_ary_push(players_array, Data_Wrap_Struct(cPlayer, NULL, NULL, &g->players[i]));
	}
	return players_array;
}

static VALUE set_players(VALUE self, VALUE players)
{
	Game *g;
	Data_Get_Struct(self, Game, g);
	for (int i = 0; i<3; i++)
	{
		VALUE playerIndex = INT2NUM(i);
		VALUE newPlayer = rb_ary_aref(1, &playerIndex, players);
		Player *p;
		Data_Get_Struct(newPlayer, Player, p);
		memcpy(p, &g->players[i], sizeof(Player));
	}
	return Qnil;
}

static VALUE ruby_clues(Clue *clues, int numClues)
{
	VALUE clues_array = rb_ary_new();
	for (int i = 0; i<numClues; i++)
	{
		rb_ary_push(clues_array, Data_Wrap_Struct(cClue, NULL, NULL, &clues[i]));
	}
	return clues_array;
}

static void *c_clues(VALUE clues, Clue *pointer)
{
	int length = RARRAY_LEN(clues);
	free(pointer);
	pointer = malloc(sizeof(Clue)*length);
	for (int i = 0; i<length; i++)
	{
		Clue *clue;
		VALUE clueIndex = INT2NUM(i);
		VALUE rClue = rb_ary_aref(1, &clueIndex, clues);
		Data_Get_Struct(rClue, Clue, clue);
		memcpy(clue, &pointer[i], sizeof(Clue));
	}
	return pointer;
}

static VALUE jeopardy_clues(VALUE self)
{
	Game *g;
	Data_Get_Struct(self, Game, g);
	return ruby_clues(g->jeopardyClues, g->numJeopardyClues);
}

static VALUE set_jeopardy_clues(VALUE self, VALUE clues)
{
	Check_Type(clues, T_ARRAY);
	Game *g;
	Data_Get_Struct(self, Game, g);
	g->jeopardyClues = c_clues(clues, g->jeopardyClues);
	g->numJeopardyClues = RARRAY_LEN(clues);
	return Qnil;
}

static VALUE double_jeopardy_clues(VALUE self)
{
	Game *g;
	Data_Get_Struct(self, Game, g);
	return ruby_clues(g->doubleJeopardyClues, g->numDoubleJeopardyClues);
}

static VALUE set_double_jeopardy_clues(VALUE self, VALUE clues)
{
	Game *g;
	Data_Get_Struct(self, Game, g);
	g->doubleJeopardyClues = c_clues(clues, g->doubleJeopardyClues);
	g->numDoubleJeopardyClues = RARRAY_LEN(clues);
	return Qnil;
}

static VALUE final_jeopardy_clue(VALUE self)
{
	Game *g;
	Data_Get_Struct(self, Game, g);
	return Data_Wrap_Struct(cClue, NULL, NULL, &g->finalJeopardyClue);
}

static VALUE set_final_jeopardy_clue(VALUE self, VALUE clue)
{
	Game *g;
	Data_Get_Struct(self, Game, g);
	Clue *c;
	Data_Get_Struct(clue, Clue, c);
	memcpy(&g->finalJeopardyClue, c, sizeof(Clue));
	return Qnil;
}

static VALUE first_DJ_DD_Column(VALUE self)
{
	Game *g;
	Data_Get_Struct(self, Game, g);
	return INT2NUM(g->firstDJDDColumn);
}

static VALUE set_first_DJ_DD_Column(VALUE self, VALUE column)
{
	Game *g;
	Data_Get_Struct(self, Game, g);
	g->firstDJDDColumn = NUM2INT(column);
	return Qnil;
}

static VALUE sim(int argc, VALUE *argv, VALUE self)
{
	VALUE trials;

	rb_scan_args(argc, argv, "01", &trials);
	
	Game *g;
	Data_Get_Struct(self, Game, g);
	int wins[3] = {0, 0, 0};
	
	if (trials == Qnil)
	{
		simulateGame(g);
		if (winningPlayerIndex(g) != -1)
		{
			wins[winningPlayerIndex(g)] += 1;
		}
	}
	else
	{
		simulateGames(g, NUM2INT(trials), wins);
	}
	
	VALUE output = rb_ary_new();
	for (int i = 0; i<3; i++)
	{
		rb_ary_push(output, INT2NUM(wins[i]));
	}
	return output;
}

static VALUE reset(VALUE self, VALUE scores)
{
	Game *g;
	Data_Get_Struct(self, Game, g);
	
	int cScores[3];
	for (int i = 0; i<3; i++)
	{
		VALUE iV = INT2NUM(i);
		VALUE score = rb_ary_aref(1, &iV, scores);
		cScores[i] = NUM2INT(score);
	}
	
	gameReset(g, cScores);
	return Qnil;
}

void Init_jeopardy()  
{	
	Jeopardy = rb_define_module("Jeopardy");
	
	srand48(time(NULL));
	srand(time(NULL));
	
	cPlayer = rb_define_class_under(Jeopardy, "Player", rb_cObject);
	rb_define_alloc_func(cPlayer, allocate_player);
	rb_define_method(cPlayer, "initialize", initialize_player, -1);
	rb_define_method(cPlayer, "score", score, 0);
	rb_define_method(cPlayer, "score=", set_score, 1);
	rb_define_method(cPlayer, "buzzer_rating", buzzer_rating, 0);
	rb_define_method(cPlayer, "buzzer_rating=", set_buzzer_rating, 1);
	rb_define_method(cPlayer, "knowledge_rating", knowledge_rating, 0);
	rb_define_method(cPlayer, "knowledge_rating=", set_knowledge_rating, 1);
	rb_define_method(cPlayer, "confidence_rating", confidence_rating, 0);
	rb_define_method(cPlayer, "confidence_rating=", set_confidence_rating, 1);
	rb_define_method(cPlayer, "dd_fj_rating", dd_fj_rating, 0);
	rb_define_method(cPlayer, "dd_fj_rating=", set_dd_fj_rating, 1);
	
	cClue = rb_define_class_under(Jeopardy, "Clue", rb_cObject);
	rb_define_alloc_func(cClue, allocate_clue);
	rb_define_method(cClue, "initialize", initialize_clue, -1);
	rb_define_method(cClue, "value", val, 0);
	rb_define_method(cClue, "value=", set_val, 1);
	rb_define_method(cClue, "round", rnd, 0);
	rb_define_method(cClue, "round=", set_rnd, 1);
	rb_define_method(cClue, "row", row, 0);
	rb_define_method(cClue, "row=", set_row, 1);
	rb_define_method(cClue, "column", column, 0);
	rb_define_method(cClue, "column=", set_column, 1);
	rb_define_method(cClue, "is_daily_double", is_daily_double, 0);
	rb_define_method(cClue, "is_daily_double=", set_is_daily_double, 1);
	rb_define_method(cClue, "final_jeopardy_odds", final_jeopardy_odds, 0);
	rb_define_method(cClue, "final_jeopardy_odds=", set_final_jeopardy_odds, 1);
	rb_define_method(cClue, "answers", answers, 0);
	rb_define_method(cClue, "answers=", set_answers, 1);
	rb_define_method(cClue, "wagers", wagers, 0);
	rb_define_method(cClue, "wagers=", set_wagers, 1);
	
	cGame = rb_define_class_under(Jeopardy, "Game", rb_cObject);
	rb_define_alloc_func(cGame, allocate_game);
	rb_define_method(cGame, "initialize", initialize_game, -1);
	rb_define_method(cGame, "players", players, 0);
	rb_define_method(cGame, "players=", set_players, 1);
	rb_define_method(cGame, "jeopardy_clues", jeopardy_clues, 0);
	rb_define_method(cGame, "jeopardy_clues=", set_jeopardy_clues, 1);
	rb_define_method(cGame, "double_jeopardy_clues", double_jeopardy_clues, 0);
	rb_define_method(cGame, "double_jeopardy_clues=", set_double_jeopardy_clues, 1);
	rb_define_method(cGame, "final_jeopardy_clue", final_jeopardy_clue, 0);
	rb_define_method(cGame, "final_jeopardy_clue=", set_final_jeopardy_clue, 1);
	rb_define_method(cGame, "first_DJ_DD_Column", first_DJ_DD_Column, 0);
	rb_define_method(cGame, "first_DJ_DD_Column=", set_first_DJ_DD_Column, 0);
	rb_define_method(cGame, "simulate", sim, -1);
	rb_define_method(cGame, "reset", reset, 1);
}