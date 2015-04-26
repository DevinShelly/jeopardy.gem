# Loads mkmf which is used to make makefiles for Ruby extensions
require 'mkmf'

# Give it a name
extension_name = 'jeopardy'

# The destination
# dir_config(extension_name)

# Do the work
$defs.push('-DRDOC_CAN_PARSE_DOCUMENTATION=0')
create_makefile(extension_name)