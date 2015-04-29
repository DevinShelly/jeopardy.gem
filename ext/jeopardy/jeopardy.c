#include "ruby.h"
#include "Game.h"
#include <time.h>

VALUE Jeopardy;
VALUE rbGame;
VALUE rbClue;
VALUE rbPlayer;

static Clue c_clue(VALUE rb_clue)
{
	if (rb_obj_is_kind_of(rb_clue, rbClue) == Qfalse)
	{
		rb_raise(rb_eTypeError, "Tried to convert a non-Clue Ruby object to a Clue struct.");
	}
	Clue clue;
	clue.value = NUM2INT(rb_funcall(rb_clue, rb_intern("value"), 0));
	clue.round = NUM2INT(rb_funcall(rb_clue, rb_intern("round"), 0));
	clue.row = NUM2INT(rb_funcall(rb_clue, rb_intern("row"), 0));
	clue.column = NUM2INT(rb_funcall(rb_clue, rb_intern("column"), 0));
	clue.value = NUM2INT(rb_funcall(rb_clue, rb_intern("value"), 0));
	clue.isDailyDouble = 0;
	clueReset(&clue);
	return clue;
}

static Clue *c_clues(VALUE rb_clues)
{
	Check_Type(rb_clues, T_ARRAY);
	Clue *clues = malloc(sizeof(Clue)*RARRAY_LEN(rb_clues));
	for (int i = 0; i<RARRAY_LEN(rb_clues); i++)
	{
		VALUE iV = INT2NUM(i);
		VALUE rb_clue = rb_ary_aref(1, &iV, rb_clues);
		clues[i] = c_clue(rb_clue);
	}
	return clues;
}

static Player c_player(VALUE rb_player)
{
	if (rb_obj_is_kind_of(rb_player, rbPlayer) == Qfalse)
	{
		rb_raise(rb_eTypeError, "Tried to convert a non-Player Ruby object to a Player struct.");
	}
	
	Player player;
	player.score = NUM2INT(rb_funcall(rb_player, rb_intern("score"), 0));
	player.buzzerRating = NUM2DBL(rb_funcall(rb_player, rb_intern("buzzer_rating"), 0));
	player.confidenceRating = NUM2DBL(rb_funcall(rb_player, rb_intern("confidence_rating"), 0));
	player.knowledgeRating = NUM2DBL(rb_funcall(rb_player, rb_intern("knowledge_rating"), 0));
	player.ddFJRating = NUM2DBL(rb_funcall(rb_player, rb_intern("dd_fj_rating"), 0));
	return player;
}

static Player *c_players(VALUE rb_players)
{
	Check_Type(rb_players, T_ARRAY);
	if (RARRAY_LEN(rb_players) != 3)
	{
		rb_raise(rb_eArgError, "Players array must contain three players.");
	}
	
	Player *players = malloc(sizeof(Player)*3);
	for (int i = 0; i<3; i++)
	{
		VALUE iV = INT2NUM(i);
		VALUE rb_player = rb_ary_aref(1, &iV, rb_players);
		players[i] = c_player(rb_player);
	}
	return players; 
}

static Game c_game(VALUE rb_game)
{
	Game g;
	Player *players_pt = c_players(rb_funcall(rb_game, rb_intern("players"), 0));
	memcpy(g.players, players_pt, sizeof(Player)*3);
	free(players_pt);
	
	VALUE jeopardy_clues = rb_funcall(rb_game, rb_intern("jeopardy_clues"), 0);
	g.jeopardyClues = c_clues(jeopardy_clues);
	g.numJeopardyClues = RARRAY_LEN(jeopardy_clues);
	
	VALUE double_jeopardy_clues = rb_funcall(rb_game, rb_intern("double_jeopardy_clues"), 0);
	g.doubleJeopardyClues = c_clues(double_jeopardy_clues);
	g.numDoubleJeopardyClues = RARRAY_LEN(double_jeopardy_clues);
	g.finalJeopardyClue = clueMake(0, 3, 0, 0, 0);
	g.playerInControlIndex = 0;
	VALUE previous_daily_double_column = rb_funcall(rb_game, rb_intern("previous_daily_double_column"), 0);
	
	if (TYPE(previous_daily_double_column) != T_FIXNUM && TYPE(previous_daily_double_column) != T_NIL)
	{
		rb_raise(rb_eTypeError, "Previous daily double column must either be a FIXNUM or NIL");
	}
	
	g.previousDailyDoubleColumn = previous_daily_double_column == Qnil ? -1 : NUM2INT(previous_daily_double_column);
	
	VALUE jeopardy_daily_doubles_left = rb_funcall(rb_game, rb_intern("jeopardy_daily_doubles_left"), 0);
	Check_Type(jeopardy_daily_doubles_left, T_FIXNUM);
	for (int i = 0; i<NUM2INT(jeopardy_daily_doubles_left); i++)
	{
		addDailyDouble(&g, 1);
	}
	VALUE double_jeopardy_daily_doubles_left = rb_funcall(rb_game, rb_intern("double_jeopardy_daily_doubles_left"), 0);
	Check_Type(double_jeopardy_daily_doubles_left, T_FIXNUM);
	for (int i = 0; i<NUM2INT(double_jeopardy_daily_doubles_left); i++)
	{
		addDailyDouble(&g, 2);
	}
	
	return g;
}

static void sync_clue(VALUE rb_clue, Clue c_clue, VALUE rb_players)
{
	VALUE rb_answers = rb_hash_new();
	VALUE rb_wagers = rb_hash_new();
	rb_funcall(rb_clue, rb_intern("answers="), 1, rb_answers);
	rb_funcall(rb_clue, rb_intern("wagers="), 1, rb_wagers);
	
	for (int j = 0; j<3; j++)
	{
		VALUE jV = INT2NUM(j);
		VALUE rb_player = rb_ary_aref(1, &jV, rb_players);
		if(c_clue.wagers[j] != NO_WAGER)
		{
			rb_hash_aset(rb_wagers, rb_player, INT2NUM(c_clue.wagers[j]));
		}
		if (c_clue.answers[j] == CORRECT_ANSWER)
		{
			rb_hash_aset(rb_answers, rb_player, Qtrue);
		}
		else if (c_clue.answers[j] == INCORRECT_ANSWER)
		{
			rb_hash_aset(rb_answers, rb_player, Qfalse);
		}
		rb_funcall(rb_clue, rb_intern("round="), 1, INT2NUM(c_clue.round));
		rb_funcall(rb_clue, rb_intern("row="), 1, INT2NUM(c_clue.row));
		rb_funcall(rb_clue, rb_intern("column="), 1, INT2NUM(c_clue.column));
		rb_funcall(rb_clue, rb_intern("value="), 1, INT2NUM(c_clue.value));
	}
}

static void sync_player(VALUE rb_player, Player c_player)
{
	rb_funcall(rb_player, rb_intern("score="), 1, INT2NUM(c_player.score));
}


static void sync_game(VALUE rb_game, Game c_game)
{
	VALUE rb_players = rb_funcall(rb_game, rb_intern("players"), 0);
	VALUE rb_jeopardy_clues = rb_funcall(rb_game, rb_intern("jeopardy_clues"), 0);
	for (int i = 0; i<c_game.numJeopardyClues; i++)
	{
		VALUE iV = INT2NUM(i);
		VALUE rb_clue = rb_ary_aref(1, &iV, rb_jeopardy_clues);
		sync_clue(rb_clue, c_game.jeopardyClues[i], rb_players);
	}
	
	VALUE rb_double_jeopardy_clues = rb_funcall(rb_game, rb_intern("double_jeopardy_clues"), 0);
	for (int i = 0; i<c_game.numDoubleJeopardyClues; i++)
	{
		VALUE iV = INT2NUM(i);
		VALUE rb_clue = rb_ary_aref(1, &iV, rb_double_jeopardy_clues);
		sync_clue(rb_clue, c_game.doubleJeopardyClues[i], rb_players);
	}
	
	VALUE rb_final_jeopardy_clue = rb_funcall(rb_game, rb_intern("final_jeopardy_clue"), 0);
	sync_clue(rb_final_jeopardy_clue, c_game.finalJeopardyClue, rb_players);
	
	for (int i = 0; i<3; i++)
	{
		VALUE iV = INT2NUM(i);
		VALUE rb_player = rb_ary_aref(1, &iV, rb_players);
		sync_player(rb_player, c_game.players[i]);
	}
}

static void rb_hash_set_if_nil(VALUE hash, VALUE key, VALUE val)
{
	if (rb_hash_aref(hash, key) == Qnil)
	{
		rb_hash_aset(hash, key, val);
	}
}

static VALUE simulate(int argc, VALUE *argv, VALUE self)
{
	VALUE options;
	rb_scan_args(argc, argv, "01", &options);
	if (options == Qnil)
	{
		options = rb_hash_new();
	}
	Check_Type(options, T_HASH);
	VALUE trials_key = ID2SYM(rb_intern("trials"));
	VALUE seed_key = ID2SYM(rb_intern("seed"));
	rb_hash_set_if_nil(options, trials_key, INT2NUM(1));
	rb_hash_set_if_nil(options, seed_key, INT2NUM(time(NULL)));
	
	VALUE trials = rb_hash_aref(options, trials_key);
	VALUE seed = rb_hash_aref(options, seed_key);
	
	Check_Type(trials, T_FIXNUM);
	Check_Type(seed, T_FIXNUM);
	if (NUM2INT(trials) < 1)
	{
		rb_raise(rb_eArgError, "The number of trials a game is simulated must be greater than zero.");
	}
	
	srand(NUM2INT(seed));
	srand48(NUM2INT(seed));
	
	Game game = c_game(self);
	
	int wins[3] = {0, 0, 0};
	simulateGames(&game, NUM2INT(trials), wins);
	
	sync_game(self, game);
	gameFree(game);
	VALUE wins_hash = rb_hash_new();
	VALUE players = rb_funcall(self, rb_intern("players"), 0);
	
	for (int i = 0; i<3; i++)
	{
		VALUE iV = INT2NUM(i);
		VALUE player = rb_ary_aref(1, &iV, players);
		rb_hash_aset(wins_hash, player, INT2NUM(wins[i]));
	}
	
	return wins_hash;
}

static VALUE daily_double_odds(VALUE self, VALUE rb_clue)
{
	Player player = c_player(self);
	Clue clue = c_clue(rb_clue);
	clue.isDailyDouble = 1;
	return DBL2NUM(oddsPlayerAnsweredDailyDouble(&player, &clue));
}

static VALUE clue_odds(VALUE self, VALUE rb_clue)
{
	Player player = c_player(self);
	Clue clue = c_clue(rb_clue);
	return DBL2NUM(oddsPlayerAnsweredClue(&player, &clue));
}

static VALUE final_jeopardy_odds(VALUE self)
{
	Player player = c_player(self);
	Clue finalJeopardy;
	finalJeopardy.finalJeopardyStandardDeviations = 0.0;
	return DBL2NUM(oddsPlayerAnsweredFinalJeopardy(&player, &finalJeopardy));
}

void Init_jeopardy()	
{
		
#if RDOC_CAN_PARSE_DOCUMENTATION
    Jeopardy = rb_define_module("Jeopardy");
		rbGame = rb_define_class_under(Jeopardy, "Game", rb_cObject);
		rbPlayer = rb_define_class_under(Jeopardy, "Player", rb_cObject);
		rbClue = rb_define_class_under(Jeopardy, "Clue", rb_cObject);
#endif
	Jeopardy = rb_const_get(rb_cObject, rb_intern("Jeopardy"));
	rbGame = rb_const_get(Jeopardy, rb_intern("Game"));
	rbClue = rb_const_get(Jeopardy, rb_intern("Clue"));
	rbPlayer = rb_const_get(Jeopardy, rb_intern("Player"));
	rb_define_method(rbGame, "simulate", simulate, -1);
	rb_define_method(rbPlayer, "daily_double_odds", daily_double_odds, 1);
	rb_define_method(rbPlayer, "clue_odds", clue_odds, 1);
	rb_define_method(rbPlayer, "final_jeopardy_odds", final_jeopardy_odds, 0);
}