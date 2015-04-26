require 'set'

module Jeopardy

  class Game
    
    attr_accessor :jeopardy_clues, :double_jeopardy_clues, :final_jeopardy_clue, :players, :previous_daily_double_column, :jeopardy_daily_doubles_left, :double_jeopardy_daily_doubles_left
    
    ##
    # Initializes a new Jeopardy::Game with an optional hash containing the following keys:
    #
    # [:jeopardy_clues] An array containing the Jeopardy::Clue instances remaining in the Jeopardy! round (default: a full round)
    # [:double_jeopardy_clues]  An array containing the Jeopardy::Clue instances remaining in Double Jeopardy! (default: a full round)
    # [:players]  An array containing three Jeopardy::Player instances (default: three average players with scores of zero)
    # [:jeopardy_daily_doubles_left]  The number of Daily Doubles yet to be uncovered in the Jeopardy! round (default : 1)
    # [:double_jeopardy_daily_doubles_left] The number of Daily Doubles yet to be uncovered in the Double Jeopardy! round (default: 2)
    # [:previous_daily_double_column] The column that the first DJ Daily Double was found in, eliminating any remaining clues in that column from potentially being the remaining Daily Double (default: nil)
    
    def initialize(options = {})
      options[:jeopardy_clues] ||= 0.upto(29).map { |location| Jeopardy::Clue.new(row: location.to_i%5, column: location.to_i/5, value: location.to_i%5*200+200) }
      options[:double_jeopardy_clues] ||= 0.upto(29).map { |location| Jeopardy::Clue.new(round: 2, row: location.to_i%5, column: location.to_i/5, value: location.to_i%5*400+400) }
      options[:players] ||= [Jeopardy::Player.new, Jeopardy::Player.new, Jeopardy::Player.new]
      options[:jeopardy_daily_doubles_left] ||= 1
      options[:double_jeopardy_daily_doubles_left] ||= 2
      
      @jeopardy_clues = options[:jeopardy_clues]
      @double_jeopardy_clues = options[:double_jeopardy_clues]
      @final_jeopardy_clue = Jeopardy::Clue.new(round: 3, value: 0)
      @players = options[:players]
      @previous_daily_double_column = options[:previous_daily_double_column]
      @jeopardy_daily_doubles_left = options[:jeopardy_daily_doubles_left]
      @double_jeopardy_daily_doubles_left = options[:double_jeopardy_daily_doubles_left]
    end
    
    ##
    # Simulates a Jeopardy::Game with an options hash containing the following keys:
    #
    # [:trials] The number of times the Monte Carlo sim will be run (default: 1)
    # [:seed] The seed which is fed to the random number generators, or nil for a time based seed (default: nil)
    #
    # Returns a hash with Jeopardy::Game#players as keys and their respective wins
    # as values.
    #
    # *Usage*
    #
    # [<tt>Jeopardy::Game.new.simulate(seed: 0)</tt>] #=>	<tt>{ Jeopardy::Game.players[0] => 0, Jeopardy::Game.players[1] => 0, Jeopardy::Game.players[2] => 1 }</tt>
    # [<tt>Jeopardy::Game.new.simulate(seed: 0, trials: 1000)</tt>] #=>	<tt>{ Jeopardy::Game.players[0] => 338, Jeopardy::Game.players[1] => 343,	Jeopardy::Game.players[2] => 319 }</tt>
    
    def simulate(options = {})
      #empty, just used a placeholder for documentation
      #actual implementation is in jeopardy.c
    end
    
    ##
    # Resets the games scores to the values found in the given array. Returns self so that a simulate call may be chained
    #
    # [:scores] Three scores corresponding to the new score for each player (default: [0, 0, 0])
    #
    # *Usage*
    #
    # [<tt>g = Jeopardy::Game.new</tt>]
    # [<tt>g.simulate</tt>]
    # [<tt>g.reset_scores([0, 0, 0])</tt>] #=>  <tt>g</tt>
    
    def reset_scores(scores = [0, 0, 0])
      @players.each_with_index {|player, index| player.score = scores[index]}
      return self
    end
    
    ##
    # Returns an array containing the Daily Doubles with the following parameter:
    #
    # [:rounds] An array or integer declaring which round(s) to look for Daily Doubles in (default: [1,2])
    # 
    # Note: Daily Doubles are not determined until after a call to Jeopardy::Game#simulate.
    #
    # *Usage*
    #
    # [<tt>g = Jeopardy::Game.new</tt>]
    # [<tt>g.simulate(seed: 0)</tt>]
    # [<tt>g.daily_doubles([1,2])</tt>] #=>  <tt>[<Jeopardy::Clue (@round = 1)>, <Jeopardy::Clue (@round = 2)>, <Jeopardy::Clue (@round = 2)>]</tt>
    # [<tt>g.daily_doubles(1)</tt>] #=>  <tt>[<Jeopardy::Clue (@round = 1)>]</tt>
    
    def daily_doubles(rounds = [1, 2])
      rounds = Set.new([rounds].flatten)
      all_clues = @jeopardy_clues + @double_jeopardy_clues
      all_clues.select {|clue| clue.wagers.count > 0 && rounds.include?(clue.round)}
    end
    
    ##
    # Returns a hash containing the Jeopardy::Game#players as keys and the corresponding Coryat scores as values.
    #
    # Note: Coryat scores cannot be calculated prior to a call to Jeopardy::Game#simulate
    #
    # *Usage*
    #
    # [<tt>g = Jeopardy::Game.new</tt>]
    # [<tt>g.simulate</tt>]
    # [<tt>g.coryats</tt>] #=> <tt>{ g.players[0] => 14_400, g.players[1] => 15_800, g.players[2] => 14_000 }</tt>
    
    def coryats
      coryats = {}
      players.each {|player| coryats[player] = 0}
      
      (@jeopardy_clues + @double_jeopardy_clues).each do |clue|
        clue.answers.each do |player, answer|
          coryats[player] += clue.value if answer
          coryats[player] -= clue.value if !answer && clue.wagers.count == 0
        end
      end
      return coryats
    end
    
    ##
    # Returns a hash containing the Jeopardy::Game#players as keys and their corresponding scores as values, with the following optional parameter:
    #
    # rounds: An array or integer declaring which round(s) to calculate scores for (default: [1,2,3])
    #
    # Note: Scores cannot be determined until after a call to Jeopardy::Game#simulate. If the simulation starts in the middle of a round, this will not
    # coincide with the scores found in Jeopardy::Game#players, as those scores include the beginning value. These scores only show the results of the clues
    # simulated.
    #
    # *Usage*
    #
    # [<tt>g = Jeopardy::Game.new</tt>]
    # [<tt>g.simulate(seed: 0)</tt>]
    # [<tt>g.scores(1)</tt>] #=> <tt>{ g.players[0] => 5_200, g.players[2] => 4_600, g.players[3] => 7_400 }</tt>
    # [<tt>g.scores(2)</tt>] #=> <tt>{ g.players[0] => 9_200, g.players[2] => 6_000, g.players[3] => 7_600 }</tt>
    # [<tt>g.scores( [1,2])</tt>]  #=> <tt>{ g.players[0] => 14_400, g.players[2] => 10_600, g.players[3] => 15_000 }</tt>
    # [<tt>g.scores(3)</tt>]  #=> <tt>{ g.players[0] => 6801, g.players[2] => 0, g.players[3] => 13_801 }</tt>
    # [<tt>g.scores([1,2,3])</tt>]  #=> <tt>{ g.players[0] => 21_201, g.players[1] => 10_600, g.players[2] => 28_801 }</tt>
    
    def scores(rounds=[1,2,3])
      rounds = Set.new([rounds].flatten)
      all_clues = @jeopardy_clues + @double_jeopardy_clues + [@final_jeopardy_clue]
      clues_for_rounds = all_clues.select {|clue| rounds.include?(clue.round)}
      scores = {}
      players.each {|player| scores[player] = 0}
      clues_for_rounds.each do |clue|
        clue.answers.each do |player, answer|
          wager = clue.wagers[player]
          scores[player] += wager if !wager.nil? && answer
          scores[player] -= wager if !wager.nil? && !answer
          scores[player] += clue.value if wager.nil? && answer
          scores[player] -= clue.value if wager.nil? && !answer
        end
      end
      return scores
    end
    
  end

  class Player

    attr_accessor :score, :buzzer_rating, :knowledge_rating, :dd_fj_rating, :confidence_rating

    ##
    # Initializes a new Jeopardy::Game with an optional hash containing the following keys:
    #
    # [:score] The current score of the player.
    # [:buzzer_rating] A rating from zero to infinity of how likely a player is to ring in first. When multiple players ring in, the odds of an individual player ringing in first is equal to their rating divided by the sum of the ratings. (default: 1.0)
    # [:knowledge_rating] A rating from -1.0 to 1.0 of how likely a player is to answer correctly after ringing in. It's a piecewise function with -1.0 ratings always answering incorrectly, 0.0 answering at the Jeopardy! average, and 1.0 always answering correctly. (default: 0.0)
    # [:dd_fj_knowledge] The same as knowledge_rating, but pertaining to Daily Doubles/Final Jeopardy! instead of after ringing in. (default: 0.0)
    # [:confidence_rating] How often a player attempts to ring in. -1.0 will never ring in, 0.0 rings in at an average rate, and 1.0 rings in every clue. (default: 0.0)
    
    def initialize(options = {})
      options[:score] ||= 0
      options[:buzzer_rating] ||= 1.0
      options[:knowledge_rating] ||= 0.0
      options[:dd_fj_rating] ||= 0.0
      options[:confidence_rating] ||= 0.0

      @score = options[:score].to_i
      @buzzer_rating = options[:buzzer_rating].to_f
      @knowledge_rating = options[:knowledge_rating].to_f
      @dd_fj_rating = options[:dd_fj_rating].to_f
      @confidence_rating = options[:confidence_rating].to_f
    end
    
    ##
    # Returns the decimal odds of the Player answering a Daily Double correctly.
    #
    # *Usage*
    #
    # [<tt>Jeopardy::Player.new.odds_of_answering_daily_double(Jeopardy::Clue.new)</tt>] #=>	0.76
    #
    
    def odds_of_answering_daily_double(daily_double)
      #empty, just used a placeholder for documentation
      #actual implementation is in jeopardy.c
    end 

    ##
    # Return the decimal odds of the Player answering an ordinary clue correctly.
    #
    # *Usage*
    #
    # [<tt>Jeopardy::Player.new.odds_of_answering_clue(Jeopardy::Clue.new)</tt>] #=>	0.9316826244
    #
    
    def odds_of_answering_clue(clue)
      #empty, just used a placeholder for documentation
      #actual implementation is in jeopardy.c
    end
  end

  class Clue

    attr_accessor :round, :row, :column, :value, :answers, :wagers
    
    ##
    # Initializes a new Jeopardy::Game with an optional hash containing the following keys:
    #
    # [:round]  An integer corresponding to the round the clue is in. 1 is Jeopardy!, 2 is Double Jeopardy!, 3 is Final Jeopardy! (default: 1)
    # [:row]  The row on the game board the clue is found. Zero is the topmost row, four the bottom row. Meaningless for a FJ! clue. (default: 0)
    # [:column] The column on the game board. Zero is the leftmost column, five the rightmost. Again, meaningless for a FJ! clue. (default: 0)
    # [:answers]  A hash containing each player who answered as the key and true/false for a correct/incorrect answer. (default: {})
    # [:wagers] A hash containing each player who wagered as the key and their wager as the value. Should be empty except for Daily Doubles/FJ. (default: {})
    
    def initialize(options = {})
      options[:round] ||= 1
      options[:row] ||= 0
      options[:column] ||= 0
      options[:wagers] ||= {}
      options[:answers] ||= {}
      
      @round = options[:round]
      @row = options[:row]
      @column = options[:column]
      @value = (options[:row]*200 + 200) * options[:round] if options[:round] == 1 || options[:round] == 2
      @value = 0 if @value.nil?
      @answers = options[:answers]
      @wagers = options[:wagers]
    end
  end
end

require "jeopardy/jeopardy"