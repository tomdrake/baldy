ddddataframe <- function(data, indices, b, c, myd){
  x = data[indices]
  y = c 
  return(mean(x) + mean(b[,1]))
}

library(boot)
library(RUnit)

 set.seed(27)
library(sprint)
pboot(discoveries, ddddataframe, R=49, b=trees, c=discoveries, myd=sample(1000))
pterminate()

