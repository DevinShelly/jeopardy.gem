require "rake/extensiontask"

Rake::ExtensionTask.new "jeopardy" do |ext|
  ext.lib_dir = "lib/jeopardy"
end

require 'rake/testtask'

Rake::TestTask.new do |t|
  t.libs << 'test'
end

desc "Run tests"
task :default => :test