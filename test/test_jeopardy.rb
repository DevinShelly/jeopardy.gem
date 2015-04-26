require 'minitest/autorun'
require "jeopardy"

module JeopardyTest
  class Game < Minitest::Test
    
    def players(scores)
      players = []
      scores.each do |score|
        players << Jeopardy::Player.new(score: score)
      end
      return players
    end
    
    def final_jeopardy_game(scores)
      g = Jeopardy::Game.new(jeopardy_clues: [], double_jeopardy_clues: [], players: players(scores))
      g.simulate
      return g
    end
    
    def final_jeopardy_wagers(game)
      wagers = [];
      game.players.each do |player|
        wagers << game.final_jeopardy_clue.wagers[player]
      end
      return wagers
    end
      
    def test_final_jeopardy
      
      g = final_jeopardy_game([10000, 8000, 2000])
      w = final_jeopardy_wagers(g)
      assert_equal 6001, w[0], "Player failed to cover from first position"
      assert_equal 0, w[1], "Player in second place failed to stand pat to win on a miss"
      
      g = final_jeopardy_game([10000, 10000, 10000])
      w = final_jeopardy_wagers(g)
      assert_equal 10000, w.inject(:+)/3, "Player did not wager everything in a three way tie #{g.final_jeopardy_clue.inspect}"
      
      g = final_jeopardy_game([10000, 5000, 1000])
      w = final_jeopardy_wagers(g)
      assert_equal 1, w[0], "Player did not wager 1 to break a lock tie situation"
      assert_equal 5000, w[1], "Player did not wager everything in a must get it right situation"
      
      g = final_jeopardy_game([8000, 5000, 5000])
      w = final_jeopardy_wagers(g)
      assert_equal 5000, w[1..2].inject(:+) / 2, "Players tied for second did not wager everything"
      
      g = final_jeopardy_game([8000, 2000, 1000])
      w = final_jeopardy_wagers(g)
      assert_equal 3999, w[0], "Player failed to wager the maximum in a lock game"
      assert_equal 0, w[1], "Player failed to cover third when first is out of reach"
      
      g = final_jeopardy_game([12400, 9000, 5200])
      w = final_jeopardy_wagers(g)
      assert_equal 1401, w[1], "Player failed to cover third place doubling up"
      assert_equal 5200, w[2], "Player failed to wager everything from third when needed"
      
      g = final_jeopardy_game([10000, 9000, 8000])
      w = final_jeopardy_wagers(g)
      assert_equal(0, w[2], "Player three failed to stand pat when doing so would win a triple stumper")
    end
    
    def test_obvious_things
      
      g = Jeopardy::Game.new
      g.simulate
      simulated_scores = g.players.map {|player| player.score}
      assert_equal g.scores.values, simulated_scores, "The calculated scores did not match the simulated scores: #{g.scores.values}, #{simulated_scores}"
      
      trials = 1000
      g = Jeopardy::Game.new(players: players([1_000_000, 0, 0]))
      assert_equal trials, g.simulate(trials: trials).values[0], "Player with an absurdly large starting score somehow lost."

      g = Jeopardy::Game.new(players: players([-1_000_000, 0, 0]))
      assert_equal 0, g.simulate(trials: trials).values[0], "Player with an absurdly low starting score somehow won a game."
    end
    
    def test_daily_doubles

      g = Jeopardy::Game.new
      g.simulate
      assert_equal 1, g.daily_doubles(1).count, "Default game has zero jeopardy round daily doubles."
      assert_equal 2, g.daily_doubles(2).count, "Default game does not have two DJ daily doubles"
      
      g = Jeopardy::Game.new(jeopardy_daily_doubles_left: 0, double_jeopardy_daily_doubles_left: 0)
      g.simulate
      assert_equal 0, g.daily_doubles.count, "Game initialized with zero daily doubles has some"
      
      p = players([5000, 0, 0])
      g = Jeopardy::Game.new(jeopardy_clues: [Jeopardy::Clue.new], players: p)
      g.simulate
      assert_equal 5000, g.daily_doubles[0].wagers[p[0]], "Player did not wager everything on the jeopardy DD #{g.jeopardy_clues[-1].inspect}"
      
      p = players([30000, 5000, 5000])
      g = Jeopardy::Game.new(jeopardy_clues: [], double_jeopardy_clues: [Jeopardy::Clue.new(round: 2, row: 3)], players: p)
      
      g = Jeopardy::Game.new(jeopardy_clues:[], double_jeopardy_clues: [Jeopardy::Clue.new(round: 2, row: 3, column: 0), Jeopardy::Clue.new(round: 2, row: 0, column: 1)], previous_daily_double_column: 0)
      g.simulate
      assert_equal 1, g.daily_doubles[0].column, "The second Daily Double was placed in the same column as the first"
    end
  
  end
end