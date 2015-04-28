# Loads mkmf which is used to make makefiles for Ruby extensions
require 'mkmf'

# Give it a name
extension_name = 'jeopardy/jeopardy'

$CFLAGS += " -std=c99"

create_makefile(extension_name)