##########################################################################
#                                                                        #
#  SPRINT: Simple Parallel R INTerface                                   #
#  Copyright © 2008,2009 The University of Edinburgh                     #
#                                                                        #
#  This program is free software: you can redistribute it and/or modify  #
#  it under the terms of the GNU General Public License as published by  #
#  the Free Software Foundation, either version 3 of the License, or     #
#  any later version.                                                    #
#                                                                        #
#  This program is distributed in the hope that it will be useful,       #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
#  GNU General Public License for more details.                          #
#                                                                        #
#  You should have received a copy of the GNU General Public License     #
#  along with this program. If not, see <http://www.gnu.or/licenses/>.   #
#                                                                        #
##########################################################################


# These functions are called automatically by the R extensions
# system. They perform some R-side initialisation, then call
# down to the library initialisation.

# Called when the extension is first included by an R script using
# the 'library("sprint")' command.

.First.lib <- function (lib, pkg) {
    library.dynam("sprint", pkg, lib)
    ver <- read.dcf(file.path(lib, pkg, "DESCRIPTION"), "Version")
    ver <- as.character(ver)	
    cat("SPRINT", ver, "loaded\n") 
}

# Called when the extension is unloaded. This is expected to happen
# when the script terminates and R shuts down.

.Last.lib <- function(libpath){
    .Call("sprint_shutdown");
    library.dynam.unload("sprint", libpath)
}
