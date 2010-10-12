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


# = =============================================================== =
# =  Massive unit test to check all possible combinations of input  =
# =  parameters and make sure that the output matches the output    =
# =  from the serial version.                                       =
# = =============================================================== =

ratio <- function(d, w) sum(d$x * w)/sum(d$u * w)
trimmedmean <- function(x, d, trim=0) { return(mean(x[d], trim/length(x))) }

test.bootExample1 <- function() {

  # example from http://www.ats.ucla.edu/stat/r/library/bootstrap.htm 
  data(city)
  #defining the ratio function
  #using the boot function
  set.seed(1337)
  a = boot(city, ratio, R=999, stype="w")
  set.seed(1337)
  b = pboot(city, ratio, R=999, stype="w")
  checkEquals(a,b,"Bootstrap examples 1")

}

test.bootTrim <- function() {
 # http://www.mayin.org/ajayshah/KB/R/documents/boot.html
   set.seed(1337)
   a = boot(discoveries, trimmedmean, R=1000, trim=5)
   set.seed(1337)
   b = pboot(discoveries, trimmedmean, R=1000, trim=5)
   checkEquals(a,b,"Bootstrap Trim example")
}   
