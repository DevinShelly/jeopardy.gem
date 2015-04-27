Gem::Specification.new "jeopardy", "0.0.4" do |s|
  s.name        = 'jeopardy'
  s.version     = '0.0.4'
  s.date        = '2015-04-26'
  s.summary     = "Jeopardy Game Simulator"
  s.description = "A fast Monte Carlo simulation of the television game show Jeopardy!"
  s.authors     = ["Devin Shelly"]
  s.email       = 'devin@devinshelly.com'
  s.homepage    =
    'http://rubygems.org/gems/jeopardy'
  s.license       = 'MIT'
  s.extensions = %w[ext/jeopardy/extconf.rb]
  s.files = ["lib/jeopardy.rb", "lib/jeopardy/jeopardy.bundle"]
end