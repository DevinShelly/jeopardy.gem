Gem::Specification.new "jeopardy", "0.0.26" do |s|
  s.name        = 'jeopardy'
  s.version     = '0.0.26'
  s.date        = '2015-04-29'
  s.summary     = "Jeopardy Game Simulator"
  s.description = "A fast Monte Carlo simulation of the television game show Jeopardy!"
  s.authors     = ["Devin Shelly"]
  s.email       = 'devin@devinshelly.com'
  s.homepage    =
    'http://rubygems.org/gems/jeopardy'
  s.license       = 'MIT'
  s.extensions = %w[ext/jeopardy/extconf.rb]
  s.files = ["lib/jeopardy.rb", "ext/jeopardy/jeopardy.c", "ext/jeopardy/Clue.c", "ext/jeopardy/Clue.h", "ext/jeopardy/Game.c", "ext/jeopardy/Game.h", "ext/jeopardy/Player.c", "ext/jeopardy/Player.h", "ext/jeopardy/MinMax.h"]
end